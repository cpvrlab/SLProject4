###############################################################################
# File:       glUtils.py
# Purpose:    General OpenGL utility functions for simple OpenGL demo apps.
#             Python equivalent of ../glUtils.h and ../glUtils.cpp. The C++
#             functions keep their names in snake_case (buildShader becomes
#             build_shader). C++ reference parameters that return an ID
#             (e.g. the vaoID of buildVAO) are return values in Python.
# Author:     Marcus Hudritsch
# Date:       09-OCT-26
# Copyright:  Marcus Hudritsch, Kirchrain 18, 2572 Sutz
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
###############################################################################
# Needs:      PyOpenGL, numpy and Pillow (see ch09_TextureMapping.py)
###############################################################################

import ctypes
import inspect
import sys
from pathlib import Path

import numpy as np
from OpenGL.GL import *
from PIL import Image

_errors = []    # list of already reported errors used in get_gl_error


def print_gl_info():
    """Prints the OpenGL version, GLSL version, renderer and vendor"""
    print("OpenGL Version  :", glGetString(GL_VERSION).decode())
    print("OpenGL GLSL Ver.:", glGetString(GL_SHADING_LANGUAGE_VERSION).decode())
    print("OpenGL Renderer :", glGetString(GL_RENDERER).decode())
    print("OpenGL Vendor   :", glGetString(GL_VENDOR).decode())
    get_gl_error()


def load_shader(filename):
    """
    load_shader loads the ASCII content of a shader file and returns it as a
    string. If the file can not be opened an error message is printed before
    the app exits with code 1.
    """
    try:
        return Path(filename).read_text()
    except OSError:
        print("File open failed:", filename)
        sys.exit(1)


def build_shader_from_source(source, shader_type):
    """
    build_shader_from_source compiles the source code string and returns the
    handle to the internal shader object and the compile success.
    All shaders are written with the initial GLSL version 110 without version
    number in the code and are therefore backwards compatible with the
    compatibility profile from OpenGL 2.1 and OpenGL ES 2 that runs on most
    mobile devices. To be upwards compatible some modifications have to be
    done.
    """
    ver_glsl = glsl_version_no()
    src_version = "#version " + ver_glsl + "\n"

    # Replace "attribute" and "varying" that came in GLSL 310
    if ver_glsl > "120":
        if shader_type == GL_VERTEX_SHADER:
            source = source.replace("attribute", "in       ")
            source = source.replace("varying", "out    ")
        if shader_type == GL_FRAGMENT_SHADER:
            source = source.replace("varying", "in     ")

    # Replace "gl_FragColor" that was deprecated in GLSL 140 (OpenGL 3.1) by
    # a custom out variable. Only shaders that still use gl_FragColor get the
    # extra out variable; a second one would be ambiguous in core profile.
    if ver_glsl > "130":
        if shader_type == GL_FRAGMENT_SHADER and "gl_FragColor" in source:
            source = source.replace("gl_FragColor", "fragColor")
            source = source.replace("void main",
                                    "out vec4 fragColor; \n\nvoid main")

    # Replace deprecated texture functions
    if ver_glsl > "140":
        if shader_type == GL_FRAGMENT_SHADER:
            source = source.replace("texture1D", "texture")
            source = source.replace("texture2D", "texture")
            source = source.replace("texture3D", "texture")
            source = source.replace("textureCube", "texture")

    # Prepend the GLSL version as the first statement in the shader code
    src_complete = src_version + source

    # Compile Shader code
    shader_handle = glCreateShader(shader_type)
    glShaderSource(shader_handle, src_complete)
    glCompileShader(shader_handle)

    # Check compile success
    compile_success = bool(glGetShaderiv(shader_handle, GL_COMPILE_STATUS))

    return shader_handle, compile_success


def build_shader(shader_file, shader_type):
    """
    build_shader loads the shader file and calls build_shader_from_source.
    If the compilation fails the compiler log is printed before the app
    exits with code 1.
    """
    source = load_shader(shader_file)
    shader_id, success = build_shader_from_source(source, shader_type)

    if not success:
        print("**** Compile Error ****")
        print("In File:", shader_file)
        print(glGetShaderInfoLog(shader_id).decode())
        sys.exit(1)

    get_gl_error()
    return shader_id


def _link_program(program_handle):
    """Links the program and exits with the linker log on failure"""
    glLinkProgram(program_handle)

    # Check linker success
    if not glGetProgramiv(program_handle, GL_LINK_STATUS):
        print("**** Link Error ****")
        print(glGetProgramInfoLog(program_handle).decode())
        sys.exit(1)
    return program_handle


def build_program(vert_shader_id, frag_shader_id, geom_shader_id=0):
    """
    build_program creates a program object, attaches the shaders, links them
    and returns the OpenGL handle of the program. The geometry shader is
    optional. If the linking fails the linker log is printed before the app
    exits with code 1.
    """
    # Create program, attach shaders and link them
    program_handle = glCreateProgram()
    glAttachShader(program_handle, vert_shader_id)
    if geom_shader_id:
        glAttachShader(program_handle, geom_shader_id)
    glAttachShader(program_handle, frag_shader_id)
    return _link_program(program_handle)


def build_program_tf(vert_shader_id, frag_shader_id):
    """
    build_program_tf creates a program object, attaches the shaders,
    establishes a connection between the output variables and the transform
    feedback buffers, links them and returns the OpenGL handle of the
    program. If the linking fails the linker log is printed before the app
    exits with code 1.
    """
    # Create program, attach shaders and link them
    program_handle = glCreateProgram()
    glAttachShader(program_handle, vert_shader_id)
    glAttachShader(program_handle, frag_shader_id)

    # Connection between the output variables and the output buffers
    output_names = [b"tf_position", b"tf_velocity", b"tf_startTime",
                    b"tf_initialVelocity", b"tf_rotation"]
    names = (ctypes.c_char_p * len(output_names))(*output_names)
    glTransformFeedbackVaryings(program_handle, len(output_names),
                                ctypes.cast(names, ctypes.POINTER(ctypes.POINTER(GLchar))),
                                GL_INTERLEAVED_ATTRIBS)
    return _link_program(program_handle)


def build_vbo(vbo_id, data, target_type_gl, usage_type_gl):
    """
    Generates a Vertex Buffer Object (VBO) and copies the data into the
    buffer on the GPU and returns its ID. The target_type_gl distincts
    between GL_ARRAY_BUFFER for attribute data and GL_ELEMENT_ARRAY_BUFFER for
    index data. The usage_type_gl distincts between GL_STREAM_DRAW,
    GL_STATIC_DRAW and GL_DYNAMIC_DRAW.

    vbo_id: If vbo_id is not zero the vbo will be deleted first before a new
            one is allocated.
    data:   numpy array with the buffer data. Its size in bytes is the
            numElements * elementSizeBytes of the C++ version.
    """
    assert data is not None and data.size, "data is empty"

    # Delete, generates and activates the VBO
    if vbo_id:
        glDeleteBuffers(1, [vbo_id])
    vbo_id = glGenBuffers(1)
    glBindBuffer(target_type_gl, vbo_id)

    # Copy data to the VBO on the GPU. The data could be deleted afterwards.
    glBufferData(target_type_gl, data.nbytes, data, usage_type_gl)
    return vbo_id


def build_vao(vao_id,
              vbo_id_vertices,
              vbo_id_indices,
              vertices,
              indices,
              shader_program_id,
              attribute_position_loc,
              attribute_color_loc=-1,
              attribute_normal_loc=-1,
              attribute_tex_coord_loc=-1):
    """
    Builds the OpenGL Vertex Array Object (VAO) with its associated vertex
    buffer objects and returns the IDs (vao_id, vbo_id_vertices,
    vbo_id_indices). VAOs were introduced in OpenGL 3.0 and reduce the
    overhead per draw call.
    All vertex attributes (e.g. position, colors, normals, texture coords,
    etc.) are float and are stored in one big VBO.
    We expect the data in following interleaved order:
    - 3 floats for position
    - 3 floats for the color (optional)
    - 3 floats for the normal (optional)
    - 2 floats for a texture coordinate (optional)
    If one of the optional attributes is not in the vertex array its
    attribute location must be -1. Pass them by keyword, e.g.
    attribute_normal_loc=n_loc, so that no location lands in the wrong slot.
    The drawing primitive is not involved in the VAO generation. It is only
    used in the draw call (glDrawElements).

    vao_id, vbo_id_vertices, vbo_id_indices: If not zero they will be deleted
            first before new ones are allocated.
    vertices: flat sequence of floats with the interleaved vertex attributes
    indices:  sequence of unsigned int indices
    """
    vertices = np.asarray(vertices, dtype=np.float32)
    indices = np.asarray(indices, dtype=np.uint32)

    assert shader_program_id
    assert vertices.size and indices.size
    assert attribute_position_loc > -1

    # 1) Generate and bind OpenGL vertex array object
    if vao_id:
        glDeleteVertexArrays(1, [vao_id])
    vao_id = glGenVertexArrays(1)
    glBindVertexArray(vao_id)

    # 2) Generate array buffer vbo for float vertices
    vbo_id_vertices = build_vbo(vbo_id_vertices,
                                vertices,
                                GL_ARRAY_BUFFER,
                                GL_STATIC_DRAW)

    # 3) Generate element array buffer for indices
    vbo_id_indices = build_vbo(vbo_id_indices,
                               indices,
                               GL_ELEMENT_ARRAY_BUFFER,
                               GL_STATIC_DRAW)

    # Tell OpenGL how to interpret the vertex buffer
    # We use an interleaved attribute layout.
    # With vertex position, normals and texture coordinates it would look like this:
    #           |               Vertex 0                |               Vertex 1                |
    # Attribs:  |   Position0  |    Normal0   |TexCoord0|   Position1  |    Normal1   |TexCoord1|
    # Elements: | PX | PY | PZ | NX | NY | NZ | TX | TY | PX | PY | PZ | NX | NY | NZ | TX | TY |
    # Bytes:    |#### #### ####|#### #### ####|#### ####|#### #### ####|#### #### ####|#### ####|
    #           |                                       |
    #           |<--------- size_vertex_bytes = 32 ---->|
    #           |<-------- offsetT = 24 ----->|
    #           |<offsetN = 12>|
    size_vertex_bytes = 3 * 4
    if attribute_color_loc > -1:
        size_vertex_bytes += 3 * 4
    if attribute_normal_loc > -1:
        size_vertex_bytes += 3 * 4
    if attribute_tex_coord_loc > -1:
        size_vertex_bytes += 2 * 4

    # 4) Activate GLSL shader program
    glUseProgram(shader_program_id)

    # 5a) We always must have a position attribute
    glVertexAttribPointer(attribute_position_loc,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          size_vertex_bytes,
                          ctypes.c_void_p(0))
    glEnableVertexAttribArray(attribute_position_loc)
    offset = 3 * 4

    # 5b) If we have colors they are the second attribute with 12 bytes offset
    if attribute_color_loc > -1:
        glVertexAttribPointer(attribute_color_loc,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              size_vertex_bytes,
                              ctypes.c_void_p(offset))
        glEnableVertexAttribArray(attribute_color_loc)
        offset += 3 * 4

    # 5c) If we have normals they follow with 3 floats
    if attribute_normal_loc > -1:
        glVertexAttribPointer(attribute_normal_loc,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              size_vertex_bytes,
                              ctypes.c_void_p(offset))
        glEnableVertexAttribArray(attribute_normal_loc)
        offset += 3 * 4

    # 5d) If we have texture coords they are the last attribute with 2 floats
    if attribute_tex_coord_loc > -1:
        glVertexAttribPointer(attribute_tex_coord_loc,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              size_vertex_bytes,
                              ctypes.c_void_p(offset))
        glEnableVertexAttribArray(attribute_tex_coord_loc)

    return vao_id, vbo_id_vertices, vbo_id_indices


def _load_image(texture_file):
    """
    Loads an image like CVImage: flipped vertically, because OpenGL expects
    the bottom row first. Returns the image and its OpenGL pixel format.
    """
    img = Image.open(texture_file)
    formats = {"L": GL_RED, "RGB": GL_RGB, "RGBA": GL_RGBA}
    if img.mode not in formats:
        img = img.convert("RGBA")
    img = img.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
    return img, formats[img.mode]


def build_texture(texture_file,
                  min_filter=GL_LINEAR_MIPMAP_LINEAR,
                  mag_filter=GL_LINEAR,
                  wrap_s=GL_REPEAT,
                  wrap_t=GL_REPEAT):
    """
    build_texture loads and builds the OpenGL texture on the GPU. The loaded
    image data in the client memory is deleted again. The parameters
    min_filter and mag_filter set the minification and magnification. The
    wrap_s and wrap_t parameters set the texture wrapping mode. See the GL
    spec.
    """
    # load texture image
    img, img_format = _load_image(texture_file)

    # check max. size
    max_size = glGetIntegerv(GL_MAX_TEXTURE_SIZE)
    print("glUtils.build_texture: Max. texture size:", max_size)
    if img.width > max_size or img.height > max_size:
        print("glUtils.build_texture: Texture height is too big.")
        sys.exit(0)

    # generate texture name (= internal texture object)
    texture_handle = glGenTextures(1)

    # bind the texture as the active one
    glBindTexture(GL_TEXTURE_2D, texture_handle)

    # apply minification & magnification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter)

    # apply texture wrapping modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t)

    # Rows of RGB or gray images are not always a multiple of 4 bytes
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)

    # Copy image data to the GPU. The image can be deleted afterwards
    glTexImage2D(GL_TEXTURE_2D,     # target texture type 1D, 2D or 3D
                 0,                 # Base level for mipmapped textures
                 img_format,        # internal format: e.g. GL_RGBA, see spec.
                 img.width,         # image width
                 img.height,        # image height
                 0,                 # border pixels: must be 0
                 img_format,        # data format: e.g. GL_RGBA, see spec.
                 GL_UNSIGNED_BYTE,  # data type
                 img.tobytes())     # image data

    # generate the mipmap levels
    if min_filter >= GL_NEAREST_MIPMAP_NEAREST:
        glGenerateMipmap(GL_TEXTURE_2D)

    get_gl_error()
    return texture_handle


def build_3d_texture(files,
                     min_filter=GL_LINEAR,
                     mag_filter=GL_LINEAR,
                     wrap_r=GL_CLAMP_TO_BORDER,
                     wrap_s=GL_CLAMP_TO_BORDER,
                     wrap_t=GL_CLAMP_TO_BORDER,
                     border_color=(0.0, 0.0, 0.0, 0.0)):
    """
    build_3d_texture builds an OpenGL 3D texture out of a stack of equally
    sized images and returns (texture_id, x_extend, y_extend, z_extend).
    """
    # check max. size
    max_size = glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE)

    assert files

    first, first_format = _load_image(files[0])
    if min(len(files), first.height, first.width) > max_size:
        print("glUtils: Texture is too big in at least one dimension.")
        sys.exit(0)

    # Concatenate the image data in a new buffer
    buffer = bytearray()
    for file in files:
        image, image_format = _load_image(file)
        assert image.height == first.height
        assert image.width == first.width
        assert image_format == first_format
        buffer += image.tobytes()

    # generate texture name (= internal texture object)
    texture_handle = glGenTextures(1)

    # bind the texture as the active one
    glBindTexture(GL_TEXTURE_3D, texture_handle)

    # apply minification & magnification filter
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_filter)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_filter)

    # apply texture wrapping modes
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_s)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_t)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_r)
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, border_color)

    x_extend = first.width
    y_extend = first.height
    z_extend = len(files)

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    glTexImage3D(GL_TEXTURE_3D,     # Copy the new buffer to the GPU
                 0,                 # Mipmap level
                 first_format,      # Internal format
                 x_extend,
                 y_extend,
                 z_extend,
                 0,                 # Border
                 first_format,      # Format
                 GL_UNSIGNED_BYTE,  # Data type
                 bytes(buffer))

    glBindTexture(GL_TEXTURE_3D, 0)
    get_gl_error()

    return texture_handle, x_extend, y_extend, z_extend


def get_gl_error(quit=False):
    """
    Checks if an OpenGL error occurred and prints it once with the file and
    line of the caller. This replaces the GETGLERROR macro of the C++
    version. Note that PyOpenGL already raises an exception on most errors
    by itself.
    """
    err = glGetError()
    if err == GL_NO_ERROR:
        return

    err_str = {GL_INVALID_ENUM: "GL_INVALID_ENUM",
               GL_INVALID_VALUE: "GL_INVALID_VALUE",
               GL_INVALID_OPERATION: "GL_INVALID_OPERATION",
               GL_INVALID_FRAMEBUFFER_OPERATION: "GL_INVALID_FRAMEBUFFER_OPERATION",
               GL_OUT_OF_MEMORY: "GL_OUT_OF_MEMORY"}.get(err, "Unknown error")

    # Build error string as a concatenation of file, line & error
    caller = inspect.stack()[1]
    new_err = f"{caller.filename}: line:{caller.lineno}: {err_str}"

    # Only print errors that do not exist already
    if new_err not in _errors:
        _errors.append(new_err)
        print(f"OpenGL Error in {caller.filename}, line {caller.lineno}: {err_str}",
              file=sys.stderr)

    if quit:
        sys.exit(1)


def glsl_version_no():
    """
    Returns the OpenGL Shading Language version number as a string.
    The string returned by glGetString can contain additional vendor
    information such as the build number and the brand name.
    For the shading language string "Nvidia GLSL 4.5" the function returns
    "450".
    """
    version_str = glGetString(GL_SHADING_LANGUAGE_VERSION).decode()
    dot_pos = version_str.find(".")
    return version_str[dot_pos - 1] + version_str[dot_pos + 1] + "0"
