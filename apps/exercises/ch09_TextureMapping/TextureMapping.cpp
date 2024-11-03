/**
 * \file      TextureMapping.cpp
 * \details   Minimal core profile OpenGL application for ambient-diffuse-
 *            specular lighting shaders with Textures.
 * \date      February 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <Utils.h>
#include <GL/gl3w.h>    // OpenGL headers
#include <GLFW/glfw3.h> // GLFW GUI library
#include <SLMat4.h>     // 4x4 matrix class
#include <SLVec3.h>     // 3D vector class
#include <glUtils.h>    // Basics for OpenGL shaders, buffers & textures

//-----------------------------------------------------------------------------
//! Struct definition for vertex attributes
struct VertexPNT
{
    SLVec3f p; // vertex position [x,y,z]
    SLVec3f n; // vertex normal [x,y,z]
    SLVec2f t; // vertex texture coord. [s,t]
};
//-----------------------------------------------------------------------------
static GLFWwindow* window;      //!< The global glfw window handle
static SLstring    projectRoot; //!< Directory of executable
static SLint       scrWidth;    //!< Window width at start up
static SLint       scrHeight;   //!< Window height at start up

// Global application variables
static SLMat4f cameraMatrix;     //!< 4x4 matrix for camera to world transform
static SLMat4f viewMatrix;       //!< 4x4 matrix for world to camera transform
static SLMat4f modelMatrix;      //!< 4x4 matrix for model to world transform
static SLMat4f lightMatrix;      //!< 4x4 matrix for light to world transform
static SLMat4f projectionMatrix; //!< Projection from view space to normalized device coordinates

static GLuint vao  = 0; //!< ID of the vertex array object
static GLuint vboV = 0; //!< ID of the VBO for vertex attributes
static GLuint vboI = 0; //!< ID of the VBO for vertex index array
static GLuint numV = 0; //!< NO. of vertices
static GLuint numI = 0; //!< NO. of vertex indexes for triangles

static GLint resolution; //!< resolution of sphere stack & slices

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

static SLVec4f globalAmbi;    //!< global ambient intensity
static SLVec4f lightAmbient;  //!< Light ambient intensity
static SLVec4f lightDiffuse;  //!< Light diffuse intensity
static SLVec4f lightSpecular; //!< Light specular intensity
static float   lightSpotDeg;  //!< Light spot cutoff angle in degrees
static float   lightSpotExp;  //!< Light spot exponent
static SLVec3f lightAtt;      //!< Light attenuation factors
static SLVec4f matAmbient;    //!< Material ambient reflection coeff.
static SLVec4f matDiffuse;    //!< Material diffuse reflection coeff.
static SLVec4f matSpecular;   //!< Material specular reflection coeff.
static SLVec4f matEmissive;   //!< Material emissive coeff.
static float   matShininess;  //!< Material shininess exponent

static GLuint shaderVertID = 0; //!< vertex shader id
static GLuint shaderFragID = 0; //!< fragment shader id
static GLuint shaderProgID = 0; //!< shader program id
static GLuint textureID    = 0; //!< texture id

static GLint pLoc;              //!< attribute location for vertex position
static GLint nLoc;              //!< attribute location for vertex normal
static GLint uvLoc;             //!< attribute location for vertex texcoords
static GLint pmLoc;             //!< uniform location for projection matrix
static GLint vmLoc;             //!< uniform location for view matrix
static GLint mmLoc;             //!< uniform location for model matrix
static GLint globalAmbiLoc;     //!< uniform location for global ambient intensity
static GLint lightPosVSLoc;     //!< uniform location for light position in VS
static GLint lightSpotDirVSLoc; //!< uniform location for light direction in VS
static GLint lightAmbientLoc;   //!< uniform location for ambient light intensity
static GLint lightDiffuseLoc;   //!< uniform location for diffuse light intensity
static GLint lightSpecularLoc;  //!< uniform location for specular light intensity
static GLint lightAttLoc;       //!< uniform location fpr light attenuation factors
static GLint matAmbientLoc;     //!< uniform location for ambient light reflection
static GLint matDiffuseLoc;     //!< uniform location for diffuse light reflection
static GLint matSpecularLoc;    //!< uniform location for specular light reflection
static GLint matEmissiveLoc;    //!< uniform location for light emission
static GLint matShininessLoc;   //!< uniform location for shininess
static GLint matTexDiffLoc;     //!< uniform location for texture 0

//-----------------------------------------------------------------------------
/*!
buildSphere creates the vertex attributes for a sphere and creates the VBO
at the end. The sphere is built in stacks & slices. The slices are around the
z-axis.
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
    std::vector<VertexPNT> vertices;
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
                          sizeof(VertexPNT),
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
buildSquare creates the vertex attributes for a textured square and VBO.
The square lies in the x-z-plane and is facing towards -y (downwards).
*/
void buildSquare()
{
    // create vertex array for interleaved position, normal and texCoord
    //                  Position,  Normal, texCrd,
    numV = 4;

    // clang-format off
    float vertices[] = { -1, 0,  -1,   0,-1, 0, 0, 0, // Vertex 0
                         1,  0,  -1, 0,-1, 0, 1, 0, // Vertex 1
                        1,  0, 1,  0,-1, 0, 1, 1, // Vertex 2
                        -1, 0, 1,  0,-1, 0, 0, 1}; // Vertex 3
    // clang-format on
    // create index array for GL_TRIANGLES
    numI             = 6;
    GLuint indices[] = {0, 1, 2, 0, 2, 3};

    // Generate the OpenGL vertex array object
    glUtils::buildVAO(vao,
                      vboV,
                      vboI,
                      vertices,
                      (GLint)numV,
                      sizeof(VertexPNT),
                      indices,
                      (GLint)numI,
                      sizeof(unsigned int),
                      (GLint)shaderProgID,
                      pLoc,
                      nLoc,
                      uvLoc);

    // The vertices and indices are on the stack memory and get deleted at the
    // end of the block.
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

    for (SLuint i = 0; i < FILTERSIZE; ++i)
        sumTime += frameTimes[i];

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
    // Set light parameters
    globalAmbi.set(0.0f, 0.0f, 0.0f);
    lightAmbient.set(0.1f, 0.1f, 0.1f);
    lightDiffuse.set(1.0f, 1.0f, 1.0f);
    lightSpecular.set(1.0f, 1.0f, 1.0f);
    lightMatrix.translate(0, 0, 3);
    lightSpotDeg = 180.0f; // point light
    lightSpotExp = 1.0f;
    lightAtt     = SLVec3f(1, 0, 0); // constant light attenuation = no attenuation
    matAmbient.set(1.0f, 1.0f, 1.0f);
    matDiffuse.set(1.0f, 1.0f, 1.0f);
    matSpecular.set(1.0f, 1.0f, 1.0f);
    matEmissive.set(0.0f, 0.0f, 0.0f);
    matShininess = 500.0f;

    // position of the camera
    camZ = 3.0f;

    // Mouse rotation parameters
    rotX          = 0;
    rotY          = 0;
    deltaX        = 0;
    deltaY        = 0;
    mouseLeftDown = false;

    // Load textures
    textureID = glUtils::buildTexture(projectRoot + "/data/images/textures/earth2048_C.png");

    // Load, compile & link shaders
    shaderVertID = glUtils::buildShader(projectRoot + "/data/shaders/ch09_TextureMapping.vert", GL_VERTEX_SHADER);
    shaderFragID = glUtils::buildShader(projectRoot + "/data/shaders/ch09_TextureMapping.frag", GL_FRAGMENT_SHADER);
    shaderProgID = glUtils::buildProgram(shaderVertID, shaderFragID);

    // Activate the shader program
    glUseProgram(shaderProgID);

    // Get the variable locations (identifiers) within the vertex & pixel shader programs
    pLoc              = glGetAttribLocation(shaderProgID, "a_position");
    nLoc              = glGetAttribLocation(shaderProgID, "a_normal");
    uvLoc             = glGetAttribLocation(shaderProgID, "a_uv");
    pmLoc             = glGetUniformLocation(shaderProgID, "u_pMatrix");
    vmLoc             = glGetUniformLocation(shaderProgID, "u_vMatrix");
    mmLoc             = glGetUniformLocation(shaderProgID, "u_mMatrix");
    globalAmbiLoc     = glGetUniformLocation(shaderProgID, "u_globalAmbi");
    lightPosVSLoc     = glGetUniformLocation(shaderProgID, "u_lightPosVS");
    lightSpotDirVSLoc = glGetUniformLocation(shaderProgID, "u_lightSpotDir");
    lightAmbientLoc   = glGetUniformLocation(shaderProgID, "u_lightAmbi");
    lightDiffuseLoc   = glGetUniformLocation(shaderProgID, "u_lightDiff");
    lightSpecularLoc  = glGetUniformLocation(shaderProgID, "u_lightSpec");
    lightAttLoc       = glGetUniformLocation(shaderProgID, "u_lightAtt");
    matAmbientLoc     = glGetUniformLocation(shaderProgID, "u_matAmbi");
    matDiffuseLoc     = glGetUniformLocation(shaderProgID, "u_matDiff");
    matSpecularLoc    = glGetUniformLocation(shaderProgID, "u_matSpec");
    matEmissiveLoc    = glGetUniformLocation(shaderProgID, "u_matEmis");
    matShininessLoc   = glGetUniformLocation(shaderProgID, "u_matShin");
    matTexDiffLoc     = glGetUniformLocation(shaderProgID, "u_matTexDiff");

    // Build object
    buildSquare();

    // Set some OpenGL states
    glClearColor(0.0f, 0.0f, 0.0f, 1); // Set the background color
    glEnable(GL_DEPTH_TEST);           // Enables depth test
    glEnable(GL_CULL_FACE);            // Enables the culling of back faces
    GETGLERROR;                        // Check for OpenGL errors
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
    glDeleteBuffers(1, &vboV);
    glDeleteBuffers(1, &vboI);
}
//-----------------------------------------------------------------------------
/*!
onPaint does all the rendering for one frame from scratch with OpenGL (in core
profile).
*/
bool onPaint()
{
    // 1) Clear the color & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 2a) Model transform: rotate the coordinate system increasingly
     * first around the y- and then around the x-axis. This type of camera
     * transform is called turntable animation.*/
    cameraMatrix.identity();
    cameraMatrix.rotate(rotY + (float)deltaY, 0, 1, 0);
    cameraMatrix.rotate(rotX + (float)deltaX, 1, 0, 0);

    // 2b) Move the camera to its position.
    cameraMatrix.translate(0, 0, camZ);

    // 2c) View transform is world to camera (= inverse of camera matrix)
    viewMatrix = cameraMatrix.inverted();

    // 3a) Rotate the model so that we see the square from the front side or the earth from the equator.
    modelMatrix.identity();
    modelMatrix.rotate(90, -1, 0, 0);

    // 4a) Transform light position into view space
    SLVec4f lightPosVS = viewMatrix * SLVec4f(lightMatrix.translation());

    // 4b) The spotlight direction is down the negative z-axis of the light transform
    SLVec3f lightSpotDirVS = viewMatrix.mat3() * -lightMatrix.axisZ();

    // 5) Activate the shader program and pass the uniform variables to the shader
    glUseProgram(shaderProgID);
    glUniformMatrix4fv(pmLoc, 1, 0, (float*)&projectionMatrix);
    glUniformMatrix4fv(vmLoc, 1, 0, (float*)&viewMatrix);
    glUniformMatrix4fv(mmLoc, 1, 0, (float*)&modelMatrix);
    glUniform4fv(globalAmbiLoc, 1, (float*)&globalAmbi);
    glUniform3fv(lightPosVSLoc, 1, (float*)&lightPosVS);
    glUniform3fv(lightSpotDirVSLoc, 1, (float*)&lightSpotDirVS);
    glUniform4fv(lightAmbientLoc, 1, (float*)&lightAmbient);
    glUniform4fv(lightDiffuseLoc, 1, (float*)&lightDiffuse);
    glUniform4fv(lightSpecularLoc, 1, (float*)&lightSpecular);
    glUniform3fv(lightAttLoc, 1, (float*)&lightAtt);
    glUniform4fv(matAmbientLoc, 1, (float*)&matAmbient);
    glUniform4fv(matDiffuseLoc, 1, (float*)&matDiffuse);
    glUniform4fv(matSpecularLoc, 1, (float*)&matSpecular);
    glUniform4fv(matEmissiveLoc, 1, (float*)&matEmissive);
    glUniform1f(matShininessLoc, matShininess);
    glUniform1i(matTexDiffLoc, 0);

    // 6) Activate the vertex array
    glBindVertexArray(vao);

    // 7) Draw model triangles by indexes
    glDrawElements(GL_TRIANGLES, (GLsizei)numI, GL_UNSIGNED_INT, nullptr);

    // 8) Fast copy the back buffer to the front buffer. This is OS dependent.
    glfwSwapBuffers(window);

    // 9) Check for OpenGL errors (only in debug done)
    GETGLERROR;

    // Calculate frames per second
    char         title[255];
    static float lastTimeSec = 0;
    float        timeNowSec  = (float)glfwGetTime();
    float        fps         = calcFPS(timeNowSec - lastTimeSec);
    snprintf(title, sizeof(title), "Texture Mapping %3.1f", fps);
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
    float w = (float)width;
    float h = (float)height;

    // define the projection matrix
    projectionMatrix.perspective(45,
                                 w / h,
                                 0.01f,
                                 10.0f);

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
            case GLFW_KEY_UP:
                //_resolution = _resolution << 1;
                //buildSphere(1.0f, (GLuint)_resolution, (GLuint)_resolution);
                break;
            case GLFW_KEY_DOWN:
                //if (_resolution > 4) _resolution = _resolution >> 1;
                //buildSphere(1.0f, (GLuint)_resolution, (GLuint)_resolution);
                break;
            case GLFW_KEY_LEFT_SHIFT:
            case GLFW_KEY_RIGHT_SHIFT: modifiers = modifiers | SHIFT; break;
            case GLFW_KEY_LEFT_CONTROL:
            case GLFW_KEY_RIGHT_CONTROL: modifiers = modifiers | CTRL; break;
            case GLFW_KEY_LEFT_ALT:
            case GLFW_KEY_RIGHT_ALT: modifiers = modifiers | ALT; break;
            default: break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (GLFWKey)
        {
            case GLFW_KEY_LEFT_SHIFT:
            case GLFW_KEY_RIGHT_SHIFT: modifiers = modifiers ^ SHIFT; break;
            case GLFW_KEY_LEFT_CONTROL:
            case GLFW_KEY_RIGHT_CONTROL: modifiers = modifiers ^ CTRL; break;
            case GLFW_KEY_LEFT_ALT:
            case GLFW_KEY_RIGHT_ALT: modifiers = modifiers ^ ALT; break;
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
    initGLFW(scrWidth, scrHeight, "TextureMapping");

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
        // if no updated occured wait for the next event (power saving)
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
