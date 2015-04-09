#version 110

varying vec3 V;
varying vec3 N;

void main()
{
	// pass the vertex position to the fragment shader
	V = vec3(gl_ModelViewMatrix * gl_Vertex);

	// pass the normal to the fragment shader      
	N = normalize(gl_NormalMatrix * gl_Normal);

	gl_TexCoord[0]		= gl_MultiTexCoord0;
	gl_Position			= gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor		= gl_Color;
}
