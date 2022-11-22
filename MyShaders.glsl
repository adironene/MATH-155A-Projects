// ***************************
// Shader programs to be used in conjunction with the
//  Phong lighting shaders of EduPhong.glsl
// Are first used with Project #6, Math 155A, Winter 2019
//
// Revision: Feb 23, 2019
// ***************************

// #beginglsl ...  #endglsl mark begining and end of code blocks.
// Syntax for #beginglsl is:
//
//   #beginglsl <shader-type> <shader-code-name>
//
// where <shader-type> is
//      vertexshader or fragmentshader or geometryshader or codeblock,
// and <shader-code-name> is used to compile/link the shader into a 
//      shader program.
// A codeblock is meant to be used as a portion of a larger shader.

// *****************************
// applyTextureMap - code block
//    applyTextureMap() is called after the Phong lighting is calculated.
//        - It returns a vec4 color value, which is used as the final pixel color.
//    Inputs: (all global variables)
//        - nonspecColor and specularColor (global variables, vec3 objects)
//        - theTexCoords (the texture coordinates, a vec2 object)
//    Returns a vec4:
//       - Will be used as the final fragment color
// *****************************
#beginglsl codeblock MyProcTexture
// vec3 nonspecColor;		// These items already declared 
// vec3 specularColor;
// vec2 theTexCoords;

uniform sampler2D theTextureMap;	// An OpenGL texture map

bool InMshape( vec2 pos );	// Function prototype
bool InLshape( vec2 pos );	// Function prototype
bool InHshape( vec2 pos );	// Function prototype

vec4 applyTextureFunction() {
	vec2 wrappedTexCoords = fract(theTexCoords);	// Wrap s,t to [0,1].
	vec4 myColor = vec4(nonspecColor, 1.0f)*texture(theTextureMap, theTexCoords);
	if ( InMshape(wrappedTexCoords) || InLshape(wrappedTexCoords) || InHshape(wrappedTexCoords)) {	
		return vec4( 0, 0, 0, 1 );                // Black color inside the "F"
	}
	else {
		vec3 combinedPhongColor = nonspecColor+specularColor;
		//myColor += vec4(combinedPhongColor, 0.5f);
        //return vec4(combinedPhongColor, 1.0f);   // Use the Phong light colors
        return myColor;
	}
}

bool InMshape( vec2 pos ) {
	float verticalMargin = 0.1127;
	float xL = -1.5*sqrt(0.1-((pos.y-0.5)*(pos.y-0.5)))+0.79;
	float xR = -0.8*sqrt(0.1-((pos.y-0.5)*(pos.y-0.5)))+0.79;
	if(pos.y<verticalMargin || pos.y>1.0-verticalMargin) {
			 return false;
	}
	return pos.x>=xL && pos.x <=xR;
}

bool InLshape( vec2 pos ){
	float landscape = 0.05* sin(pos.x*20)*cos(20*pos.x*pos.x)+0.15;
	return pos.y<= landscape;
}

bool InHshape(vec2 pos){
	float xCut = 0.3;
	if(pos.x>xCut){
		return false;
	}
	else if(pos.x<=0.15){
	float hill = 20*pos.x*pos.x +0.08;
		return pos.y<=hill;
	}else{
	float hill = 20*(pos.x-0.3)*(pos.x-0.3)+0.08;
		return pos.y<=hill;
	}
}
#endglsl
