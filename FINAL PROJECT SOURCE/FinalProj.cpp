/*
 * TextureProj.cpp - Version 0.3 - February 24, 2019
 *
 * Starting code for Math 155A, Project #6,
 * 
 * Author: Sam Buss
 *
 * Software accompanying POSSIBLE SECOND EDITION TO the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG2
 */

// These libraries are needed to link the program.
// First five are usually provided by the system.
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <filesystem>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "EduPhong.h"
#include "PhongData.h"
#include "GlShaderMgr.h"
#include "GlGeomSphere.h"
#include "GlGeomCylinder.h"
#include "GlGeomTorus.h"
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>

#include "FinalProj.h"
#include "MyGeometries.h"


// ********************
// Animation controls and state infornation
// ********************

// These variables control the view direction.
//    The arrow keys are used to change these values.
double viewAzimuth = 0.25;	// Angle of view up/down (in radians)
double viewDirection = 0.0; // Rotation of view around y-axis (in radians)
double deltaAngle = 0.01;	// Change in view angle for each up/down/left/right arrow key press
LinearMapR4 viewMatrix;		// The current view matrix, based on viewAzimuth and viewDirection.

// Control Phong lighting modes
phGlobal globalPhongData;

// These two variables control how triangles are rendered.
bool wireframeMode = false;	// Equals true for polygon GL_LINE mode. False for polygon GL_FILL mode.
bool cullBackFaces = true;
bool renderFloorOnly = false;

// The next variable controls the resolution of the meshes for cylinders and spheres and tori.
int meshRes=20;             // Resolution of the meshes (slices, stacks, and rings all equal)
double snorlaxPOV = PI / 2;
double snorlaxPOSx = 0;
double snorlaxPOSy = 0;
double armTime = 0;

double faceDir = 0;
bool faceSide = false;

double mapValx = 0.63;
double mapValy = 0.7;

bool mapFull = false;

// These variables control the animation's state and speed.
// YOUR CODE WILL NOT USE THIS UNLESS YOU ADD ANIMATION  
double animateIncrement = 0.01;   // Make bigger to speed up animation, smaller to slow it down.
double currentTime = 0.0;         // Current "time" for the animation.
bool spinMode = true;       // Controls whether running or paused.
double currentDelta = 0.0;        // Current state of the animation (YOUR CODE MAY NOT WANT TO USE THIS.)

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************

unsigned int shaderProgramBitmap;       // The shader program that applies a bitmapped texture map (from a file)
unsigned int shaderProgramProc ;       // The shader program that applies a procedural texture map

unsigned int modelviewMatLocation;					// Location of the modelviewMatrix in the currently active shader program
unsigned int applyTextureLocation; 					// Location of the applyTexture bool in the currently active shader program
unsigned int timeLoc;

//  The Projection matrix: Controls the "camera view/field-of-view" transformation
//     Generally is the same for all objects in the scene.
LinearMapR4 theProjectionMatrix;		//  The Projection matrix: Controls the "camera/view" transformation

// *****************************
// These variables set the dimensions of the perspective region we wish to view.
// They are used to help form the projection matrix and the view matrix
// All rendered objects lie in the rectangular prism centered on the z-axis
//     equal to (-Xmax,Xmax) x (-Ymax,Ymax) x (Zmin,Zmax)
// Be sure to leave some slack in these values, to allow for rotations, etc.
// The model/view matrix can be used to move objects to this position
// THESE VALUES MAY NEED AD-HOC ADJUSTMENT TO GET THE SCENE TO BE VISIBLE.
const double Xmax = 100;                // Control x dimensions of viewable scene
const double Ymax = 20;                // Control y dimensions of viewable scene
const double Zmin = -200.0, Zmax = 200.0;   // Control z dimensions of the viewable scene

// zDistance equals the initial distance from the camera to the z = Zmax plane
const double zDistance = 20.0;              // Make this value larger or smaller to affect field of view.

double ZextraDistance = 0.0;              // Extra distance we have moved to/from the scene
double ZextraDelta = 0.2;                // Pressing HOME/END moves closer/farther by this amount
const double ZextraDistanceMin = -15.0;
const double ZextraDistanceMax = 50.0;
int screenWidth = 800, screenHeight = 600;     // Width and height in pixels. Initially 800x600

// skybox

float skyboxVertices[] =
{
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};

// *************************
// mySetupGeometries defines the scene data, especially vertex  positions and colors.
//    - It also loads all the data into the VAO's (Vertex Array Objects) and
//      into the VBO's (Vertex Buffer Objects).
// This routine is only called once to initialize the data.
// *************************
void mySetupGeometries() {
 
    MySetupSurfaces();

    mySetViewMatrix();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void mySetViewMatrix() {
    // Set the view matrix. Sets view distance, and view direction.
    // The final translation is done because the ground plane lies in the xz-plane,
    //    se the center of the scene is about 3 or 4 units above the origin.
    // YOU MAY NEED TO ADJUST THE FINAL TRANSLATION AND?OR ADD A SCALING TO MAKE THE SCENE VISIBLE.
    viewMatrix.Set_glTranslate(0.0, 0.0, -(20 + zDistance + ZextraDistance));      // Translate to be in front of the camera
    viewMatrix.Mult_glRotate(viewAzimuth, 1.0, 0.0, 0.0);	    // Rotate viewAzimuth radians around x-axis
    viewMatrix.Mult_glRotate(-viewDirection, 0.0, 1.0, 0.0);    // Rotate -viewDirection radians around y-axis
    viewMatrix.Mult_glTranslate(0.0, -3.5, -10.0);                // Translate the scene down the y-axis so the center is near the origin.
}

// *************************************
// Main routine for rendering the scene
// myRenderScene() is called every time the scene needs to be redrawn.
// mySetupGeometries() has already created the vertex and buffer objects
//    and the model view matrices.
// The EduPhong shaders are already setup.
// *************************************
void myRenderScene() {

    currentTime += animateIncrement;
    currentTime -= floor(currentTime);
    selectShaderProgram(shaderProgramProc);
    glUniform1f(timeLoc, (float)currentTime);
   
    // Clear the rendering window
    static const float black[] = { 0.67f, 0.84f, 0.9f, 1.0f };
    const float clearDepth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, black);
    glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a *pointer* to the depth

    selectShaderProgram(shaderProgramProc);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture
    MyRenderSpheresForLights();

    MyRenderGeometries();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_SceneData() {

    GlShaderMgr::LoadShaderSource("EduPhong.glsl");
    GlShaderMgr::LoadShaderSource("MyShaders.glsl");

    // These two shaders differ only in the third part of the code used for the fragment shader!

    // The first shader program applies a texture map (a bitmap)
    unsigned int vertexShader1 = GlShaderMgr::CompileShader("vertexShader_PhongPhong");
    unsigned int fragmentShader1 = GlShaderMgr::CompileShader("fragmentShader_PhongPhong", "calcPhongLighting", "applyTextureMap");
    unsigned int shaderList1[2] = { vertexShader1 , fragmentShader1 };
    shaderProgramBitmap = GlShaderMgr::LinkShaderProgram(2, shaderList1);
    phRegisterShaderProgram(shaderProgramBitmap);

    // The second shader program applies a procedural texture map -- Defined in MyShaders.glsl
    // FOR PROJECT 6: YOU WILL RE_WRITE THE SHADER CODE IN MyShaders.glsl.
    unsigned int fragmentShader2 = GlShaderMgr::CompileShader("fragmentShader_PhongPhong", "calcPhongLighting", "MyProcTexture");
    unsigned int shaderList2[2] = { vertexShader1 , fragmentShader2 };
    shaderProgramProc = GlShaderMgr::LinkShaderProgram(2, shaderList2);
    phRegisterShaderProgram(shaderProgramProc);

    timeLoc = glGetUniformLocation(shaderProgramProc, "currentTime");

    mySetupGeometries();
    check_for_opengl_errors();
    SetupForTextures();   // The shader programs should be compiled and linked before setting up textures.
    check_for_opengl_errors();

    MySetupGlobalLight();
    MySetupLights();
    LoadAllLights();
    MySetupMaterials();

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void selectShaderProgram(unsigned int shaderProgram) {
    assert(shaderProgram == shaderProgramBitmap || shaderProgram == shaderProgramProc);
    glUseProgram(shaderProgram);
    modelviewMatLocation = phGetModelviewMatLoc(shaderProgram);
    applyTextureLocation = phGetApplyTextureLoc(shaderProgram);
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static const double Pi = 3.1415926535f;
    if (action == GLFW_RELEASE) {
        return;			// Ignore key up (key release) events
    }
    bool viewChanged = false;
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        return;
    case '4':
    case '1':
    case '2':
    case '3':
    {
        phLight& theLight = myLights[key - '1'];
        theLight.IsEnabled = !theLight.IsEnabled;   // Toggle whether the light is enabled.
        LoadAllLights();
        return;
    }
    case 'R':
        spinMode = !spinMode;	// Toggle animation on and off.
        return;
    case 'W':		// Toggle wireframe mode
        if (wireframeMode) {
            wireframeMode = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            wireframeMode = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        return;
    case 'C':		// Toggle backface culling
        cullBackFaces = !cullBackFaces;     // Negate truth value of cullBackFaces
        if (cullBackFaces) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        return;
    case 'M':
        mapFull = !mapFull;
        LoadAllLights();
        return;
    case 'F':
        if (mods & GLFW_MOD_SHIFT) {                // If upper case 'F'
            animateIncrement *= sqrt(2.0);			// Double the animation time step after two key presses
        }
        else {                                      // Else lose case 'f',
            animateIncrement *= sqrt(0.5);			// Halve the animation time step after two key presses
        }
        return;
    case GLFW_KEY_Q:
        renderFloorOnly = !renderFloorOnly;
        break;

    case GLFW_KEY_UP:
      //ZextraDistance -= ZextraDelta;
      //viewAzimuth = Min(viewAzimuth + 0.01, PIhalves - 0.05);
        faceSide = false;
        faceDir = 0;
        snorlaxPOSy -= 0.1;
        if (mapValy < 1) {
            mapValy += 0.0007;
        }
        armTime += 0.02;
        viewChanged = true;
        break;
    case GLFW_KEY_DOWN:
      //ZextraDistance += ZextraDelta;
      //viewAzimuth = Max(viewAzimuth - 0.01, -PIhalves + 0.05);
        faceSide = false;
        faceDir = 360;
        if (mapValy > 0) {
            mapValy -= 0.0007;
        }
        armTime += 0.02;
        snorlaxPOSy += 0.1;
        viewChanged = true;
        break;
    case GLFW_KEY_RIGHT:
        snorlaxPOV -= 0.02;
        snorlaxPOSx += 0.1;

        faceSide = true;
        faceDir = -180;
        if (mapValx < 1) {
            mapValx += 0.0005;
        }
        armTime += 0.02;
        viewChanged = true;
        break;
    case GLFW_KEY_LEFT:
        //viewDirection += 0.01;
        //if (viewDirection < -PI) {
        //    viewDirection += PI2;
        //}
        faceSide = true;
        faceDir = 180;
        snorlaxPOSx -= 0.1;
        if (mapValx > 0) {
            mapValx -= 0.0005;
        }
        armTime += 0.02;
        snorlaxPOV += 0.02;
        viewChanged = true;
        break;
    case GLFW_KEY_HOME:     
        ZextraDistance -= ZextraDelta;         // Move closer to the scene     
        //ClampMin(&ZextraDistance, ZextraDistanceMin);
        viewChanged = true;
        break;
    case GLFW_KEY_END:
        ZextraDistance += ZextraDelta;         // Move farther away from the scene
        //ClampMax(&ZextraDistance, ZextraDistanceMax);
        viewChanged = true;
        break;
    }

    if (viewChanged) {
        mySetViewMatrix();
        setProjectionMatrix();
        LoadAllLights();        // Have to call this since it affects the position of the lights!
    }
    else {
        // Updated the global phong data above: upload it to the shader program.
        globalPhongData.LoadIntoShaders();
    }
}


// *************************************************
// This function is called with the graphics window is first created,
//    and again whenever it is resized.
// The Projection View Matrix is typically set here.
//    But this program does not use any transformations or matrices.
// *************************************************
void window_size_callback(GLFWwindow* window, int width, int height) {
    // Define the portion of the window used for OpenGL rendering.
    glViewport(0, 0, width, height);
    screenWidth = width == 0 ? 1 : width;
    screenHeight = height==0 ? 1 : height;
    setProjectionMatrix();
}

void skyBoxSet() {

    // Create VAO, VBO, and EBO for the skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string facesCubemap[6] =
    {
        "right.png",
        "leftMost.png",
        "up.png",
        "down.png",
        "center.png",
        "rightMost.png"
    };

    // Creates the cubemap texture object
    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // This might help with seams on some systems
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }

}
void setProjectionMatrix() {
	// Setup the projection matrix as a perspective view.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	double w = (double)screenWidth;
	double h = (double)screenHeight;
	double windowXmax, windowYmax;
    double aspectFactor = w * Ymax / (h * Xmax);   // == (w/h)/(Xmax/Ymax), ratio of aspect ratios
	if (aspectFactor>1) {
		//windowXmax = Xmax * aspectFactor;
        windowXmax = 10;
		windowYmax = Ymax;
        windowYmax = 5;
	}
	else {
		windowYmax = Ymax / aspectFactor;
		//windowXmax = Xmax;
        windowXmax = 10;
        windowYmax = 5;
	}

	// Using the max & min values for x & y & z that should be visible in the window,
	//		we set up the orthographic projection.
    // Could update the zNear and zFar each time the distance changes, but we'll avoid it for now
    double zNear = zDistance+ZextraDistance;
    double zFar = zNear + Zmax - Zmin;
    double scale = zNear / zDistance;
    theProjectionMatrix.Set_glFrustum(-windowXmax * scale, windowXmax * scale,
                                      -windowYmax * scale, windowYmax * scale, zNear, zFar);
    float matEntries[16];
    theProjectionMatrix.DumpByColumns(matEntries);
    if (glIsProgram(shaderProgramBitmap)) {
        check_for_opengl_errors();
        glUseProgram(shaderProgramBitmap);
        glUniformMatrix4fv(phGetProjMatLoc(shaderProgramBitmap), 1, false, matEntries);
    }
    if (glIsProgram(shaderProgramProc)) {
        glUseProgram(shaderProgramProc);
        glUniformMatrix4fv(phGetProjMatLoc(shaderProgramProc), 1, false, matEntries);
    }

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_OpenGL() {
	
	glEnable(GL_DEPTH_TEST);	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders

	// Set polygon drawing mode for front and back of each polygon
    glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL );

    glEnable(GL_CULL_FACE);

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window) {
	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	// Set callback for key up/down/repeat events
	glfwSetKeyCallback(window, key_callback);

	// Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
	// glfwSetCursorPosCallback(window, cursor_pos_callback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}

int main() {
	glfwSetErrorCallback(error_callback);	// Supposed to be called in event of errors. (doesn't work?)
	glfwInit();
#if defined(__APPLE__) || defined(__linux__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Snorlax in Minecraft", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return -1;
	}

	// Print info of GPU and supported OpenGL version
    setup_callbacks(window);
   
	// Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
	my_setup_SceneData();
    //skyBoxSet();
 	window_size_callback(window, screenWidth, screenHeight);

    // Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {
	
		myRenderScene();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		// Poll events (key presses, mouse events)
		glfwWaitEventsTimeout(1.0/60.0);	    // Use this to animate at 60 frames/sec (timing is NOT reliable)
		// glfwWaitEvents();					// Or, Use this instead if no animation.
		// glfwPollEvents();					// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[8][36] = {
	"Unknown OpenGL error",
	"GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
	"GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
	"GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW" };
bool check_for_opengl_errors() {
	int numErrors = 0;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		numErrors++;
		int errNum = 0;
		switch (err) {
		case GL_INVALID_ENUM:
			errNum = 1;
			break;
		case GL_INVALID_VALUE:
			errNum = 2;
			break;
		case GL_INVALID_OPERATION:
			errNum = 3;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errNum = 4;
			break;
		case GL_OUT_OF_MEMORY:
			errNum = 5;
			break;
		case GL_STACK_UNDERFLOW:
			errNum = 6;
			break;
		case GL_STACK_OVERFLOW:
			errNum = 7;
			break;
		}
		printf("OpenGL ERROR: %s.\n", errNames[errNum]);
	}
	return (numErrors != 0);
}