//
//  MySurfaces.cpp
//
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #4.
//
//   Comes supplied with the code for Sam Buss's "X".
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "MathMisc.h"       // Adjust path as needed

#include "MySurfaces.h"
#include "PhongProj.h"


// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************
const int NumObjects = 2;
const int iFloor = 0;
const int iCircularSurf = 1;

unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;
unsigned int myEBO[NumObjects];  // a Element Array Buffer Object - holds an array of elements (vertex indices)

// **********************
// This sets up geometries needed for the "Initial" (the 3-D alphabet letter)
//  It is called only once.
// **********************
void MySetupSurfaces() {
    // Initialize the VAO's, VBO's and EBO's for the ground plane,
    // and the surface of rotation. Give them the "aPos" location,
    // and the "aNormal" location in the shader program.
    // No data is loaded into the VBO's or EBO's until the "Remesh"
    //   routines are called.

    // For the floor:
    // Allocate the needed Vertex Array Objects (VAO's),
    //      Vertex Buffer Objects (VBO's) and Element Array Buffer Objects (EBO's)
    // Since the floor has normal (0,1,0) everywhere, it will be given as
    // generic vertex attribute, not loaded in the VBO as a per-vertex value.
    // The color is also a generic vertex attribute.
    glGenVertexArrays(NumObjects, &myVAO[0]);
    glGenBuffers(NumObjects, &myVBO[0]);
    glGenBuffers(NumObjects, &myEBO[0]);

    glBindVertexArray(myVAO[iFloor]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);

    glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Store vertices in the VBO
    glEnableVertexAttribArray(vPos_loc);									// Enable the stored vertices

    // For the circular surface:
    // Allocate the needed VAO, VBO< EBO
    // The normal vector is specified separately for each vertex. (It is not a generic attribute.)
    // The color is still a generic vertex attribute.
    glBindVertexArray(myVAO[iCircularSurf]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCircularSurf]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iCircularSurf]);

    glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);	// Store vertices in the VBO
    glEnableVertexAttribArray(vPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vNormal_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Store normals in the VBO
    glEnableVertexAttribArray(vNormal_loc);                                 // Enabled the stored normals (so they are not generic)

    // No data has been loaded into the VBO's yet.
    // This is done next by the "Remesh" routines.

    MyRemeshSurfaces();

    check_for_opengl_errors();      // Watch the console window for error messages!
}

// **********************************************
// MODIFY THIS ROUTINE TO CALL YOUR OWN CODE IN
//   MyRemeshFloor AND MyRemeshCircularSurf
// INSTEAD OF THE "DEMO" VERSIONS.
// **********************************************

void MyRemeshSurfaces()
{
    // WRITE MyRemeshFloor (see below) AND USE IT INSTEAD OF RemeshPlaneDemo
    //RemeshFloorDemo();
    MyRemeshFloor();

    // WRITE MyRemeshCircularSurf (see below) AND USE IT INSTEAD OF RemeshCircularDemo
    //RemeshCircularDemo();
    MyRemeshCircularSurf();

    check_for_opengl_errors();      // Watch the console window for error messages!
}

// **********************************************
// MODIFY THIS ROUTINE TO CALL YOUR OWN CODE IN
//   MyRemeshFloor AND MyRemeshCircularSurf
// INSTEAD OF THE "DEMO" VERSIONS.
// **********************************************

void MyRenderSurfaces() {
    // WRITE MyRemeshFloor (see below) AND USE IT INSTEAD OF RemeshPlaneDemo
    //RenderFloorDemo();
    MyRenderFloor();

    // WRITE MyRemeshCircularSurf (see below) AND USE IT INSTEAD OF RemeshCircularDemo
    //RenderCircularDemo();
    MyRenderCircularSurf();

    check_for_opengl_errors();      // Watch the console window for error messages!
}

// *********************************************
// THE CODE FOR THE NEXT FOUR "Demo" ROUIINTES BELOW GIVES SIMPLE FIXED SIZE OBJECTS:
// A plane gridded into 4x4 array of rectangles (triangulated)
// and Circular piece with 4 slices, and 4 rings.
// THIS CODE WILL NOT BE USED IN YOUR PROGRAMMING SOLUTION.
// INSTEAD, FINISH WRITING MyRemeshFloor() and MyRemeshCircularSurf() below.
// *********************************************************

// Make the floor a 4x4 grid of triangulated quads.
// Render it as four triangle strips.
void RemeshFloorDemo()
{
    // Floor vertices.
    float floorVerts[][3] = {
        { -5.0f, 0.0f, -5.0f }, { -2.5f, 0.0f, -5.0f }, { 0.0f, 0.0f, -5.0f }, { 2.5f, 0.0f, -5.0f }, { 5.0f, 0.0f, -5.0f },  // Back row
        { -5.0f, 0.0f, -2.5f }, { -2.5f, 0.0f, -2.5f }, { 0.0f, 0.0f, -2.5f }, { 2.5f, 0.0f, -2.5f }, { 5.0f, 0.0f, -2.5f },  // Fourth row
        { -5.0f, 0.0f,  0.0f }, { -2.5f, 0.0f,  0.0f }, { 0.0f, 0.0f,  0.0f }, { 2.5f, 0.0f,  0.0f }, { 5.0f, 0.0f,  0.0f },  // Third row
        { -5.0f, 0.0f,  2.5f }, { -2.5f, 0.0f,  2.5f }, { 0.0f, 0.0f,  2.5f }, { 2.5f, 0.0f,  2.5f }, { 5.0f, 0.0f,  2.5f },  // Second row
        { -5.0f, 0.0f,  5.0f }, { -2.5f, 0.0f,  5.0f }, { 0.0f, 0.0f,  5.0f }, { 2.5f, 0.0f,  5.0f }, { 5.0f, 0.0f,  5.0f },  // Front row
    };
    // Floor elements (indices to vertices in a triangle strip)
    unsigned int floorElements[] = {
        0,5,1,6,2,7,3,8,4,9,            // Elements for first triangle strip (back strip)
        5,10,6,11,7,12,8,13,9,14,       // Elements for second triangle strip
        10,15,11,16,12,17,13,18,14,19,  // Elements for third triangle strip
        15,20,16,21,17,22,18,23,19,24,  // Elements for fourth triangle strip (front strip)
    };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerts), floorVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorElements), floorElements, GL_STATIC_DRAW);
}

void RemeshCircularDemo()
{
    // Circular surface as four triangle strips (low resolution)
    // Both vertex positions AND normal vectors
    // Note how this corresponds to the call to glVertexAttribPointer and glEnableVertexAttribArray
    //      in MySetupSurfaces() above.
    float circularVerts[] = {
        // Vertex positions     Normals (please make them *unit* vectors)
        0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,        // Central vertex
        0.0f, 0.866f, 0.5f,     0.0f, 0.866f, 0.5f,     // Over positive z axis
        0.0f, 0.5f, 0.866f,     0.0f, 0.5f, 0.866f,
        0.5f, 0.866f, 0.0f,     0.5f, 0.866f, 0.0f,     // Over positive x-axis
        0.866f, 0.5f, 0.0f,     0.866f, 0.5f, 0.0f,
        0.0f, 0.866f, -0.5f,    0.0f, 0.866f, -0.5f,    // Over negative z axis
        0.0f, 0.5f, -0.866f,    0.0f, 0.5f, -0.866f,
        -0.5f, 0.866f, 0.0f,    -0.5f, 0.866f, 0.0f,    // Over negative x-axis
        -0.866f, 0.5f, 0.0f,    -0.866f, 0.5f, 0.0f,
    };
    // Circular elements (indices to vertices in triangle strips)
    unsigned int circularElements[] = {
        0, 1, 3, 2, 4,            // Elements for first triangle strip 
        0, 3, 5, 4, 6,            // Elements for second triangle strip
        0, 5, 7, 6, 8,            // Elements for third triangle strip
        0, 7, 1, 8, 2            // Elements for fourth triangle strip 
    };
    glBindVertexArray(myVAO[iCircularSurf]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCircularSurf]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circularVerts), circularVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iCircularSurf]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circularElements), circularElements, GL_STATIC_DRAW);
}

void RenderFloorDemo()
{
    glBindVertexArray(myVAO[iFloor]);

    // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
    glVertexAttrib3f(vNormal_loc, 0.0, 1.0, 0.0);    // Generic vertex attribute: Normal is (0,1,0) for the floor.
    myMaterials[0].LoadIntoShaders();
    //glVertexAttrib3f(vColor_loc, 1.0f, 0.4f, 0.4f);	 // Generic vertex attribute: Color (light red) for the floor. 
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);

    // Draw the four triangle strips
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, (void*)0);                              // Draw first triangle strip (back strip)
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, (void*)(10 * sizeof(unsigned int)));    // Draw second triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, (void*)(20 * sizeof(unsigned int)));    // Draw third triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));    // Draw fourth triangle strip (front strip)
}

void RenderCircularDemo()
{
    glBindVertexArray(myVAO[iCircularSurf]);

    // Calculute the model view matrix for the circular surface
    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(2.5, 1.0, 2.5);     // Center in the front right quadrant & raise up
    matDemo.Mult_glScale(3.0, 1.0, 3.0);         // Increase the circular diameter

    // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
    //glVertexAttrib3f(vColor_loc, 1.0f, 0.8f, 0.4f);	 // Generic vertex attribute: Color (yellow-ish) for the circular surface. 
    matDemo.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);

    // Draw the four triangle strips
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, (void*)0);                             // Draw first triangle strip 
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, (void*)(5 * sizeof(unsigned int)));    // Draw second triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, (void*)(10 * sizeof(unsigned int)));   // Draw third triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, (void*)(15 * sizeof(unsigned int)));   // Draw fourth triangle strip 

}


// *********************************************
// THE CODE BELOW MUST BE WRITTEN FOR PROJECT 4.
// *********************************************

void MyRemeshFloor()
{
    // Floor vertices.
    int numFloorVerts = (meshRes + 1) * (meshRes + 1);
    float* floorVerts = new float[3 * numFloorVerts];
    // Floor elements (indices to vertices in a triangle strip)
    int numFloorElts = meshRes * 2 * (meshRes + 1);
    unsigned int* floorElements = new unsigned int[numFloorElts];

    for (int i = 0; i <= meshRes; i++) {
        for (int j = 0; j <= meshRes; j++) {
            //printf("j=%d", j);
            floorVerts[i * (meshRes + 1) * 3 + j * 3] = -5.0f + ((10.0f * j) / (float)meshRes);
            //printf("index =%d: value = %f. \n", i * (meshRes + 1) * 3 + j * 3,floorVerts[i * meshRes*3 + j * 3]);
            floorVerts[i * (meshRes + 1) * 3 + j * 3 + 1] = 0.0f;
            //printf("index =%d: value = %f. \n", i * (meshRes + 1) * 3 + j * 3 + 1, floorVerts[i * meshRes*3 + j * 3+1]);
            floorVerts[i * (meshRes + 1) * 3 + j * 3 + 2] = -5.0f + ((10.0f * i) / (float)meshRes);
            //printf("index =%d: value = %f. \n", i * (meshRes + 1) * 3 + j * 3 + 2, floorVerts[i * meshRes*3 + j * 3 + 2]);
        }
    }

    for (int i = 0; i < meshRes; i++) {
        for (int j = 0; j < 2 * (meshRes + 1); j += 2) {
            floorElements[(meshRes + 1) * 2 * i + j] = i * (meshRes + 1) + j / 2;
            //printf("index = %d, value = %d\n", (meshRes + 1) * 2 * i + j, i * (meshRes + 1) + j/2);
            floorElements[(meshRes + 1) * 2 * i + j + 1] = (i + 1) * (meshRes + 1) + j / 2;
            //printf("index = %d, value = %d\n", (meshRes + 1) * 2 * i + j + 1, (i + 1) * (meshRes + 1) + j/2);
        }
    }

# if 0
    printf("floorVerts:\n");
    printf("%d\n", 3 * numFloorVerts);
    for (int k = 0; k < numFloorVerts; k++) {
        printf("k =%3d:  pos = %7.4f, %7.4f, %7.4f. \n", k, floorVerts[3 * k], floorVerts[3 * k + 1], floorVerts[3 * k + 2]);
    }
    printf("floorElements:\n");
    for (int k = 0; k < numFloorElts; k++) {
        printf("%d, ", floorElements[k]);
    }
#endif
    // Load data into the VBO and EBO using glBindBuffer and glBufferData commands
    // YOU NEED TO WRITE THIS CODE FOR THE PROJECT 4
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBufferData(GL_ARRAY_BUFFER, 3 * numFloorVerts * sizeof(float), floorVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshRes * 2 * (meshRes + 1) * sizeof(unsigned int), floorElements, GL_STATIC_DRAW);

    // The array should have been copied into the GPU buffers now.
    // If you use "new" above, you MUST delete the arrays here to avoid a memory leak.
    delete[] floorVerts;
    delete[] floorElements;
}

// ****
// MyRemeshCircularSurf: To be written for Project 4.
// ****
void MyRemeshCircularSurf()
{
    int numSurfVerts = meshRes * meshRes + 1;
    float* surfVerts = new float[2 * 3 * numSurfVerts];
    float r_inc = 2.9 * PI / meshRes;
    float ang_inc = PI2 / meshRes;
    float cur_ang = 0;

    for (int i = 0; i < meshRes; i++) {

        float r = ang_inc;
        for (int j = 1; j <= meshRes; j++) {
            int curr_ind = (i * meshRes + j) * 6;
            surfVerts[curr_ind] = r * cos(cur_ang);
            surfVerts[curr_ind + 2] = -r * sin(cur_ang);
            surfVerts[curr_ind + 1] = (r * sin(r)) / (r + 1);
            //printf("r = %f\n", r);
            //printf("height = %f\n", (r * sin(r)) / (r + 1));
            float dx = -sin(cur_ang);
            float dy = (r * r * cos(r) + r * cos(r) + sin(r)) / ((r + 1) * (r + 1));
            float dz = -cos(cur_ang);
            float nx = dz * dy;
            float ny = 1;
            float nz = -dx*dy;
            float n = sqrt(nx * nx + ny * ny + nz * nz);
            surfVerts[curr_ind + 3] = nx/n;
            surfVerts[curr_ind + 4] = ny/n;
            surfVerts[curr_ind + 5] = nz/n;

            r += r_inc;
        }
        cur_ang += ang_inc;
    }
    surfVerts[0] = 0;
    surfVerts[1] = 0;
    surfVerts[2] = 0;
    surfVerts[3] = 0;
    surfVerts[4] = 1;
    surfVerts[5] = 0;

    int numSurfElts = meshRes * (2 * meshRes + 1);
    unsigned int* surfElts = new unsigned int[numSurfElts];

    for (int i = 0; i < meshRes; i++) {
        int cur_ind = i * (2 * meshRes + 1);
        surfElts[cur_ind] = 0;
        for (int j = 0; j < meshRes; j++) {
            surfElts[cur_ind + 2 * j + 1] = i * meshRes + j + 1;
            if (i != meshRes - 1) {
                surfElts[cur_ind + 2 * j + 2] = i * meshRes + j + 1 + meshRes;
            }
            else { surfElts[cur_ind + 2 * j + 2] = j + 1; }
        }
    }


    /*
    printf("Verts:\n");
    printf("%d\n", 6 * numSurfVerts);
    for (int k = 0; k < numSurfVerts; k ++) {
        printf("k =%3d:  pos = %7.4f, %7.4f, %7.4f. \n", k, surfVerts[3 * k], surfVerts[3 * k + 1], surfVerts[3 * k + 2]);
    }
        printf("floorElements:\n");
    for (int k = 0; k < numSurfElts; k++) {
        printf("%d, ", surfElts[k]);
    }
    */

    glBindVertexArray(myVAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * (meshRes * meshRes + 1) * sizeof(float), surfVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSurfElts * sizeof(unsigned int), surfElts, GL_STATIC_DRAW);

    delete[] surfVerts;
    delete[] surfElts;
}

// ****
// MyRenderFloor: To be written for Project 4. Renders the square ground plane.
//    This will loop, giving glDrawElements(...) commands. (Compare to RenderFloorDemo above.)
// ****
void MyRenderFloor()
{
    // Render the floor using calls to glDrawElements
    // YOU MUST WRITE THIS FUNCTION FOR PROJECT 4.
    glBindVertexArray(myVAO[iFloor]);

    // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
    glVertexAttrib3f(vNormal_loc, 0.0, 1.0, 0.0);    // Generic vertex attribute: Normal is (0,1,0) for the floor.
    myMaterials[0].LoadIntoShaders();
    //glVertexAttrib3f(vColor_loc, 1.0f, 0.4f, 0.4f);	 // Generic vertex attribute: Color (light red) for the floor. 
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);

    // Draw the four triangle strips
    for (int i = 0; i < meshRes; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, (meshRes + 1) * 2, GL_UNSIGNED_INT, (void*)((meshRes + 1) * 2 * i * sizeof(unsigned int)));
    }
}

// ****
// MyRenderCircularSurf: To be written for Project 4. Renders the circular surface.
//    This will loop, giving glDrawElements(...) commands. (Compare to RenderCircularDemo above.)
// ****
void MyRenderCircularSurf()
{
    glBindVertexArray(myVAO[iCircularSurf]);

    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(2.5, 1.0, 2.5);
    matDemo.Mult_glScale(0.35, 0.7, 0.35);

    // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
    //glVertexAttrib3f(vColor_loc, 1.0f, 0.8f, 0.4f);	 // Generic vertex attribute: Color (yellow-ish) for the circular surface. 
    myMaterials[1].LoadIntoShaders();
    matDemo.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);

    // Draw the four triangle strips
    for (int i = 0; i < meshRes; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, 2 * meshRes + 1, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * i * (2 * meshRes + 1)));
    }
}


