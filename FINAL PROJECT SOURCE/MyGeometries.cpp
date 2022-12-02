//
//  MyGeometries.cpp - Fall 2022
//
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #6.
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"       // Adjust path as needed.
#include "LinearR4.h"       // Adjust path as needed.
#include "MathMisc.h"       // Adjust path as needed

#include "MyGeometries.h"
#include "TextureProj.h"
#include "PhongData.h"
#include "RgbImage.h"
#include "GlGeomCylinder.h"
#include "GlGeomSphere.h"
#include "GlGeomTorus.h"

// **********************************
// Material to underlie a texture map.
// YOU MAY DEFINE A SECOND ONE OF THESE IF YOU WISH
// **********************************
phMaterial materialUnderTexture;

// **************************
// Information for loading textures
// **************************
const int NumTextures = 8;
unsigned int TextureNames[NumTextures];     // Texture names generated by OpenGL
const char* TextureFiles[NumTextures] = {
    "snorlax_body.bmp",
    "snorlax_top.bmp",
    "road.bmp",
    "concrete_base.bmp",
    "flowers.bmp",
    "snow.bmp",
    "night.bmp",
    "sunset.bmp"
};

// *******************************
// For spheres and a cylinder and a torus (Torus is currently not used.)
// *******************************
GlGeomSphere texSphere(4, 4);
GlGeomCylinder texCylinder(4, 4, 4);
GlGeomTorus texTorus(4, 4, 0.75);


// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************
const int NumObjects = 5;
const int iFloor = 0;
const int iTempleGround = 1;
const int iTemple = 2;            // RESERVED FOR USE BY 155A PROJECT
const int iLawn1 = 3;
const int iLawn2 = 4;

const int spawnX = 0;
const int spawnY = 0;
const int spawnZ = 0;
const int snorlaxBodyHeight = 2;
const int snorlaxTopHeight = 1.7;

unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;
unsigned int myEBO[NumObjects];  // a Element Array Buffer Object - holds an array of elements (vertex indices)

// ********************************************
// This sets up for texture maps. It is called only once
// ********************************************
void SetupForTextures()
{
    // This material goes under the textures.
    // IF YOU WISH, YOU MAY DEFINE MORE THAN ONE OF THESE FOR DIFFERENT GEOMETRIES
    materialUnderTexture.SpecularColor.Set(0.9, 0.9, 0.9);
    materialUnderTexture.AmbientColor.Set(0.3, 0.3, 0.3);
    materialUnderTexture.DiffuseColor.Set(0.7, 0.7, 0.7);       // Increase or decrease to adjust brightness
    materialUnderTexture.SpecularExponent = 40.0;

    // ***********************************************
    // Load texture maps
    // ***********************************************
    RgbImage texMap;

    glUseProgram(shaderProgramBitmap);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(NumTextures, TextureNames);
    for (int i = 0; i < NumTextures; i++) {
        texMap.LoadBmpFile(TextureFiles[i]);            // Read i-th texture from the i-th file.
        glBindTexture(GL_TEXTURE_2D, TextureNames[i]);  // Bind (select) the i-th OpenGL texture

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set best quality filtering.   Also see below for disabling mipmaps.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // Requires that mipmaps be generated (see below)
        // You may also try GL_LINEAR_MIPMAP_NEAREST -- try looking at the wall from a 30 degree angle, and look for sweeping transitions.

        // Store the texture into the OpenGL texture named TextureNames[i]
        int textureWidth = texMap.GetNumCols();
        int textureHeight = texMap.GetNumRows();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texMap.ImageData());
#if 1
        // Use mipmaps  (Best!)
        glGenerateMipmap(GL_TEXTURE_2D);
#else
        // Don't use mipmaps.  Try moving away from the brick wall a great distance
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

    }

    // Make sure that the shaderProgramBitmap uses the GL_TEXTURE_0 texture.
    glUseProgram(shaderProgramBitmap);
    glUniform1i(glGetUniformLocation(shaderProgramBitmap, "theTextureMap"), 0);
    glActiveTexture(GL_TEXTURE0);


}

// **********************
// This sets up geometries needed for 
//   (a) the floor (ground plane)
//   (b) the back wall
//   (c) the circular mesh
//   (d) two spheres
//   (e) one cylinder
//  It is called only once.
//  YOU NEED TO CHANGE THIS ONCE YOU ADD THE TEXTURE COORDINATES TO THE CIRCULAR SURFACE.
// **********************
void MySetupSurfaces() {

    texSphere.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
    texCylinder.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
    texTorus.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);

    // Initialize the VAO's, VBO's and EBO's for the ground plane, the back wall
    // and the surface of rotation. Gives them the "vertPos" location,
    // and the "vertNormal"  and the "vertTexCoords" locations in the shader program.
    // No data is loaded into the VBO's or EBO's for the circular surface until the "Remesh"
    //   routines is called

    glGenVertexArrays(NumObjects, &myVAO[0]);
    glGenBuffers(NumObjects, &myVBO[0]);
    glGenBuffers(NumObjects, &myEBO[0]);

    check_for_opengl_errors();      // Watch the console window for error messages!
}

void MyRemeshGeometries()
{
    // IT IS NOT NECESSARY TO REMESH EITHER THE FLOOR OR THE BACK WALL
    // YOU DO NOT NEED TO CHANGE THIS FOR PROJECT #6.

    texSphere.Remesh(meshRes, meshRes);
    texTorus.Remesh(meshRes, meshRes);

    check_for_opengl_errors();      // Watch the console window for error messages!
}

// **********************************************
// MODIFY THIS ROUTINE TO RENDER THE FLOOR, THE BACK WALL,
//    AND THE SPHERES AND THE CYLINDER. -- WITH TEXTURES
// **********************************************

void MyRenderGeometries() {

    float matEntries[16];       // Temporary storage for floats

    renderTempleLawn();
    // Render the Floor - using a procedural texture map
    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[iFloor]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[2]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the floor as a single triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
    check_for_opengl_errors();

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[iLawn1]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[4]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the floor as a single triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
    check_for_opengl_errors();

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[iLawn2]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[4]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
    check_for_opengl_errors();

    // ******
    // Render the wall - using a procedural texture map
    // ******
    renderTemple();

    if (renderFloorOnly) {
        return;
    }

    renderChar();

    // **************
    // Render the circular surface
    selectShaderProgram(shaderProgramBitmap);
    glBindTexture(GL_TEXTURE_2D, TextureNames[6]);
    glUniform1i(applyTextureLocation, true);
    //SamsRenderCircularSurf();
    glUniform1i(applyTextureLocation, false);
    check_for_opengl_errors();

    // *************
    // Render two spheres and a cylinder
    // YOU MUST MODIFY THE CODE BELOW, AND ADD TEXTURES TO THE SPHERE AND THE CAPS OF THE CYLINDER.

    // Render a sphere with a earth image texture map
    //  YOU DO NOT NEED TO MODIFY THIS SPHERE OR ITS TEXTURE MAP
    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(-3.5, 1.1, 0.4);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[7]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    //texSphere.Render();                                 // Render the sphere
    glUniform1i(applyTextureLocation, false);

    // Render a sphere with a texture map 
    // ADD A TEXTURE MAP TO THIS SPHERE - SIMILAR TO THE CODE ABOVE
    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(3.5, 1.1, 0.4);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[5]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    //texSphere.Render();                                 // Render the sphere
    glUniform1i(applyTextureLocation, false);                               // Render the sphere


    // Render a cylinder 
    // YOU MAY LEAVE THE TEXTURE MAP ON THE SIDE OF THE CYLINDER
    // YOU MUST ADD TEXTURE MAPS TO THE TOP AND BOTTOM FACES OF THE CYLINDER
    // NOTE THERE ARE SEPARATE RENDER COMMANDS FOR THE THREE PARTS OF THE CYLINDER.
    // YOU WILL ALSO CHANGE THE ORIENTATION OF THE CYLINDER
    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(0.0, 1.1, 1.0);
    matDemo.Mult_glRotate(PI / 2, 0, 0, 1);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[2]);     // Choose texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    //texCylinder.RenderSide();                             // Render the sphere side
    glBindTexture(GL_TEXTURE_2D, TextureNames[4]);     // Choose texture
    glUniform1i(applyTextureLocation, true);             // Turn off applying texture!
    //texCylinder.RenderTop();                              // RENDER THIS WITH A TEXTURE MAP
    //texCylinder.RenderBase();                             // RENDER THIS WITH A TEXTURE MAP
    glUniform1i(applyTextureLocation, false);


    check_for_opengl_errors();      // Watch the console window for error messages!
}

void renderChar() {

    float matEntries[16];       // Temporary storage for floats
    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(spawnX, spawnY + snorlaxBodyHeight, spawnZ);
    matDemo.Mult_glScale(snorlaxBodyHeight, snorlaxBodyHeight, snorlaxBodyHeight);
    matDemo.Mult_glRotate(snorlaxPOV, 0, 1, 0);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[0]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    texSphere.Render();                              // Render the sphere
    texSphere.Remesh(meshRes, meshRes);
    glUniform1i(applyTextureLocation, false);

    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(spawnX, spawnY + snorlaxBodyHeight*2 + snorlaxTopHeight, spawnZ);
    matDemo.Mult_glScale(snorlaxTopHeight, snorlaxTopHeight, snorlaxTopHeight);
    matDemo.Mult_glRotate(snorlaxPOV, 0, 1, 0);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[1]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    texSphere.Render();
    texSphere.Remesh(meshRes, meshRes);
}

void renderTempleLawn() {

    float floorVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -2.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // Back left
         2.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 1.0f,         // Back right
         2.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 0.0f,         // Front right
        -2.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // Front left
    };
    unsigned int floorElts[] = { 0, 3, 1, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBindVertexArray(myVAO[iFloor]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerts), floorVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);    // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);                                 // Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));    // Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);                                  // Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);                                   // Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorElts), floorElts, GL_STATIC_DRAW);

    float lawnVerts1[] = {
        // Position              // Normal                  // Texture coordinates
        -8.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // Back left
        -2.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 1.0f,         // Back right
        -2.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 0.0f,         // Front right
        -8.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // Front left
    };
    unsigned int lawnElts1[] = { 0, 3, 1, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iLawn1]);
    glBindVertexArray(myVAO[iLawn1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lawnVerts1), lawnVerts1, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);    // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);                                 // Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));    // Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);                                  // Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);                                   // Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iLawn1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lawnElts1), lawnElts1, GL_STATIC_DRAW);

    float lawnVerts2[] = {
        // Position              // Normal                  // Texture coordinates
        2.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // Back left
        8.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 1.0f,         // Back right
        8.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.5f, 0.0f,         // Front right
        2.0f - snorlaxPOSx, 0.0f,  8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // Front left
    };
    unsigned int lawnElts2[] = { 0, 3, 1, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iLawn2]);
    glBindVertexArray(myVAO[iLawn2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lawnVerts2), lawnVerts2, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);    // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);                                 // Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));    // Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);                                  // Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);                                   // Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iLawn2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lawnElts2), lawnElts2, GL_STATIC_DRAW);

}
void renderTemple() {

    float matEntries[16];       // Temporary storage for floats

    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(0 - snorlaxPOSx, 2.0, -14 - snorlaxPOSy);
    matDemo.Mult_glRotate(PI/4, 0, 1, 0);
    matDemo.Mult_glScale(11, 2, 11);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[3]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    texCylinder.RenderSide();       
    texCylinder.RenderTop();
    texCylinder.RenderBase();
    texCylinder.Remesh(4, 4, 4);
    glUniform1i(applyTextureLocation, false);

    float baseBottomVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -8.0f - snorlaxPOSx, 3.0f, -8.0f - snorlaxPOSy,       0.0f, 0.0f, 1.0f,          0.0f, 0.5f,
        -8.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,       0.0f, 0.0f, 1.0f,          0.0f, 0.0f,         // Back right
        8.0f - snorlaxPOSx, 3.0f, -8.0f - snorlaxPOSy,  0.0f, 0.0f, 1.0f,          0.25f, 0.5f,         // Back left
        8.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,        0.0f, 0.0f, 1.0f,          0.25f, 0.0f,         // Back left
        8.0f - snorlaxPOSx, 3.0f,  -16.0f - snorlaxPOSy,      0.0f, 0.0f, 1.0f,          0.5f, 0.5f,         // Front right
        8.0f - snorlaxPOSx, 0.0f,  -16.0f - snorlaxPOSy,      0.0f, 0.0f, 1.0f,          0.5f, 0.0f,         // Front left
        -8.0f - snorlaxPOSx, 3.0f,  -16.0f - snorlaxPOSy,      0.0f, 0.0f, 1.0f,          0.75f, 0.5f,         // Front right
        -8.0f - snorlaxPOSx, 0.0f,  -16.0f - snorlaxPOSy,      0.0f, 0.0f, 1.0f,          0.75f, 0.0f,         // Front left
        -8.0f - snorlaxPOSx, 3.0f, -8.0f - snorlaxPOSy,       0.0f, 0.0f, 1.0f,           1.0f, 0.5f,
        -8.0f - snorlaxPOSx, 0.0f, -8.0f - snorlaxPOSy,       0.0f, 0.0f, 1.0f,          1.0f, 0.0f,         // Back right
    };
    unsigned int baseBottomElts[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };


    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iTemple]);
    glBindVertexArray(myVAO[iTemple]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseBottomVerts), baseBottomVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);    // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);                                 // Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));  // Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);                                  // Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);                                   // Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iTemple]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(baseBottomElts), baseBottomElts, GL_STATIC_DRAW);

    float baseGroundVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -8.0f - snorlaxPOSx, 3.0f, -16.0f - snorlaxPOSy,    0.0f, 1.0f, 0.0f,           0.0f, 1.0f,
        -8.0f - snorlaxPOSx, 3.0f, -8.0f - snorlaxPOSy,     0.0f, 1.0f, 0.0f,           0.0f, 0.0f,
         8.0f - snorlaxPOSx, 3.0f, -16.0f - snorlaxPOSy,     0.0f, 1.0f, 0.0f,           1.0f, 1.0f,
        8.0f - snorlaxPOSx, 3.0f, -8.0f - snorlaxPOSy,      0.0f, 1.0f, 0.0f,           1.0f, 0.0f,
    };
    unsigned int baseGroundElts[] = { 0, 1, 2, 3 };


    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iTempleGround]);
    glBindVertexArray(myVAO[iTempleGround]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseGroundVerts), baseGroundVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);    // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);                                 // Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));  // Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);                                  // Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);                                   // Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iTempleGround]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(baseGroundElts), baseGroundElts, GL_STATIC_DRAW);

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[iTemple]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[3]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the wall as a single triangle strip
   //glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[iTempleGround]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[3]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the wall as a single triangle strip
    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
    check_for_opengl_errors();
}