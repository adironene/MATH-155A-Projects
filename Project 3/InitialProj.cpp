//
//  MyInitial.cpp   - Version 0.4 - October 24, 2020
//
//   Sets up and renders the initial (alphabet letter)
//   for the Math 155A project.
//
//   Comes supplied with some code for a cylinders, a torus,
//      and a revolving ellipsoid.
//
//  THIS FILE IS WHAT YOU WILL MODIFY FOR PROJECT #3.
//  IT WILL ALSO BE USED FOR PROJECTS #4 and #5.
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		// Adjust paths as needed.
#include "LinearR4.h"		
#include "GlGeomSphere.h"
#include "GlGeomCylinder.h"
#include "GlGeomTorus.h"

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>

#include "MyInitial.h"
#include "InitialProj.h"
//#include "SurfaceProj.h"      // Replace previous line with this line for Project #4

// ***********
// Animation Controls.  
//     Optional: you may customize these for Project #3.
// ***********

// YOU MAY WISH TO RE-DO THIS FOR YOUR CUSTOM ANIMATION.  
double animateIncrement = 0.01;   // Make bigger to speed up animation, smaller to slow it down.
double currentTime = 0.0;         // Current "time" for the animation.
double armTime = 0.0;
double maxTime = 1.0;             // Time cycles back to 0 after reaching maxTime.
float xWidth = 0.3;
float zWidth = 0.4;
int colorFrame = 1;

// These two variables control whether running or paused.
//  IT IS OPTIONAL TO INCLUDE THIS FUNCTIONALITY IN YOUR PROGRAMMING ASSIGNMENT
bool spinMode = true;
bool singleStep = false;

float sphereColors[22][3] = { {1,1,0},{1,0.8,0},{1,0.6,0.2},{1,0.4,0},{1,0.4,0.4},
    {1,0.4, 0.6},{1,0.4,0.8},{1,0.4,1.0},{0.8,0.4,1.0},{0.6,0.6,1},
{0.4,0.6,1}, {0,0.4,1}, {0,0.6,1}, {0,0.8,1}, {0,1,1}, 
{0.4,1,0.8}, {0.4,1,0.6}, {0.4,1,0.4}, {0.6,1,0.4}, {0.8,1,0.4},{1,1,0.4} };

float letterColors[19][3] = { {0.96,0.90,1},{0.92,0.8,1},{0.88,0.70,1},{0.84,0.6,1},{0.8,0.5,1}
,{0.76,0.4,1},{0.72,0.3,1},{0.68,0.2,1},{0.63,0.1,1},{0.6,0,1},
{0.63,0.1,1},{0.68,0.2,1},{0.72,0.3,1},{0.76,0.4,1},{0.8,0.5,1},
{0.84,0.6,1},{0.88,0.70,1}, {0.92,0.8,1}, {0.96,0.90,1} };


// These objects take care of generating and loading VAO's, VBO's and EBO's,
//    rendering ellipsoids and cylinders
// IF YOU ADDED EXTRA TORII, THEY SHOULD BE DECLARED HERE
GlGeomSphere unitSphere;
GlGeomCylinder unitCylinder;
GlGeomTorus torus1(6, 6, 0.05f);   // Set default mesh resolutions and inner radius
                                  // Initialize multiple tori if they have different inder radii.

// **********************
// This sets up a sphere and a cylinder and a torus needed for the "Initial" (the 3-D alphabet letter)
//  This routine is called only once, for the first initialization.
//  IF YOU ADD EXTRA TORII, THEY NEED TO HANDLED HERE.
// **********************
void MySetupInitialGeometries() {
    // Initialize the sphere and cylinder, and give them the vertPos, vertNormal, and vertTexCoords locations.
    MyRemeshGeometries();
    unitSphere.InitializeAttribLocations(vPos_loc,vNormal_loc,vTexcoords_loc);
    unitCylinder.InitializeAttribLocations(vPos_loc, vNormal_loc, vTexcoords_loc);
    torus1.InitializeAttribLocations(vPos_loc, vNormal_loc, vTexcoords_loc);

    check_for_opengl_errors();
}

// *********************
// This is called when geometric shapes are initialized.
// And is called again whenever the mesh resolution changes.
// IF YOU ADD EXTRA TORII, THEY NEED TO BE HANDLED HERE
// ********************
void MyRemeshGeometries() {
    unitSphere.Remesh(meshRes, meshRes);              // Number of slices and stacks both set to meshRes
    unitCylinder.Remesh(meshRes, meshRes, meshRes);   // Number of slices, stacks and rings all set to meshRes
    torus1.Remesh(meshRes, meshRes);                  // Number of rings and number of sides per ring.
}

// *************************************
// Render the initial (3D alphabet letter)
// THIS CODE IS THE CORE PART TO RE_WRITE FOR YOUR 155A PROJECT  ****************************
// ************
void MyRenderInitial() {
    // Compute the "currentTime" for the animation.
    //    As initially implemented, CurrentTime goes from 0.0 to 1.0, and then back to 0.0
    //    THIS IS SPECIFIC TO THE ANIMATION IN THE DEMO.
    //    FOR PROJECT 3 YOU MAY DO SOMETHING DIFFERENT, FOR INSTANCE, SIMILAR TO WHAT 
	//       THE SOLAR SYSTEM PROJECT DID.
    //  

    if (spinMode) {
        currentTime += animateIncrement;
        armTime += animateIncrement;
        if (currentTime >= maxTime) {
            currentTime = currentTime - floor(currentTime / maxTime);  // Floor function = round down to nearest integer
        }
        if((int)(currentTime*100) %5==0)
            colorFrame++;
        if (singleStep) {
            spinMode = false;       // If in single step mode, turn off future animation
        }
    }
 /*
 * 
 *    

 */
    // Render the letter "X" (Sam's initial) with two cylinders,
    //    Plus a revolving ellipsoid.
   
    // Setup the main Model view matrix
    LinearMapR4 mat1 = viewMatrix;              // Base off of viewMatrix
    mat1.Mult_glTranslate(-2.5, 2.0, -2.5);     // Center of the letter

    LinearMapR4 mat2 = mat1;
    glVertexAttrib3f(vColor_loc, sphereColors[(int)(colorFrame)%21][0], sphereColors[(int)(colorFrame) % 21][1], sphereColors[(int)(colorFrame) % 21][2]); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(0.1, 1.90, 0.0);
    mat2.Mult_glScale(0.5, 0.5, 0.5);         // Nonuniform scaling  
    mat2.Mult_glRotate(currentTime * PI2, 0.0, 0.0, 1.0);   // PI2 is 2*pi (defined in MathMisc.h)
    mat2.Mult_glRotate(currentTime * PI2, 0.0, 1.0, 0.0);
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame) % 18][0], letterColors[(int)(colorFrame) % 18][1], letterColors[(int)(colorFrame) % 18][2]);
    mat2 = mat1;                   // Modelview matrix for cylinders & sphere
    mat2.Mult_glTranslate(-1, 0.0, 0.0);
    mat2.Mult_glScale(xWidth, 3.0, zWidth);          // Scale the cylinder, to thiner, flater and taller 
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitCylinder.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame+2) % 18][0], letterColors[(int)(colorFrame+2) % 18][1], letterColors[(int)(colorFrame+2) % 18][2]);
    mat2 = mat1;                   // Modelview matrix for cylinders & sphere
    mat2.Mult_glRotate(-80 * PI / 180, 0, 0, 1);
    mat2.Mult_glTranslate(-3.0-xWidth, -0.8, 0);
    mat2.Mult_glScale(xWidth, 1, zWidth);
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame+4) % 18][0], letterColors[(int)(colorFrame+4) % 18][1], letterColors[(int)(colorFrame+4) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(0, 3.0+xWidth/2, 0);
    LinearMapR4 mat3 = mat2;
    mat3.Mult_glRotate(-95*PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame + 6) % 18][0], letterColors[(int)(colorFrame + 6) % 18][1], letterColors[(int)(colorFrame + 6) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(0.8, 3.0- xWidth / 2, 0);
    mat3 = mat2;
    mat3.Mult_glRotate(-130 * PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame + 8) % 18][0], letterColors[(int)(colorFrame + 8) % 18][1], letterColors[(int)(colorFrame + 8) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(1.3, 3.0 - 3*xWidth, 0);
    mat3 = mat2;
    mat3.Mult_glRotate(-175 * PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame + 10) % 18][0], letterColors[(int)(colorFrame + 10) % 18][1], letterColors[(int)(colorFrame + 10) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(1.0, 3.0 - 6 * xWidth, 0);
    mat3 = mat2;
    mat3.Mult_glRotate(-210 * PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame + 12) % 18][0], letterColors[(int)(colorFrame + 12) % 18][1], letterColors[(int)(colorFrame + 12) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(0.3, 3.0 - 8 * xWidth, 0);
    mat3 = mat2;
    mat3.Mult_glRotate(-250 * PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, letterColors[(int)(colorFrame+14) % 18][0], letterColors[(int)(colorFrame+14) % 18][1], letterColors[(int)(colorFrame+14) % 18][2]);
    mat2 = mat1;
    mat2.Mult_glTranslate(-0.5, 3.0 - 8 * xWidth, 0);
    mat3 = mat2;
    mat3.Mult_glRotate(-290 * PI / 180, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * abs(cos(10 * armTime)), 0, 0, 1);
    mat3.Mult_glScale(xWidth, 0.5, zWidth);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    glVertexAttrib3f(vColor_loc, sphereColors[(int)(colorFrame) % 21][0], sphereColors[(int)(colorFrame) % 21][1],sphereColors[(int)(colorFrame) % 21][2]); // Blue/Magenta-ish color
    mat2 = mat1;                              // Back to the main Modelview matrix
    mat2.Mult_glTranslate(0.1, 1.90, 0.0);
    mat2.Mult_glScale(2.5);                   // Uniform scaling
    mat2.Mult_glRotate(currentTime* PI2, 0.0, 1.0, 0.0);
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    torus1.Render();

    // snorlax
    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0,0,1); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(2, -0.75, 2.0);
    mat2.Mult_glScale(1.0, 1.0, 0.5);         // Nonuniform scaling  
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0.95, 0.90, 0.85); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(2, -0.75, 2.5);
    mat2.Mult_glScale(0.7, 0.7, 0.05);         // Nonuniform scaling  
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0, 0, 1); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(2, 0.50, 2.0);
    mat2.Mult_glScale(0.6, 0.6, 0.3);         // Nonuniform scaling  
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0.95, 0.90, 0.85); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(2, 0.50, 2.5);
    mat2.Mult_glScale(0.4, 0.4, 0.05);         // Nonuniform scaling  
    mat2.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    // arm right
    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0, 0, 1); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(3, -0.3, 2);
    mat3 = mat2;
    mat3.Mult_glRotate(-250 * PI / 360, 0, 0, 1);
    mat3.Mult_glRotate(-0.45*cos(10*armTime), 1, 0, 0);
    mat3.Mult_glScale(0.3, 0.6, 0.3);         // Nonuniform scaling  
    //LinearMapR4 mat4 = mat3;
    //mat4.Mult_glRotate(currentTime* PI, 0.0, 0, 1.0);
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();
    

    // arm left
    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0, 0, 1); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(1, -0.3, 2);
    mat3 = mat2;
    mat3.Mult_glRotate(250 * PI / 360, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * cos(10 * armTime), 1, 0, 0);
    mat3.Mult_glScale(0.3, 0.6, 0.3);         // Nonuniform scaling  
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0.95, 0.90, 0.85); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(1.2, -1.50, 2);
    mat3 = mat2;
    mat3.Mult_glRotate(250 * PI / 360, 0, 0, 1);
    mat3.Mult_glRotate(0.45 * cos(10 * armTime), 0, 1, 0);
    mat3.Mult_glScale(0.3, 0.2, 0.4);         // Nonuniform scaling  
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();

    mat2 = mat1;
    glVertexAttrib3f(vColor_loc, 0.95, 0.90, 0.85); // Blue/Magenta-ish color
    mat2.Mult_glTranslate(2.8, -1.50, 2);
    mat3 = mat2;
    mat3.Mult_glRotate(250 * PI / 360, 0, 0, 1);
    mat3.Mult_glRotate(-0.45 * cos(10 * armTime), 0, 1, 0);
    mat3.Mult_glScale(0.3, 0.2, 0.4);         // Nonuniform scaling  
    mat3.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    unitSphere.Render();
}