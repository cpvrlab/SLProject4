###############################################################################
# File:       TextureMapping.py
# Purpose:    Minimal core profile OpenGL application for ambient-diffuse-
#             specular lighting shaders with textures.
#             Python/tkinter equivalent of the C++ exercise in
#             ../ch09_TextureMapping. It uses the same shaders
#             (data/shaders/ch09_TextureMapping.*), the same texture, the same
#             square and the same light and material settings as there.
# Author:     Marcus Hudritsch
# Date:       09-OCT-26
# Copyright:  Marcus Hudritsch, Kirchrain 18, 2572 Sutz
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
###############################################################################
# How it works:
#             tkinter has no OpenGL widget of its own. This script therefore
#             creates an invisible GLFW window only to get an OpenGL 3.3 core
#             profile context. All rendering goes into an offscreen
#             framebuffer object (FBO). After each frame the pixels are read
#             back with glReadPixels and shown in a tkinter canvas. This works
#             the same way on Windows, macOS and Linux. Everything between
#             glClear and glDrawElements is the same OpenGL as in the C++
#             version. The helper functions for shaders, buffers and
#             textures are in glUtils.py next to this file, the Python
#             version of ../glUtils.cpp.
#
# Setup:      tkinter can NOT be installed with pip. A virtual environment
#             (.venv) takes tkinter from the Python it was created with, so
#             that Python must bring a working tkinter. Everything else comes
#             from pip: PyOpenGL, glfw, numpy and Pillow.
#
#             The .venv belongs in THIS folder (ch09_TextureMapping_Python),
#             next to this file. "-m venv .venv" creates it in the current
#             folder, and the VS Code terminal starts in the repository root,
#             so always change into this folder first:
#               cd apps/exercises/ch09_TextureMapping_Python
#
#   Windows:  Install Python 3 from python.org and keep the option
#             "tcl/tk and IDLE" checked (it is checked by default).
#             Then, in this folder:
#               py -m venv .venv
#               .venv\Scripts\python -m pip install PyOpenGL glfw numpy Pillow
#               .venv\Scripts\python ch09_TextureMapping.py
#             If "import tkinter" fails, rerun the Python installer, choose
#             "Modify" and check "tcl/tk and IDLE".
#
#   macOS:    Do NOT use /usr/bin/python3: its Tk 8.5 shows an empty window.
#             Use Homebrew's Python and add its separate tkinter package.
#             Then, in this folder:
#               brew install python python-tk
#               $(brew --prefix)/bin/python3 -m venv .venv
#               .venv/bin/python -m pip install PyOpenGL glfw numpy Pillow
#               .venv/bin/python ch09_TextureMapping.py
#
#   Linux:    sudo apt install python3-tk python3-venv   (Debian/Ubuntu)
#             Then, in this folder:
#               python3 -m venv .venv
#               .venv/bin/python -m pip install PyOpenGL glfw numpy Pillow
#               .venv/bin/python ch09_TextureMapping.py
#
#             The shaders and the texture are read from the data folder of
#             the repository. It is filled by the first CMake configure of
#             SLProject4 (see BUILD.md).
#             VS Code may not find a .venv this deep in the repository on
#             its own. Either open ch09_TextureMapping_Python itself in
#             VS Code, or use "Python: Select Interpreter" > "Enter
#             interpreter path" and choose .venv\Scripts\python.exe (Windows)
#             or .venv/bin/python.
###############################################################################

import math
import sys
import time
import tkinter as tk
from pathlib import Path

import glfw
import numpy as np
from OpenGL.GL import *

import glUtils

# Root folder of the SLProject4 repository with the data folder
PROJECT_ROOT = Path(__file__).resolve().parents[3]


class SLVec3f:
    """A 3D vector with x, y and z"""

    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)

    def __neg__(self):
        return SLVec3f(-self.x, -self.y, -self.z)


class SLMat4f:
    """
    A 4x4 matrix stored column by column, exactly as in SLMat4.h and as
    OpenGL expects it:

            | 0  4  8 12 |
            | 1  5  9 13 |
        M = | 2  6 10 14 |
            | 3  7 11 15 |

    Vectors are column vectors, so transformations are applied right to left:
    to transform v by M1, then M2, then M3 the product is M3 * M2 * M1 * v.
    """

    def __init__(self):
        self.m = [0.0] * 16
        self.identity()

    def identity(self):
        """Sets the matrix to the identity matrix"""
        self.m = [1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  0.0, 0.0, 0.0, 1.0]

    def multiply(self, a):
        """Post multiplies the matrix by the matrix a"""
        m, am = self.m, a.m
        r = [0.0] * 16
        for col in range(4):
            for row in range(4):
                r[col * 4 + row] = (m[row] * am[col * 4] +
                                    m[4 + row] * am[col * 4 + 1] +
                                    m[8 + row] * am[col * 4 + 2] +
                                    m[12 + row] * am[col * 4 + 3])
        self.m = r

    def multiply_vec4(self, v, w=1.0):
        """Returns the 4D product M * [v.x, v.y, v.z, w] as a list"""
        m = self.m
        return [m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * w,
                m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * w,
                m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * w,
                m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * w]

    def multiply_mat3(self, v):
        """Returns the upper left 3x3 matrix times v (rotation only)"""
        return self.multiply_vec4(v, 0.0)[:3]

    def translation(self):
        """Returns the translation part of the matrix"""
        return SLVec3f(self.m[12], self.m[13], self.m[14])

    def axis_z(self):
        """Returns the z-axis of the matrix (3rd column)"""
        return SLVec3f(self.m[8], self.m[9], self.m[10])

    def inverted(self):
        """Returns the inverse of the matrix by Gauss-Jordan elimination"""
        # a[row] = [4 matrix columns | 4 identity columns]
        a = [[self.m[col * 4 + row] for col in range(4)] +
             [1.0 if i == row else 0.0 for i in range(4)] for row in range(4)]
        for c in range(4):
            pivot = max(range(c, 4), key=lambda r: abs(a[r][c]))
            if abs(a[pivot][c]) < 1e-12:
                raise ValueError("SLMat4f.inverted: matrix is singular")
            a[c], a[pivot] = a[pivot], a[c]
            p = a[c][c]
            a[c] = [x / p for x in a[c]]
            for r in range(4):
                if r != c and a[r][c] != 0.0:
                    f = a[r][c]
                    a[r] = [x - f * y for x, y in zip(a[r], a[c])]
        inv = SLMat4f()
        inv.m = [a[row][4 + col] for col in range(4) for row in range(4)]
        return inv

    def translate(self, tx, ty, tz):
        """Post multiplies a translation matrix defined by [tx,ty,tz]"""
        tr = SLMat4f()
        tr.m[12] = float(tx)
        tr.m[13] = float(ty)
        tr.m[14] = float(tz)
        self.multiply(tr)

    def rotate(self, deg_ang, axis_x, axis_y, axis_z):
        """Post multiplies a rotation matrix of deg_ang degrees about an axis"""
        r = SLMat4f()
        rad_ang = math.radians(deg_ang)
        ca = math.cos(rad_ang)
        sa = math.sin(rad_ang)
        m = r.m

        x, y, z = float(axis_x), float(axis_y), float(axis_z)
        length = x * x + y * y + z * z                       # length squared
        if length != 0 and (length > 1.0001 or length < 0.9999):
            length = 1 / math.sqrt(length)
            x *= length; y *= length; z *= length
        xy, yz, xz = x * y, y * z, x * z
        xx, yy, zz = x * x, y * y, z * z
        m[0] = xx + ca * (1 - xx)
        m[4] = xy - xy * ca - z * sa
        m[8] = xz - xz * ca + y * sa
        m[1] = xy - xy * ca + z * sa
        m[5] = yy + ca * (1 - yy)
        m[9] = yz - yz * ca - x * sa
        m[2] = xz - xz * ca - y * sa
        m[6] = yz - yz * ca + x * sa
        m[10] = zz + ca * (1 - zz)

        self.multiply(r)

    def frustum(self, l, r, b, t, n, f):
        """Defines a view frustum projection matrix as OpenGL's glFrustum"""
        m = self.m
        m[0] = (2 * n) / (r - l); m[4] = 0; m[8] = (r + l) / (r - l); m[12] = 0
        m[1] = 0; m[5] = (2 * n) / (t - b); m[9] = (t + b) / (t - b); m[13] = 0
        m[2] = 0; m[6] = 0; m[10] = -(f + n) / (f - n); m[14] = (-2 * f * n) / (f - n)
        m[3] = 0; m[7] = 0; m[11] = -1; m[15] = 0

    def perspective(self, fov, aspect, n, f):
        """Defines a perspective projection matrix as gluPerspective"""
        t = math.tan(math.radians(fov) * 0.5) * n
        b = -t
        r = t * aspect
        l = -r
        self.frustum(l, r, b, t, n, f)


###############################################################################

class TextureMapping:
    """
    A window that draws a textured square lit by a point light with per pixel
    Blinn-Phong lighting. Drag with the left mouse button to rotate the
    camera, hold the right button to see the wireframe, and use the mouse
    wheel to move the camera forward or backward.
    """

    SAMPLES = 4         # samples per pixel for full screen anti aliasing

    def __init__(self, root, width, height):
        self.root = root
        self.root.title("Texture Mapping")
        self.root.geometry(f"{width}x{height}")

        # Global application variables
        self.camera_matrix = SLMat4f()      # camera to world transform
        self.view_matrix = SLMat4f()        # world to camera transform
        self.model_matrix = SLMat4f()       # model to world transform
        self.light_matrix = SLMat4f()       # light to world transform
        self.projection_matrix = SLMat4f()  # view space to normalized device coords.

        self.vao = 0                        # ID of the vertex array object
        self.vbo_v = 0                      # ID of the VBO for vertex attributes
        self.vbo_i = 0                      # ID of the VBO for vertex index array
        self.num_v = 0                      # NO. of vertices
        self.num_i = 0                      # NO. of vertex indexes for triangles

        self.resolution = 16                # resolution of sphere stack & slices

        self.mouse_left_down = False        # Flag if mouse is down
        self.modifiers = set()              # pressed modifier keys

        self.frame_times = [0.0] * 60       # frame times for the FPS average
        self.frame_no = 0
        self.last_time_sec = time.perf_counter()

        self.fbo_size = (0, 0)              # size of the offscreen framebuffers
        self.fbo_ms = self.fbo = 0          # multisample & resolve framebuffer
        self.rbos = []                      # their color & depth renderbuffers
        self.after_id = None

        self.canvas = tk.Canvas(root, background="black", highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.photo = tk.PhotoImage(width=width, height=height)
        self.canvas.create_image(0, 0, image=self.photo, anchor=tk.NW)

        self.init_glfw()
        print("")
        print("--------------------------------------------------------------")
        glUtils.print_gl_info()
        self.on_init()
        self.on_resize_gl(width, height)

        # Connect the event handlers
        self.canvas.bind("<Configure>", self.on_resize)
        self.canvas.bind("<ButtonPress>", self.on_mouse_button)
        self.canvas.bind("<ButtonRelease>", self.on_mouse_button)
        self.canvas.bind("<Motion>", self.on_mouse_move)
        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)     # Win & macOS
        self.root.bind("<KeyPress>", self.on_key)
        self.root.bind("<KeyRelease>", self.on_key)
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

        self.on_paint()

    def init_glfw(self):
        """
        Creates an invisible GLFW window only to get an OpenGL core profile
        context. The rendering goes into our own framebuffer objects.
        """
        glfw.init_hint(glfw.COCOA_MENUBAR, glfw.FALSE)
        if not glfw.init():
            print("Failed to initialize GLFW")
            sys.exit(1)

        glfw.window_hint(glfw.VISIBLE, glfw.FALSE)
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        self.gl_window = glfw.create_window(16, 16, "TextureMapping", None, None)
        if not self.gl_window:
            glfw.terminate()
            print("Failed to create an OpenGL 3.3 core profile context")
            sys.exit(1)

        # Get the current GL context. After this you can call GL
        glfw.make_context_current(self.gl_window)

    def build_sphere(self, radius, stacks, slices):
        """
        build_sphere creates the vertex attributes for a sphere and creates
        the VBO at the end. The sphere is built in stacks & slices. The
        slices are around the z-axis.
        """
        assert stacks > 3 and slices > 3

        # Spherical to cartesian coordinates
        # dtheta = PI  / stacks
        # dphi = 2 * PI / slices
        # x = r*sin(theta)*cos(phi)
        # y = r*sin(theta)*sin(phi)
        # z = r*cos(theta)

        # Create vertex list with 8 floats per vertex (position, normal, texCoord)
        vertices = []
        # ???

        # create index list
        indices = []
        # ???

        if vertices and indices:
            self.num_v = len(vertices) // 8
            self.num_i = len(indices)
            self.vao, self.vbo_v, self.vbo_i = glUtils.build_vao(
                self.vao, self.vbo_v, self.vbo_i,
                vertices, indices,
                self.shader_prog_id,
                self.p_loc,
                attribute_normal_loc=self.n_loc,
                attribute_tex_coord_loc=self.uv_loc)
        else:
            print("**** You have to define some vertices and indices first in build_sphere! ****")

    def build_square(self):
        """
        build_square creates the vertex attributes for a textured square and
        VBO. The square lies in the x-z-plane and is facing towards -y
        (downwards).
        """
        # create vertex array for interleaved position, normal and texCoord
        #           Position,     Normal,    texCrd,
        self.num_v = 4
        vertices = [-1, 0, -1,    0, -1, 0,  0, 0,   # Vertex 0
                     1, 0, -1,    0, -1, 0,  1, 0,   # Vertex 1
                     1, 0,  1,    0, -1, 0,  1, 1,   # Vertex 2
                    -1, 0,  1,    0, -1, 0,  0, 1]   # Vertex 3

        # create index array for GL_TRIANGLES
        self.num_i = 6
        indices = [0, 1, 2, 0, 2, 3]

        # Generate the OpenGL vertex array object
        self.vao, self.vbo_v, self.vbo_i = glUtils.build_vao(
            self.vao, self.vbo_v, self.vbo_i,
            vertices, indices,
            self.shader_prog_id,
            self.p_loc,
            attribute_normal_loc=self.n_loc,
            attribute_tex_coord_loc=self.uv_loc)

    def calc_fps(self, delta_time):
        """Determines the frames per second by averaging 60 frames"""
        self.frame_times[self.frame_no % len(self.frame_times)] = delta_time
        self.frame_no += 1
        frame_time_sec = sum(self.frame_times) / len(self.frame_times)
        return 1 / frame_time_sec if frame_time_sec > 0 else 0.0

    def on_init(self):
        """
        on_init initializes the global variables and builds the shader
        program. It is called after a valid OpenGL context is present.
        """
        # Set light parameters
        self.global_ambi = [0.0, 0.0, 0.0, 1.0]
        self.light_ambient = [0.1, 0.1, 0.1, 1.0]
        self.light_diffuse = [1.0, 1.0, 1.0, 1.0]
        self.light_specular = [1.0, 1.0, 1.0, 1.0]
        self.light_matrix.translate(0, 0, 3)
        self.light_spot_deg = 180.0         # point light
        self.light_spot_exp = 1.0
        self.light_att = [1.0, 0.0, 0.0]    # constant light attenuation = no attenuation
        self.mat_ambient = [1.0, 1.0, 1.0, 1.0]
        self.mat_diffuse = [1.0, 1.0, 1.0, 1.0]
        self.mat_specular = [1.0, 1.0, 1.0, 1.0]
        self.mat_emissive = [0.0, 0.0, 0.0, 1.0]
        self.mat_shininess = 500.0

        # position of the camera
        self.cam_z = 3.0

        # Mouse rotation parameters
        self.rot_x = 0.0
        self.rot_y = 0.0
        self.delta_x = 0
        self.delta_y = 0
        self.start_x = 0
        self.start_y = 0
        self.mouse_left_down = False

        # Load textures
        self.texture_id = glUtils.build_texture(PROJECT_ROOT / "data/images/textures/earth2048_C.png")

        # Load, compile & link shaders
        self.shader_vert_id = glUtils.build_shader(PROJECT_ROOT / "data/shaders/ch09_TextureMapping.vert", GL_VERTEX_SHADER)
        self.shader_frag_id = glUtils.build_shader(PROJECT_ROOT / "data/shaders/ch09_TextureMapping.frag", GL_FRAGMENT_SHADER)
        self.shader_prog_id = glUtils.build_program(self.shader_vert_id, self.shader_frag_id)

        # Activate the shader program
        glUseProgram(self.shader_prog_id)

        # Get the variable locations (identifiers) within the vertex & pixel shader programs
        prog = self.shader_prog_id
        self.p_loc = glGetAttribLocation(prog, "a_position")
        self.n_loc = glGetAttribLocation(prog, "a_normal")
        self.uv_loc = glGetAttribLocation(prog, "a_uv")
        self.pm_loc = glGetUniformLocation(prog, "u_pMatrix")
        self.vm_loc = glGetUniformLocation(prog, "u_vMatrix")
        self.mm_loc = glGetUniformLocation(prog, "u_mMatrix")
        self.global_ambi_loc = glGetUniformLocation(prog, "u_globalAmbi")
        self.light_pos_vs_loc = glGetUniformLocation(prog, "u_lightPosVS")
        self.light_spot_dir_vs_loc = glGetUniformLocation(prog, "u_lightSpotDir")
        self.light_spot_deg_loc = glGetUniformLocation(prog, "u_lightSpotDeg")
        self.light_spot_cos_loc = glGetUniformLocation(prog, "u_lightSpotCos")
        self.light_spot_exp_loc = glGetUniformLocation(prog, "u_lightSpotExp")
        self.light_ambient_loc = glGetUniformLocation(prog, "u_lightAmbi")
        self.light_diffuse_loc = glGetUniformLocation(prog, "u_lightDiff")
        self.light_specular_loc = glGetUniformLocation(prog, "u_lightSpec")
        self.light_att_loc = glGetUniformLocation(prog, "u_lightAtt")
        self.mat_ambient_loc = glGetUniformLocation(prog, "u_matAmbi")
        self.mat_diffuse_loc = glGetUniformLocation(prog, "u_matDiff")
        self.mat_specular_loc = glGetUniformLocation(prog, "u_matSpec")
        self.mat_emissive_loc = glGetUniformLocation(prog, "u_matEmis")
        self.mat_shininess_loc = glGetUniformLocation(prog, "u_matShin")
        self.mat_tex_diff_loc = glGetUniformLocation(prog, "u_matTexDiff")

        # Build object
        self.build_square()

        # Set some OpenGL states
        glClearColor(0.0, 0.0, 0.0, 1)      # Set the background color
        glEnable(GL_DEPTH_TEST)             # Enables depth test
        glEnable(GL_CULL_FACE)              # Enables the culling of back faces

    def delete_vao(self):
        """Deletes the vertex array and its buffers on the GPU"""
        if self.vao:
            glDeleteVertexArrays(1, [self.vao])
            glDeleteBuffers(2, [self.vbo_v, self.vbo_i])
            self.vao = self.vbo_v = self.vbo_i = 0

    def on_close(self):
        """
        on_close is called when the user closes the window and is used for
        proper deallocation of resources.
        """
        if self.after_id:
            self.root.after_cancel(self.after_id)

        # Delete shaders, programs & textures on GPU
        glDeleteShader(self.shader_vert_id)
        glDeleteShader(self.shader_frag_id)
        glDeleteProgram(self.shader_prog_id)
        glDeleteTextures(1, [self.texture_id])

        # Delete arrays & buffers on GPU
        self.delete_vao()
        self.delete_framebuffers()

        glfw.destroy_window(self.gl_window)
        glfw.terminate()
        self.root.destroy()

    def on_paint(self):
        """
        on_paint does all the rendering for one frame from scratch with
        OpenGL (in core profile).
        """
        glBindFramebuffer(GL_FRAMEBUFFER, self.fbo_ms)

        # 1) Clear the color & depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        # 2a) Model transform: rotate the coordinate system increasingly
        # first around the y- and then around the x-axis. This type of camera
        # transform is called turntable animation.
        self.camera_matrix.identity()
        self.camera_matrix.rotate(self.rot_y + self.delta_y, 0, 1, 0)
        self.camera_matrix.rotate(self.rot_x + self.delta_x, 1, 0, 0)

        # 2b) Move the camera to its position.
        self.camera_matrix.translate(0, 0, self.cam_z)

        # 2c) View transform is world to camera (= inverse of camera matrix)
        self.view_matrix = self.camera_matrix.inverted()

        # 3a) Rotate the model so that we see the square from the front side
        # or the earth from the equator.
        self.model_matrix.identity()
        self.model_matrix.rotate(90, -1, 0, 0)

        # 4a) Transform light position into view space
        light_pos_vs = self.view_matrix.multiply_vec4(self.light_matrix.translation())

        # 4b) The spotlight direction is down the negative z-axis of the light transform
        light_spot_dir_vs = self.view_matrix.multiply_mat3(-self.light_matrix.axis_z())

        # 5) Activate the shader program and pass the uniform variables to the shader
        glUseProgram(self.shader_prog_id)
        glUniformMatrix4fv(self.pm_loc, 1, GL_FALSE, self.projection_matrix.m)
        glUniformMatrix4fv(self.vm_loc, 1, GL_FALSE, self.view_matrix.m)
        glUniformMatrix4fv(self.mm_loc, 1, GL_FALSE, self.model_matrix.m)
        glUniform4fv(self.global_ambi_loc, 1, self.global_ambi)
        glUniform4fv(self.light_pos_vs_loc, 1, light_pos_vs)
        glUniform3fv(self.light_spot_dir_vs_loc, 1, light_spot_dir_vs)
        glUniform1f(self.light_spot_deg_loc, self.light_spot_deg)
        glUniform1f(self.light_spot_cos_loc, math.cos(math.radians(self.light_spot_deg)))
        glUniform1f(self.light_spot_exp_loc, self.light_spot_exp)
        glUniform4fv(self.light_ambient_loc, 1, self.light_ambient)
        glUniform4fv(self.light_diffuse_loc, 1, self.light_diffuse)
        glUniform4fv(self.light_specular_loc, 1, self.light_specular)
        glUniform3fv(self.light_att_loc, 1, self.light_att)
        glUniform4fv(self.mat_ambient_loc, 1, self.mat_ambient)
        glUniform4fv(self.mat_diffuse_loc, 1, self.mat_diffuse)
        glUniform4fv(self.mat_specular_loc, 1, self.mat_specular)
        glUniform4fv(self.mat_emissive_loc, 1, self.mat_emissive)
        glUniform1f(self.mat_shininess_loc, self.mat_shininess)
        glUniform1i(self.mat_tex_diff_loc, 0)

        # 6) Activate the texture and the vertex array
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.texture_id)
        glBindVertexArray(self.vao)

        # 7) Draw model triangles by indexes
        glDrawElements(GL_TRIANGLES, self.num_i, GL_UNSIGNED_INT, None)

        # 8) Copy the rendered image into the tkinter window. This replaces
        # glfwSwapBuffers of the C++ version.
        self.show_framebuffer()

        # Calculate frames per second
        time_now_sec = time.perf_counter()
        fps = self.calc_fps(time_now_sec - self.last_time_sec)
        self.root.title(f"Texture Mapping {fps:3.1f}")
        self.last_time_sec = time_now_sec

        # Repaint again as soon as possible
        self.after_id = self.root.after(1, self.on_paint)

    def show_framebuffer(self):
        """
        Resolves the multisampled framebuffer into the single sampled one,
        reads its pixels back and shows them in the canvas as a PPM image.
        """
        w, h = self.fbo_size
        glBindFramebuffer(GL_READ_FRAMEBUFFER, self.fbo_ms)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self.fbo)
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST)

        glBindFramebuffer(GL_READ_FRAMEBUFFER, self.fbo)
        glPixelStorei(GL_PACK_ALIGNMENT, 1)
        pixels = glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE)

        # OpenGL delivers the bottom row first, an image starts at the top
        rows = np.frombuffer(pixels, dtype=np.uint8).reshape(h, w * 3)[::-1]
        self.photo.configure(data=b"P6 %d %d 255\n" % (w, h) + rows.tobytes(),
                             format="PPM")

    def delete_framebuffers(self):
        """Deletes the offscreen framebuffers and their renderbuffers"""
        if self.fbo:
            glDeleteFramebuffers(2, [self.fbo_ms, self.fbo])
            glDeleteRenderbuffers(len(self.rbos), self.rbos)
            self.fbo_ms = self.fbo = 0
            self.rbos = []

    def build_framebuffers(self, width, height):
        """
        Creates the offscreen framebuffers we render into: a multisampled one
        with a color and a depth buffer, and a single sampled one with only a
        color buffer that we can read the pixels from.
        """
        self.delete_framebuffers()
        self.rbos = list(glGenRenderbuffers(3))
        self.fbo_ms, self.fbo = glGenFramebuffers(2)

        glBindFramebuffer(GL_FRAMEBUFFER, self.fbo_ms)
        glBindRenderbuffer(GL_RENDERBUFFER, self.rbos[0])
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, self.SAMPLES, GL_RGBA8, width, height)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.rbos[0])
        glBindRenderbuffer(GL_RENDERBUFFER, self.rbos[1])
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, self.SAMPLES, GL_DEPTH_COMPONENT24, width, height)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self.rbos[1])

        glBindFramebuffer(GL_FRAMEBUFFER, self.fbo)
        glBindRenderbuffer(GL_RENDERBUFFER, self.rbos[2])
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.rbos[2])

        if glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE:
            print("Failed to create the offscreen framebuffer")
            sys.exit(1)
        self.fbo_size = (width, height)

    def on_resize(self, event):
        """Event handler called on the resize event of the canvas"""
        if (event.width, event.height) != self.fbo_size:
            self.on_resize_gl(event.width, event.height)

    def on_resize_gl(self, width, height):
        """
        Does everything that is dependent on the size and ratio of the window.
        """
        width = max(width, 1)
        height = max(height, 1)

        # define the projection matrix
        self.projection_matrix.perspective(45, width / height, 0.01, 10.0)

        # recreate the offscreen framebuffers and define the viewport
        self.build_framebuffers(width, height)
        glViewport(0, 0, width, height)

    def on_mouse_button(self, event):
        """Mouse button down & release event handler starts and ends mouse rotation"""
        if event.num not in (1, 2, 3):
            return
        # The right mouse button is button 3. Only on macOS with Tk 8.6 it
        # is button 2, so there we accept both.
        right = (2, 3) if sys.platform == "darwin" else (3,)

        self.mouse_left_down = (event.type == tk.EventType.ButtonPress)
        if self.mouse_left_down:
            self.start_x = event.x
            self.start_y = event.y

            # Renders only the lines of a polygon during mouse moves
            if event.num in right:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        else:
            self.rot_x += self.delta_x
            self.rot_y += self.delta_y
            self.delta_x = 0
            self.delta_y = 0

            # Renders filled polygons
            if event.num in right:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

    def on_mouse_move(self, event):
        """Mouse move event handler tracks the mouse delta since touch down"""
        if self.mouse_left_down:
            self.delta_y = self.start_x - event.x
            self.delta_x = self.start_y - event.y

    def on_mouse_wheel(self, event):
        """Mouse wheel event handler that moves the camera forward or backward"""
        if not self.modifiers and event.delta != 0:
            self.cam_z += math.copysign(0.1, event.delta)

    def on_key(self, event):
        """Key event handler handles key down & release events"""
        modifier = event.keysym.split("_")[0]   # e.g. Shift_L -> Shift
        if event.type == tk.EventType.KeyPress:
            if event.keysym == "Escape":
                self.on_close()
            elif event.keysym == "Up":
                pass
                # self.resolution = self.resolution << 1
                # self.build_sphere(1.0, self.resolution, self.resolution)
            elif event.keysym == "Down":
                pass
                # if self.resolution > 4: self.resolution = self.resolution >> 1
                # self.build_sphere(1.0, self.resolution, self.resolution)
            elif modifier in ("Shift", "Control", "Alt", "Option"):
                self.modifiers.add(modifier)
        else:
            self.modifiers.discard(modifier)


def main():
    root = tk.Tk()
    TextureMapping(root, 640, 480)
    root.mainloop()


if __name__ == "__main__":
    main()
