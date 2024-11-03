/**
 * \file      DiffuseSphere.cpp
 * \details   Core profile OpenGL application with diffuse lighted sphere with
 *            GLFW as the OS GUI interface (http://www.glfw.org/).
 * \date      December 2015 (HS15)
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#include <GL/gl3w.h>    // OpenGL headers
#include <GLFW/glfw3.h> // GLFW GUI library
#include <SLMat4.h>     // 4x4 matrix class
#include <SLVec3.h>     // 3D vector class
#include <glUtils.h>    // Basics for OpenGL shaders, buffers & textures

//-----------------------------------------------------------------------------
//! Struct definition for vertex attributes position and normal
struct VertexPN
{
    SLVec3f p; // vertex position [x,y,z]
    SLVec3f n; // vertex normal [x,y,z]

    // Setter method
    void set(float posX, float posY, float posZ, float normalX, float normalY, float normalZ)
    {
        p.set(posX, posY, posZ);
        n.set(normalX, normalY, normalZ);
    }
};
//-----------------------------------------------------------------------------
// Global application variables
GLFWwindow*     window;      //!< The global glfw window handle
static SLstring projectRoot; //!< Directory of executable
SLint           scrWidth;    //!< Window width at start up
SLint           scrHeight;   //!< Window height at start up

static SLMat4f cameraMatrix;     //!< 4x4 matrix for camera to world transform
static SLMat4f viewMatrix;       //!< 4x4 matrix for world to camera transform
static SLMat4f modelMatrix;      //!< 4x4 matrix for model to world transform
static SLMat4f projectionMatrix; //!< Projection from view space to normalized device coordinates

static GLuint vao  = 0; //!< ID of the Vertex Array Object (VAO)
static GLuint vboV = 0; //!< ID of the VBO for vertex array
static GLuint vboI = 0; //!< ID of the VBO for vertex index array

static GLuint numV = 0;      //!< NO. of vertices
static GLuint numI = 0;      //!< NO. of vertex indexes for triangles
static GLint  resolution;    //!< resolution of sphere stack & slices
static GLint  primitiveType; //!< Type of GL primitive to render

static float        camZ;                   //!< z-distance of camera
static float        rotX, rotY;             //!< rotation angles around x & y axis
static int          deltaX, deltaY;         //!< delta mouse motion
static int          startX, startY;         //!< x,y mouse start positions
static int          mouseX, mouseY;         //!< current mouse position
static bool         mouseLeftDown;          //!< Flag if mouse is down
static GLuint       modifiers = 0;          //!< modifier bit flags
static const GLuint NONE      = 0;          //!< constant for no modifier
static const GLuint SHIFT     = 0x00200000; //!< constant for shift key modifier
static const GLuint CTRL      = 0x00400000; //!< constant for control key modifier
static const GLuint ALT       = 0x00800000; //!< constant for alt key modifier

static GLuint shaderVertID = 0; //! vertex shader id
static GLuint shaderFragID = 0; //! fragment shader id
static GLuint shaderProgID = 0; //! shader program id

// Attribute & uniform variable location indexes
static GLint pLoc;              //!< attribute location for vertex position
static GLint nLoc;              //!< attribute location for vertex normal
static GLint pmLoc;             //!< uniform location for projection matrix
static GLint vmLoc;             //!< uniform location for view matrix
static GLint mmLoc;             //!< uniform location for model matrix
static GLint lightSpotDirVSLoc; //!< uniform location for light direction in view space (VS)
static GLint lightDiffuseLoc;   //!< uniform location for diffuse light intensity
static GLint matDiffuseLoc;     //!< uniform location for diffuse light reflection

static const SLfloat PI = 3.14159265358979f;

//-----------------------------------------------------------------------------
/*!
 * Build the vertex and index data for a box and sends it to the GPU
 */
void buildBox()
{
    // create C arrays on heap
    // Define the vertex pos. and normals as an array of structure
    numV = 24;
    std::vector<VertexPN> vertices(numV);
    vertices[0].set(1, 1, 1, 1, 0, 0);
    vertices[1].set(1, 0, 1, 1, 0, 0);
    vertices[2].set(1, 0, 0, 1, 0, 0);
    vertices[3].set(1, 1, 0, 1, 0, 0);
    vertices[4].set(1, 1, 0, 0, 0, -1);
    vertices[5].set(1, 0, 0, 0, 0, -1);
    vertices[6].set(0, 0, 0, 0, 0, -1);
    vertices[7].set(0, 1, 0, 0, 0, -1);
    vertices[8].set(0, 0, 1, -1, 0, 0);
    vertices[9].set(0, 1, 1, -1, 0, 0);
    vertices[10].set(0, 1, 0, -1, 0, 0);
    vertices[11].set(0, 0, 0, -1, 0, 0);
    vertices[12].set(1, 1, 1, 0, 0, 1);
    vertices[13].set(0, 1, 1, 0, 0, 1);
    vertices[14].set(0, 0, 1, 0, 0, 1);
    vertices[15].set(1, 0, 1, 0, 0, 1);
    vertices[16].set(1, 1, 1, 0, 1, 0);
    vertices[17].set(1, 1, 0, 0, 1, 0);
    vertices[18].set(0, 1, 0, 0, 1, 0);
    vertices[19].set(0, 1, 1, 0, 1, 0);
    vertices[20].set(0, 0, 0, 0, -1, 0);
    vertices[21].set(1, 0, 0, 0, -1, 0);
    vertices[22].set(1, 0, 1, 0, -1, 0);
    vertices[23].set(0, 0, 1, 0, -1, 0);

    // Define the triangle indexes of the cubes vertices
    numI = 36;
    std::vector<GLuint> indices(numI);

    int n         = 0;
    indices[n++]  = 0;
    indices[n++]  = 1;
    indices[n++]  = 2;
    indices[n++]  = 0;
    indices[n++]  = 2;
    indices[n++]  = 3;
    indices[n++]  = 4;
    indices[n++]  = 5;
    indices[n++]  = 6;
    indices[n++]  = 4;
    indices[n++]  = 6;
    indices[n++]  = 7;
    indices[n++]  = 8;
    indices[n++]  = 9;
    indices[n++]  = 10;
    indices[n++]  = 8;
    indices[n++]  = 10;
    indices[n++]  = 11;
    indices[n++]  = 12;
    indices[n++]  = 13;
    indices[n++]  = 14;
    indices[n++]  = 12;
    indices[n++]  = 14;
    indices[n++]  = 15;
    indices[n++]  = 16;
    indices[n++]  = 17;
    indices[n++]  = 18;
    indices[n++]  = 16;
    indices[n++]  = 18;
    indices[n++]  = 19;
    indices[n++]  = 20;
    indices[n++]  = 21;
    indices[n++]  = 22;
    indices[n++]  = 20;
    indices[n++]  = 22;
    indices[n++]  = 23;
    primitiveType = GL_TRIANGLES;

    // Generate the OpenGL vertex array object
    glUtils::buildVAO(vao,
                      vboV,
                      vboI,
                      vertices.data(),
                      (GLint)numV,
                      sizeof(VertexPN),
                      indices.data(),
                      (GLint)numI,
                      sizeof(unsigned int),
                      (GLint)shaderProgID,
                      pLoc,
                      -1,
                      nLoc);

    // The vertices and indices get deleted here by the vectors (RAII)
}
//-----------------------------------------------------------------------------
/*!
buildSphere creates the vertex attributes for a sphere and creates the VBO
at the end. The sphere is built in stacks & slices and the primitive type can
be GL_TRIANGLES or GL_TRIANGLE_STRIP.
*/
void buildSphere(float radius, int stacks, int slices, GLuint primitveType)
{
    assert(stacks > 3 && slices > 3);
    assert(primitveType == GL_TRIANGLES || primitveType == GL_TRIANGLE_STRIP);

    // Spherical to cartesian coordinates
    // dtheta = PI  / stacks;
    // dphi = 2 * PI / slices;
    // x = r*sin(theta)*cos(phi);
    // y = r*sin(theta)*sin(phi);
    // z = r*cos(theta);

    // Create vertex array
    std::vector<VertexPN> vertices;
    // ???

    // create Index array
    std::vector<GLuint> indices;
    // ???

    // Delete arrays on heap. The data for rendering is now on the GPU
    if (vertices.size() && indices.size())
    {
        glUtils::buildVAO(vao,
                          vboV,
                          vboI,
                          vertices.data(),
                          (GLint)numV,
                          sizeof(VertexPN),
                          indices.data(),
                          (GLint)numI,
                          sizeof(unsigned int),
                          (GLint)shaderProgID,
                          pLoc,
                          nLoc);
    }
    else
        std::cout << "**** You have to define some vertices and indices first in buildSphere! ****" << std::endl;
}
//-----------------------------------------------------------------------------
/*!
calcFPS determines the frame per second measurement by averaging 60 frames.
*/
float calcFPS(float deltaTime)
{
    const SLint    FILTERSIZE = 60;
    static SLfloat frameTimes[FILTERSIZE];
    static SLuint  frameNo = 0;

    frameTimes[frameNo % FILTERSIZE] = deltaTime;
    float sumTime                    = 0.0f;
    for (SLuint i = 0; i < FILTERSIZE; ++i) sumTime += frameTimes[i];
    frameNo++;
    float frameTimeSec = sumTime / (SLfloat)FILTERSIZE;
    float fps          = 1 / frameTimeSec;

    return fps;
}
//-----------------------------------------------------------------------------
/*!
onInit initializes the global variables and builds the shader program. It
should be called after a window with a valid OpenGL context is present.
*/
void onInit()
{
    // backwards movement of the camera
    camZ = 3;

    // Mouse rotation parameters
    rotX = rotY = 0;
    deltaX = deltaY = 0;
    mouseLeftDown   = false;

    // Load, compile & link shaders
    shaderVertID = glUtils::buildShader(projectRoot + "/data/shaders/ch07_DiffuseLighting.vert", GL_VERTEX_SHADER);
    shaderFragID = glUtils::buildShader(projectRoot + "/data/shaders/ch07_DiffuseLighting.frag", GL_FRAGMENT_SHADER);
    shaderProgID = glUtils::buildProgram(shaderVertID, shaderFragID);

    // Activate the shader program
    glUseProgram(shaderProgID);

    // Get the variable locations (identifiers) within the program
    pLoc              = glGetAttribLocation(shaderProgID, "a_position");
    nLoc              = glGetAttribLocation(shaderProgID, "a_normal");
    pmLoc             = glGetUniformLocation(shaderProgID, "u_pMatrix");
    vmLoc             = glGetUniformLocation(shaderProgID, "u_vMatrix");
    mmLoc             = glGetUniformLocation(shaderProgID, "u_mMatrix");
    lightSpotDirVSLoc = glGetUniformLocation(shaderProgID, "u_lightSpotDir");
    lightDiffuseLoc   = glGetUniformLocation(shaderProgID, "u_lightDiff");
    matDiffuseLoc     = glGetUniformLocation(shaderProgID, "u_matDiff");

    // Create sphere
    resolution    = 16;
    primitiveType = GL_TRIANGLE_STRIP;

    // buildBox();
    buildSphere(1.0f, resolution, resolution, primitiveType);

    glClearColor(0.5f, 0.5f, 0.5f, 1); // Set the background color
    glEnable(GL_DEPTH_TEST);           // Enables depth test
    glEnable(GL_CULL_FACE);            // Enables the culling of back faces
}
//-----------------------------------------------------------------------------
/*!
onClose is called when the user closes the window and can be used for proper
deallocation of resources.
*/
void onClose(GLFWwindow* myWindow)
{
    // Delete shaders & programs on GPU
    glDeleteShader(shaderVertID);
    glDeleteShader(shaderFragID);
    glDeleteProgram(shaderProgID);

    // Delete arrays & buffers on GPU
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboV);
    glDeleteBuffers(1, &vboI);
}
//-----------------------------------------------------------------------------
/*!
onPaint does all the rendering for one frame from scratch with OpenGL.
*/
bool onPaint()
{
    // Clear the color & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 2a) Camera transform: rotate the coordinate system increasingly
     * first around the y- and then around the x-axis. This type of camera
     * transform is called turntable animation.*/
    cameraMatrix.identity();
    cameraMatrix.rotate(rotY + (float)deltaY, 0, 1, 0);
    cameraMatrix.rotate(rotX + (float)deltaX, 1, 0, 0);

    // 2c) Move the camera to its position.
    cameraMatrix.translate(0, 0, camZ);

    // 2c) View transform is world to camera (= inverse of camera matrix)
    viewMatrix = cameraMatrix.inverted();

    // 3) Model transform: move the cube so that it rotates around its center
    modelMatrix.identity();

    // 4) Lights get prepared here later on

    // 5) Pass the uniform variables to the shader
    glUniformMatrix4fv(pmLoc, 1, 0, (float*)&projectionMatrix);
    glUniformMatrix4fv(vmLoc, 1, 0, (float*)&viewMatrix);
    glUniformMatrix4fv(mmLoc, 1, 0, (float*)&modelMatrix);
    glUniform3f(lightSpotDirVSLoc, 0.5f, 1.0f, 1.0f);     // light direction in view space
    glUniform4f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f, 1.0f); // diffuse light intensity (RGBA)
    glUniform4f(matDiffuseLoc, 1.0f, 0.0f, 0.0f, 1.0f);   // diffuse material reflection (RGBA)

    // 6) Activate the vertex array
    glBindVertexArray(vao);

    // 7) Final draw call that draws the cube with triangles by indexes
    glDrawElements(primitiveType, (GLint)numI, GL_UNSIGNED_INT, 0);

    // 8) Fast copy the back buffer to the front buffer. This is OS dependent.
    glfwSwapBuffers(window);

    // Calculate frames per second
    char         title[255];
    static float lastTimeSec = 0;
    float        timeNowSec  = (float)glfwGetTime();
    float        fps         = calcFPS(timeNowSec - lastTimeSec);
    string       prim        = primitiveType == GL_TRIANGLES ? "GL_TRIANGLES" : "GL_TRIANGLE_STRIPS";
    snprintf(title,
             sizeof(title),
             "Sphere, %d x %d, fps: %4.0f, %s",
             resolution,
             resolution,
             fps,
             prim.c_str());
    glfwSetWindowTitle(window, title);
    lastTimeSec = timeNowSec;

    // Return true to get an immediate refresh
    return true;
}
//-----------------------------------------------------------------------------
/*!
onResize: Event handler called on the resize event of the window. This event
should called once before the onPaint event. Do everything that is dependent on
the size and ratio of the window.
*/
void onResize(GLFWwindow* myWindow, int width, int height)
{
    double w = (double)width;
    double h = (double)height;

    // define the projection matrix
    projectionMatrix.perspective(50.0f, (float)(w / h), 0.01f, 10.0f);

    // define the viewport
    glViewport(0, 0, width, height);

    onPaint();
}
//-----------------------------------------------------------------------------
/*!
Mouse button down & release eventhandler starts and end mouse rotation
*/
void onMouseButton(GLFWwindow* myWindow, int button, int action, int mods)
{
    SLint x = mouseX;
    SLint y = mouseY;

    mouseLeftDown = (action == GLFW_PRESS);
    if (mouseLeftDown)
    {
        startX = x;
        startY = y;

        // Renders only the lines of a polygon during mouse moves
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        rotX += (float)deltaX;
        rotY += (float)deltaY;
        deltaX = 0;
        deltaY = 0;

        // Renders filled polygons
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
//-----------------------------------------------------------------------------
/*!
Mouse move eventhandler tracks the mouse delta since touch down (_deltaX/_deltaY)
*/
void onMouseMove(GLFWwindow* myWindow, double x, double y)
{
    mouseX = (int)x;
    mouseY = (int)y;

    if (mouseLeftDown)
    {
        deltaY = (int)(startX - x);
        deltaX = (int)(startY - y);
        onPaint();
    }
}
//-----------------------------------------------------------------------------
/*!
Mouse wheel eventhandler that moves the camera foreward or backwards
*/
void onMouseWheel(GLFWwindow* myWindow, double xscroll, double yscroll)
{
    if (modifiers == NONE)
    {
        camZ += (SLfloat)Utils::sign(yscroll) * 0.1f;
        onPaint();
    }
}
//-----------------------------------------------------------------------------
/*!
Key action eventhandler handles key down & release events
*/
void onKey(GLFWwindow* myWindow, int GLFWKey, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (GLFWKey)
        {
            case GLFW_KEY_ESCAPE:
                onClose(window);
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_RIGHT:
                resolution    = resolution << 1;
                primitiveType = GL_TRIANGLE_STRIP;
                buildSphere(1.0f, resolution, resolution, primitiveType);
                break;
            case GLFW_KEY_LEFT:
                primitiveType = GL_TRIANGLE_STRIP;
                if (resolution > 4) resolution = resolution >> 1;
                buildSphere(1.0f, resolution, resolution, primitiveType);
                break;
            case GLFW_KEY_UP:
                resolution    = resolution << 1;
                primitiveType = GL_TRIANGLES;
                buildSphere(1.0f, resolution, resolution, primitiveType);
                break;
            case GLFW_KEY_DOWN:
                primitiveType = GL_TRIANGLES;
                if (resolution > 4) resolution = resolution >> 1;
                buildSphere(1.0f, resolution, resolution, primitiveType);
                break;
            default: break;
        }
    }
}

//-----------------------------------------------------------------------------
/*!
Error callback handler for GLFW.
*/
void onGLFWError(int error, const char* description)
{
    fputs(description, stderr);
}
//-----------------------------------------------------------------------------
/*! Inits OpenGL and the GLFW window library
 */
void initGLFW(int wndWidth, int wndHeight, const char* wndTitle)
{
    // Initialize the platform independent GUI-Library GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(onGLFWError);

    // Enable fullscreen anti aliasing with 4 samples
    glfwWindowHint(GLFW_SAMPLES, 4);

    // You can enable or restrict newer OpenGL context here (read the GLFW documentation)
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#else
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create the GLFW window
    window = glfwCreateWindow(wndWidth,
                              wndHeight,
                              wndTitle,
                              nullptr,
                              nullptr);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Get the current GL context. After this you can call GL
    glfwMakeContextCurrent(window);

    // Init OpenGL access library gl3w
    if (gl3wInit() != 0)
    {
        std::cerr << "Failed to initialize OpenGL" << std::endl;
        exit(-1);
    }

    // Set GLFW callback functions
    glfwSetKeyCallback(window, onKey);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseWheel);
    glfwSetWindowCloseCallback(window, onClose);

    // Set number of monitor refreshes between 2 buffer swaps
    glfwSwapInterval(1);
}
//-----------------------------------------------------------------------------
/*!
The C main procedure running the GLFW GUI application.
*/
int main(int argc, char* argv[])
{
    projectRoot = SLstring(SL_PROJECT_ROOT);

    scrWidth  = 640;
    scrHeight = 480;

    // Init OpenGL and the window library GLFW
    initGLFW(scrWidth, scrHeight, "ColorSphere");

    // Check errors before we start
    GETGLERROR;

    // Print OpenGL info on console
    glUtils::printGLInfo();

    // Prepare all our OpenGL stuff
    onInit();

    // Call once resize to define the projection
    onResize(window, scrWidth, scrHeight);

    // Event loop
    while (!glfwWindowShouldClose(window))
    {
        // if no updated occurred wait for the next event (power saving)
        if (!onPaint())
            glfwWaitEvents();
        else
            glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
}
//-----------------------------------------------------------------------------
