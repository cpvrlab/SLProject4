/**
 * \file      PhongLighting.cpp
 * \details   Core profile OpenGL application with Phong lighted cube with
 *            GLFW as the OS GUI interface (http://www.glfw.org/).
 * \date      September 2012 (HS12)
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#include <GL/gl3w.h>    // OpenGL headers
#include <GLFW/glfw3.h> // GLFW GUI library
#include <SLMat4.h>     // 4x4 matrix class
#include <SLVec3.h>     // 3D vector class
#include <glUtils.h>    // Basics for OpenGL shaders, buffers & textures

//-----------------------------------------------------------------------------
//! Struct definition for vertex attribute position and normal
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
static GLFWwindow* window;      //!< The global GLFW window handle
static SLstring    projectRoot; //!< Directory of executable
static SLint       scrWidth;    //!< Window width at start up
static SLint       scrHeight;   //!< Window height at start up

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

static SLVec4f globalAmbi;   //!< global ambient intensity
static SLVec4f lightAmbi;    //!< Light ambient intensity
static SLVec4f lightDiff;    //!< Light diffuse intensity
static SLVec4f lightSpec;    //!< Light specular intensity
static float   lightSpotDeg; //!< Light spot cutoff angle in degrees
static float   lightSpotExp; //!< Light spot exponent
static SLVec3f lightAtt;     //!< Light attenuation factors
static SLVec4f matAmbi;      //!< Material ambient reflection coeff.
static SLVec4f matDiff;      //!< Material diffuse reflection coeff.
static SLVec4f matSpec;      //!< Material specular reflection coeff.
static SLVec4f matEmis;      //!< Material emissive coeff.
static float   matShin;      //!< Material shininess

static GLuint shaderVertID = 0; //! vertex shader id
static GLuint shaderFragID = 0; //! fragment shader id
static GLuint shaderProgID = 0; //! shader program id

// Attribute & uniform variable location indexes
static GLint pLoc;  //!< attribute location for vertex position
static GLint nLoc;  //!< attribute location for vertex normal
static GLint pmLoc; //!< uniform location for projection matrix
static GLint vmLoc; //!< uniform location for view matrix
static GLint mmLoc; //!< uniform location for model matrix

static GLint globalAmbiLoc;   //!< uniform location for global ambient intensity
static GLint lightPosVSLoc;   //!< uniform location for light position in VS
static GLint lightAmbiLoc;    //!< uniform location for ambient light intensity
static GLint lightDiffLoc;    //!< uniform location for diffuse light intensity
static GLint lightSpecLoc;    //!< uniform location for specular light intensity
static GLint lightSpotDirLoc; //!< uniform location for light direction in VS
static GLint lightSpotDegLoc; //!< uniform location for spot cutoff angle in degrees
static GLint lightSpotCosLoc; //!< uniform location for cosine of spot cutoff angle
static GLint lightSpotExpLoc; //!< uniform location for cosine of spot cutoff angle
static GLint lightAttLoc;     //!< uniform location fpr light attenuation factors
static GLint matAmbiLoc;      //!< uniform location for ambient light reflection
static GLint matDiffLoc;      //!< uniform location for diffuse light reflection
static GLint matSpecLoc;      //!< uniform location for specular light reflection
static GLint matEmisLoc;      //!< uniform location for light emission
static GLint matShinLoc;      //!< uniform location for shininess

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

    int n        = 0;
    indices[n++] = 0;
    indices[n++] = 1;
    indices[n++] = 2;
    indices[n++] = 0;
    indices[n++] = 2;
    indices[n++] = 3;
    indices[n++] = 4;
    indices[n++] = 5;
    indices[n++] = 6;
    indices[n++] = 4;
    indices[n++] = 6;
    indices[n++] = 7;
    indices[n++] = 8;
    indices[n++] = 9;
    indices[n++] = 10;
    indices[n++] = 8;
    indices[n++] = 10;
    indices[n++] = 11;
    indices[n++] = 12;
    indices[n++] = 13;
    indices[n++] = 14;
    indices[n++] = 12;
    indices[n++] = 14;
    indices[n++] = 15;
    indices[n++] = 16;
    indices[n++] = 17;
    indices[n++] = 18;
    indices[n++] = 16;
    indices[n++] = 18;
    indices[n++] = 19;
    indices[n++] = 20;
    indices[n++] = 21;
    indices[n++] = 22;
    indices[n++] = 20;
    indices[n++] = 22;
    indices[n++] = 23;

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
onInit initializes the global variables and builds the shader program. It
should be called after a window with a valid OpenGL context is present.
*/
void onInit()
{
    // Position of the camera
    camZ = 3;

    // Mouse rotation parameters
    rotX = rotY = 0;
    deltaX = deltaY = 0;
    mouseLeftDown   = false;

    // Set light parameters
    globalAmbi.set(0.05f, 0.05f, 0.05f);
    lightAmbi.set(0.2f, 0.2f, 0.2f);
    lightDiff.set(1.0f, 1.0f, 1.0f);
    lightSpec.set(1.0f, 1.0f, 1.0f);
    lightMatrix.translate(0, 0, 3);
    lightSpotDeg = 10.0f; // 180.0f; // point light
    lightSpotExp = 1.0f;
    lightAtt     = SLVec3f(1, 0, 0); // constant light attenuation = no attenuation
    matAmbi.set(1.0f, 0.0f, 0.0f);
    matDiff.set(1.0f, 0.0f, 0.0f);
    matSpec.set(1.0f, 1.0f, 1.0f);
    matEmis.set(0.0f, 0.0f, 0.0f);
    matShin = 1000.0f;

    // Load, compile & link shaders
    shaderVertID = glUtils::buildShader(projectRoot + "/data/shaders/ch08_BlinnPhongLighting.vert", GL_VERTEX_SHADER);
    shaderFragID = glUtils::buildShader(projectRoot + "/data/shaders/ch08_BlinnPhongLighting.frag", GL_FRAGMENT_SHADER);
    shaderProgID = glUtils::buildProgram(shaderVertID, shaderFragID);

    // Activate the shader program
    glUseProgram(shaderProgID);

    // Get the variable locations (identifiers) within the program
    pLoc = glGetAttribLocation(shaderProgID, "a_position");
    nLoc = glGetAttribLocation(shaderProgID, "a_normal");

    mmLoc = glGetUniformLocation(shaderProgID, "u_mMatrix");
    vmLoc = glGetUniformLocation(shaderProgID, "u_vMatrix");
    pmLoc = glGetUniformLocation(shaderProgID, "u_pMatrix");

    lightPosVSLoc   = glGetUniformLocation(shaderProgID, "u_lightPosVS");
    lightAmbiLoc    = glGetUniformLocation(shaderProgID, "u_lightAmbi");
    lightDiffLoc    = glGetUniformLocation(shaderProgID, "u_lightDiff");
    lightSpecLoc    = glGetUniformLocation(shaderProgID, "u_lightSpec");
    lightSpotDirLoc = glGetUniformLocation(shaderProgID, "u_lightSpotDir");
    lightSpotDegLoc = glGetUniformLocation(shaderProgID, "u_lightSpotDeg");
    lightSpotCosLoc = glGetUniformLocation(shaderProgID, "u_lightSpotCos");
    lightSpotExpLoc = glGetUniformLocation(shaderProgID, "u_lightSpotExp");
    lightAttLoc     = glGetUniformLocation(shaderProgID, "u_lightAtt");
    globalAmbiLoc   = glGetUniformLocation(shaderProgID, "u_globalAmbi");

    matAmbiLoc = glGetUniformLocation(shaderProgID, "u_matAmbi");
    matDiffLoc = glGetUniformLocation(shaderProgID, "u_matDiff");
    matSpecLoc = glGetUniformLocation(shaderProgID, "u_matSpec");
    matEmisLoc = glGetUniformLocation(shaderProgID, "u_matEmis");
    matShinLoc = glGetUniformLocation(shaderProgID, "u_matShin");

    buildBox();

    glClearColor(0.5f, 0.5f, 0.5f, 1); // Set the background color
    glEnable(GL_DEPTH_TEST);           // Enables depth test
    glEnable(GL_CULL_FACE);            // Enables the culling of back faces
    GETGLERROR;
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
    // 1) Clear the color & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 2b) Model transform: rotate the coordinate system increasingly
     * first around the y- and then around the x-axis. This type of camera
     * transform is called turntable animation.*/
    cameraMatrix.identity();
    cameraMatrix.rotate(rotY + (float)deltaY, 0, 1, 0);
    cameraMatrix.rotate(rotX + (float)deltaX, 1, 0, 0);

    // 2a) Move the camera to its position.
    cameraMatrix.translate(0, 0, camZ);

    // 2c) View transform is world to camera (= inverse of camera matrix)
    viewMatrix = cameraMatrix.inverted();

    // 3) Model transform: move the cube so that it rotates around its center
    modelMatrix.identity();
    modelMatrix.translate(-0.5f, -0.5f, -0.5f);

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
    glUniform4fv(lightPosVSLoc, 1, (float*)&lightPosVS);
    glUniform4fv(lightAmbiLoc, 1, (float*)&lightAmbi);
    glUniform4fv(lightDiffLoc, 1, (float*)&lightDiff);
    glUniform4fv(lightSpecLoc, 1, (float*)&lightSpec);
    glUniform3fv(lightSpotDirLoc, 1, (float*)&lightSpotDirVS);
    glUniform3fv(lightAttLoc, 1, (float*)&lightAtt);
    glUniform1f(lightSpotDegLoc, lightSpotDeg);
    glUniform1f(lightSpotCosLoc, cos(lightSpotDeg * Utils::DEG2RAD));
    glUniform1f(lightSpotExpLoc, lightSpotExp);

    glUniform4fv(matAmbiLoc, 1, (float*)&matAmbi);
    glUniform4fv(matDiffLoc, 1, (float*)&matDiff);
    glUniform4fv(matSpecLoc, 1, (float*)&matSpec);
    glUniform4fv(matEmisLoc, 1, (float*)&matEmis);
    glUniform1f(matShinLoc, matShin);

    // 6) Activate the vertex array
    glBindVertexArray(vao);

    // 7) Final draw call that draws the cube with triangles by indexes
    glDrawElements(GL_TRIANGLES, (GLsizei)numI, GL_UNSIGNED_INT, nullptr);

    // 8) Fast copy the back buffer to the front buffer. This is OS dependent.
    glfwSwapBuffers(window);
    GETGLERROR;

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
    projectionMatrix.perspective(50, w / h, 0.01f, 10.0f);

    // define the viewport
    glViewport(0, 0, width, height);

    GETGLERROR;

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
Mouse wheel eventhandler that moves the camera forward or backwards
*/
void onMouseWheel(GLFWwindow* myWindow, double xscroll, double yscroll)
{
    if (modifiers == NONE)
    {
        camZ += (SLfloat)Utils::sign(yscroll) * 0.1f;
        onPaint();
    }
    else if (modifiers == ALT)
    {
        matShin *= yscroll > 0.0 ? 1.5f : 0.75f;
        onPaint();
    }
    else if (modifiers & ALT && modifiers & CTRL)
    {
        lightSpotExp += yscroll > 0.0 ? 10.0f : -10.0f;
        lightSpotExp = Utils::clamp(lightSpotExp, 0.0f, 200.0f);
        onPaint();
    }
    else if (modifiers == CTRL)
    {
        lightSpotDeg += yscroll > 0.0 ? 1.0f : -1.0f;
        lightSpotDeg = Utils::clamp(lightSpotDeg, 0.0f, 180.0f);
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
    initGLFW(scrWidth, scrHeight, "Blinn-Phong-Lighting");

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
