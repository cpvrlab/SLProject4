###############################################################################
# File:       ch04_HelloCube.py
# Purpose:    Perspective projection of a wireframe cube drawn with lines.
#             Python/tkinter equivalent of the C# exercise in
#             ../ch04_HelloCube_Net. The matrix pipeline, the cube vertices
#             and the line colors are the same as there.
# Author:     Marcus Hudritsch
# Date:       18-SEP-26
# Copyright:  Marcus Hudritsch, Kirchrain 18, 2572 Sutz
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
###############################################################################
# Setup:      This script uses only the Python standard library, but it needs
#             tkinter, which can NOT be installed with pip. A virtual
#             environment (.venv) takes tkinter from the Python it was
#             created with, so that Python must bring a working tkinter.
#
#             The .venv belongs in THIS folder (ch04_HelloCube_Python), next
#             to this file. "-m venv .venv" creates it in the current folder,
#             and the VS Code terminal starts in the repository root, so
#             always change into this folder first:
#               cd apps/exercises/ch04_HelloCube_Python
#
#   Windows:  Install Python 3 from python.org and keep the option
#             "tcl/tk and IDLE" checked (it is checked by default).
#             Then, in this folder:
#               py -m venv .venv
#               .venv\Scripts\python -c "import tkinter; tkinter._test()"
#               .venv\Scripts\python ch04_HelloCube.py
#             If "import tkinter" fails, rerun the Python installer, choose
#             "Modify" and check "tcl/tk and IDLE".
#
#   macOS:    Do NOT use /usr/bin/python3: its Tk 8.5 shows an empty window.
#             Use Homebrew's Python and add its separate tkinter package.
#             Then, in this folder:
#               brew install python python-tk
#               $(brew --prefix)/bin/python3 -m venv .venv
#               .venv/bin/python -c "import tkinter; tkinter._test()"
#               .venv/bin/python ch04_HelloCube.py
#
#   Linux:    sudo apt install python3-tk python3-venv   (Debian/Ubuntu)
#             Then, in this folder:
#               python3 -m venv .venv
#               .venv/bin/python ch04_HelloCube.py
#
#             The tkinter._test() line opens a small test window; if it shows
#             its buttons, tkinter works.
#             VS Code may not find a .venv this deep in the repository on
#             its own. Either open ch04_HelloCube_Python itself in VS Code, or
#             use "Python: Select Interpreter" > "Enter interpreter path" and
#             choose .venv\Scripts\python.exe (Windows) or .venv/bin/python.
#             Utils.py in this folder additionally needs, in this folder:
#               .venv\Scripts\python -m pip install numpy matplotlib opencv-python
#               .venv/bin/python -m pip install numpy matplotlib opencv-python
###############################################################################

import math
import tkinter as tk


class SLVec3f:
    """A 3D vector with x, y and z"""

    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)


class SLMat4f:
    """
    A 4x4 matrix stored column by column, exactly as in SLMat4f.cs and as
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

    def multiply_vec(self, v):
        """
        Post multiplies the matrix by the vector v and returns a vector
        with the additional perspective division.
        """
        m = self.m
        w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15]
        return SLVec3f((m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12]) / w,
                       (m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13]) / w,
                       (m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]) / w)

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

        if axis_x == 1 and axis_y == 0 and axis_z == 0:      # about x-axis
            m[0] = 1; m[4] = 0;   m[8] = 0
            m[1] = 0; m[5] = ca;  m[9] = -sa
            m[2] = 0; m[6] = sa;  m[10] = ca
        elif axis_x == 0 and axis_y == 1 and axis_z == 0:    # about y-axis
            m[0] = ca;  m[4] = 0; m[8] = sa
            m[1] = 0;   m[5] = 1; m[9] = 0
            m[2] = -sa; m[6] = 0; m[10] = ca
        elif axis_x == 0 and axis_y == 0 and axis_z == 1:    # about z-axis
            m[0] = ca; m[4] = -sa; m[8] = 0
            m[1] = sa; m[5] = ca;  m[9] = 0
            m[2] = 0;  m[6] = 0;   m[10] = 1
        else:                                                # arbitrary axis
            x, y, z = float(axis_x), float(axis_y), float(axis_z)
            length = x * x + y * y + z * z                   # length squared
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

    def scale(self, sx, sy, sz):
        """Post multiplies a scaling matrix defined by [sx,sy,sz]"""
        s = SLMat4f()
        s.m[0] = float(sx)
        s.m[5] = float(sy)
        s.m[10] = float(sz)
        self.multiply(s)

    def frustum(self, l, r, b, t, n, f):
        """Defines a view frustum projection matrix as OpenGL's glFrustum"""
        m = self.m
        m[0] = (2 * n) / (r - l); m[4] = 0; m[8] = (r + l) / (r - l); m[12] = 0
        m[1] = 0; m[5] = (2 * n) / (t - b); m[9] = (t + b) / (t - b); m[13] = 0
        m[2] = 0; m[6] = 0; m[10] = -(f + n) / (f - n); m[14] = (-2 * f * n) / (f - n)
        m[3] = 0; m[7] = 0; m[11] = -1; m[15] = 0

    def perspective(self, fov, aspect, n, f):
        """
        Defines a perspective projection matrix as gluPerspective.

        fov:    vertical field of view angle in degrees (zoom angle)
        aspect: aspect ratio of the viewport = width / height
        n:      distance from the eye to the near clipping plane
        f:      distance from the eye to the far clipping plane
        """
        t = math.tan(math.radians(fov) * 0.5) * n
        b = -t
        r = t * aspect
        l = -r
        self.frustum(l, r, b, t, n, f)

    def viewport(self, x, y, ww, wh, n, f):
        """
        Defines a viewport matrix as OpenGL's glViewport.

        x, y:   left and top window coordinate in pixels
        ww, wh: window width and height in pixels
        n, f:   near and far depth range (default 0 and 1)
        """
        ww2 = ww * 0.5
        wh2 = wh * 0.5
        m = self.m
        # negate the first wh because the window has top-down coordinates
        m[0] = ww2; m[4] = 0;    m[8] = 0;                m[12] = ww2 + x
        m[1] = 0;   m[5] = -wh2; m[9] = 0;                m[13] = wh2 + y
        m[2] = 0;   m[6] = 0;    m[10] = (f - n) * 0.5;   m[14] = (f + n) * 0.5
        m[3] = 0;   m[7] = 0;    m[11] = 0;               m[15] = 1


class HelloCube:
    """
    A window that draws a wire frame cube with a perspective projection.
    The cube spins about the y-axis until you start to orbit it with the mouse.
    """

    def __init__(self, root):
        # Create matrices
        self.model_matrix = SLMat4f()       # Model matrix (object to world)
        self.view_matrix = SLMat4f()        # View matrix (world to camera)
        self.projection_matrix = SLMat4f()  # Projection matrix (camera to NDC)
        self.viewport_matrix = SLMat4f()    # Viewport matrix (NDC to pixels)

        # define the 8 vertices of a cube
        self.v = [
            SLVec3f(-0.5, -0.5,  0.5),      # front lower left
            SLVec3f( 0.5, -0.5,  0.5),      # front lower right
            SLVec3f( 0.5,  0.5,  0.5),      # front upper right
            SLVec3f(-0.5,  0.5,  0.5),      # front upper left
            SLVec3f(-0.5, -0.5, -0.5),      # back lower left
            SLVec3f( 0.5, -0.5, -0.5),      # back lower right
            SLVec3f( 0.5,  0.5, -0.5),      # back upper right
            SLVec3f(-0.5,  0.5, -0.5),      # back upper left
        ]

        self.cam_z = -4.0                   # backwards movement of the camera
        self.rot_angle = 0.0                # initial rotation angle

        # State for the mouse orbit that you have to implement below
        self.mouse_is_down = False
        self.start_x = 0.0
        self.start_y = 0.0
        self.rot_x = 0.0
        self.rot_y = 0.0

        self.root = root
        self.root.title("Hello Cube with Python")
        self.root.geometry("640x480")

        self.canvas = tk.Canvas(root, background="white", highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=True)

        # Connect the event handlers
        self.canvas.bind("<Configure>", self.on_resize)
        self.canvas.bind("<ButtonPress-1>", self.on_mouse_down)
        self.canvas.bind("<B1-Motion>", self.on_mouse_move)
        self.canvas.bind("<ButtonRelease-1>", self.on_mouse_up)
        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)     # Win & macOS
        self.canvas.bind("<Button-4>", self.on_mouse_wheel)       # X11 up
        self.canvas.bind("<Button-5>", self.on_mouse_wheel)       # X11 down

        print("")
        print("--------------------------------------------------------------")
        print("Spinning cube with Python ...")

        self.on_paint()

    def on_resize(self, event):
        """
        Gets called whenever the window is resized. When the window resizes we
        have to redefine the projection matrix as well as the viewport matrix.
        """
        width = max(event.width, 1)
        height = max(event.height, 1)
        aspect = width / height
        self.projection_matrix.perspective(50, aspect, 1.0, 3.0)
        self.viewport_matrix.viewport(0, 0, width, height, 0, 1)

    def on_paint(self):
        """The paint routine where all drawing happens"""
        # start with identity every frame
        self.view_matrix.identity()

        # view transform: move the coordinate system away from the camera
        self.view_matrix.translate(0, 0, self.cam_z)

        # model transform: rotate the coordinate system increasingly
        self.model_matrix.identity()
        self.rot_angle += 0.1
        self.model_matrix.rotate(self.rot_angle, 0, 1, 0)
        self.model_matrix.scale(2, 2, 2)

        # build combined matrix out of viewport, projection & modelview matrix
        m = SLMat4f()
        m.multiply(self.viewport_matrix)
        m.multiply(self.projection_matrix)
        m.multiply(self.view_matrix)
        m.multiply(self.model_matrix)

        # transform all vertices into screen space (x & y in px, z as the depth)
        v2 = [m.multiply_vec(vertex) for vertex in self.v]

        self.canvas.delete("all")

        # draw front square
        self.draw_line(v2, 0, 1, "red")
        self.draw_line(v2, 1, 2, "red")
        self.draw_line(v2, 2, 3, "red")
        self.draw_line(v2, 3, 0, "red")
        # draw back square
        self.draw_line(v2, 4, 5, "green")
        self.draw_line(v2, 5, 6, "green")
        self.draw_line(v2, 6, 7, "green")
        self.draw_line(v2, 7, 4, "green")
        # draw from the front corners to the back corners
        self.draw_line(v2, 0, 4, "blue")
        self.draw_line(v2, 1, 5, "blue")
        self.draw_line(v2, 2, 6, "blue")
        self.draw_line(v2, 3, 7, "blue")

        # Tell the system that the window should be repainted again
        self.root.after(16, self.on_paint)

    def draw_line(self, v2, i, j, color):
        """Draws one edge of the cube from vertex i to vertex j"""
        self.canvas.create_line(v2[i].x, v2[i].y,
                                v2[j].x, v2[j].y,
                                fill=color, width=2)

    def on_mouse_down(self, event):
        """Handles the mouse down event"""
        # ???

    def on_mouse_move(self, event):
        """Handles the mouse move event"""
        # ???

    def on_mouse_up(self, event):
        """Handles the mouse up event"""
        # ???

    def on_mouse_wheel(self, event):
        """Handles the mouse wheel event"""
        # ???


def main():
    root = tk.Tk()
    HelloCube(root)
    root.mainloop()


if __name__ == "__main__":
    main()
