###############################################################################
# File:       camPinhole.py
# Purpose:    Defines a pinhole camera model and draws a wireframe cube for test
# Author:     Marcus Hudritsch
# Date:       13-OCT-25
# Copyright:  Marcus Hudritsch, Kirchrain 18, 2572 Sutz
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
###############################################################################

import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial.transform import Rotation  # for euler angles to matrix
import Utils as utl


def intrinsic(fov_deg, pixel_width, pixel_height, width_px, height_px):
    """
    Build a 3x4 intrinsic matrix (K | 0) given field-of-view and pixel geometry.

    fov_deg: horizontal field-of-view in degrees
    pixel_width, pixel_height: physical pixel size (e.g., mm per pixel)
    width_px, height_px: image resolution in pixels
    """
    cx = width_px / 2.0
    cy = height_px / 2.0
    sx = pixel_width
    sy = pixel_height

    fov_rad = np.deg2rad(fov_deg)
    # focal length in *physical* units (same px size in x/y allowed but not required)
    f = (width_px * sx) / (2.0 * np.tan(fov_rad / 2.0))

    # focal length in pixels
    fx = f / sx
    fy = f / sy

    K3 = np.array([
        [fx, 0.0, cx],
        [0.0, fy, cy],
        [0.0, 0.0, 1.0]
    ])

    # Extend to 3x4
    K = np.hstack([K3, np.zeros((3, 1))])
    return K


def extrinsic(wTrans_x, wTrans_y, wTrans_z, xRotDEG, yRotDEG, zRotDEG):
    """
    Build world-to-camera 4x4 matrix cTw:

    1) Compose a camera pose in world coords as wTc = T * [R 0; 0 1]
       where rotations are applied in XYZ order (Rx then Ry then Rz).
    2) Return its inverse to get world -> camera.
    """

    T = np.array([
        [1, 0, 0, wTrans_x],
        [0, 1, 0, wTrans_y],
        [0, 0, 1, wTrans_z],
        [0, 0, 0, 1]
    ])

    # Rotation matrix from Euler angles (XYZ order)
    R = Rotation.from_euler(
        'xyz', [xRotDEG, yRotDEG, zRotDEG], degrees=True).as_matrix()

    wTc = T @ np.block([
        [R,               np.zeros((3, 1))],
        [np.zeros((1, 3)),            1.0]
    ])

    cTw = np.linalg.inv(wTc)
    return cTw


def project(P_homog, camera_matrix_3x4):
    """
    P_homog: 4xN (homogeneous world points)
    camera_matrix_3x4: K (3x4) @ cTw (4x4)  -> 3x4
    Returns 2xN pixel coordinates.
    """
    p = camera_matrix_3x4 @ P_homog  # 3xN
    # Normalize homogeneous image coordinates
    p[0, :] /= p[2, :]
    p[1, :] /= p[2, :]
    p[2, :] /= p[2, :]
    return p[0:2, :]


def decompose(C):
    """
    Decompose a 3x4 camera matrix C into intrinsics & extrinsics

        C = K [R | T]

    Returns:
      f : scalar focal length (K[0,0])
      s : np.array([1, K[1,1]/K[0,0]])   # scale factors
      c : np.array([cx, cy])             # principal point
      T : 3-vector (translation in camera coords)
      R : 3x3 rotation (world->camera)
    """
    if C.shape != (3, 4):
        raise ValueError("argument is not a 3x4 matrix")

    # ---- RQ via QR on reversed ----
    M = C[:, :3]
    M = M.T
    # Reverse rows & cols
    Mr = M[::-1, ::-1]
    # QR
    Qr, Rr = np.linalg.qr(Mr)
    # Undo reversals and transposes to get R, K
    R = (Qr.T)[::-1, ::-1]
    K = (Rr.T)[::-1, ::-1]

    # If det(R) < 0, flip first column of K and first row of R
    if np.linalg.det(R) < 0:
        K[:, 0] = -K[:, 0]
        R[0, :] = -R[0, :]

    # Fix signs so intrinsic diagonal is positive using P = diag(sign(diag(K)))
    P = np.diag(np.sign(np.diag(K)))
    # asserts det(P) == 1
    if not np.isclose(np.linalg.det(P), 1.0):
        raise AssertionError("cannot correct signs in the intrinsic matrix")

    K = K @ P
    R = P.T @ R

    # ---- Translation before normalizing K (critical) ----
    a4 = C[:, 3]
    T = np.linalg.inv(K) @ a4

    # ---- Normalize K so K[2,2] == 1 ----
    K = K / K[2, 2]

    # ---- Extract parameters ----
    f = K[0, 0]
    s = np.array([1.0, K[1, 1] / K[0, 0]])  # [1, fy/fx]
    c = K[0:2, 2].copy()                    # [cx, cy]

    return f, s, c, T, R


def rotm2eul_zyx(R):
    """
    ZYX Euler angles from rotation matrix.
    Returns (z, y, x) in radians
    """
    # Handle numerical safety
    sy = -R[2, 0]
    cy_sq = 1 - sy**2
    eps = 1e-12

    if cy_sq > eps:  # not gimbal lock
        z = np.arctan2(R[1, 0], R[0, 0])
        y = np.arcsin(sy)
        x = np.arctan2(R[2, 1], R[2, 2])
    else:  # gimbal lock: cos(y) ~ 0
        # When y ~ ±pi/2, set z = 0 and solve x from first row
        z = 0.0
        y = np.pi/2 if sy >= 0 else -np.pi/2
        x = np.arctan2(-R[0, 1], R[1, 1])
    return np.array([z, y, x])


def hello_cube():
    # Create a camera with 80° FOV, pixel size of 0.001, and resolution 640x480
    K = intrinsic(80.0, 0.001, 0.001, 640, 480)

    # Extrinsics: translate and rotate camera in world
    cTw = extrinsic(0.0, 0.0, -2.0, 0.0, 0.0, 0.0)

    # Full camera matrix C = K * cTw (3x4)
    C = K @ cTw

    # Define cube (8 corners) in homogeneous world coordinates (each column = one point)
    P = np.array([
        [-0.5, -0.5, -0.5, 1.0],
        [-0.5,  0.5, -0.5, 1.0],
        [ 0.5,  0.5, -0.5, 1.0],
        [ 0.5, -0.5, -0.5, 1.0],
        [-0.5, -0.5,  0.5, 1.0],
        [-0.5,  0.5,  0.5, 1.0],
        [ 0.5,  0.5,  0.5, 1.0],
        [ 0.5, -0.5,  0.5, 1.0],
    ], dtype=float).T  # 4x8

    # Project points to pixels
    p = project(P, C).T  # 8x2

    # Plot the cube wireframe
    plt.figure()
    plt.title("Perspective Projection of a Wireframe Cube")
    plt.grid(False)
    plt.xlim(0, 640)
    plt.ylim(0, 480)
    plt.gca().set_aspect('equal', adjustable='box')

    # Front square (red): points 0-1-2-3
    utl.plot_edge(p, 0, 1, 'r')
    utl.plot_edge(p, 1, 2, 'r')
    utl.plot_edge(p, 2, 3, 'r')
    utl.plot_edge(p, 3, 0, 'r')

    # Back square (blue): points 4-5-6-7
    utl.plot_edge(p, 4, 5, 'b')
    utl.plot_edge(p, 5, 6, 'b')
    utl.plot_edge(p, 6, 7, 'b')
    utl.plot_edge(p, 7, 4, 'b')

    # Side connections (green): 0-4, 1-5, 2-p6, 3-7
    utl.plot_edge(p, 0, 4, 'g')
    utl.plot_edge(p, 1, 5, 'g')
    utl.plot_edge(p, 2, 6, 'g')
    utl.plot_edge(p, 3, 7, 'g')

    # If you want image-style with (0,0) top-left, uncomment:
    # plt.gca().invert_yaxis()

    plt.show()


if __name__ == "__main__":
    hello_cube()
