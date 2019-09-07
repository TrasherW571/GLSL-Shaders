#version 120

uniform mat4 P;
uniform mat4 MV;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space

varying vec3 normal;
varying vec3 vertexPos;

void main()
{
	gl_Position = P * MV * aPos;
	//Transforms normal vector from object space to camera space
	normal = vec3(MV * vec4(aNor, 0.0));
	// Transforms vertex from object space to camera space
	vertexPos = vec3(MV * aPos);
}
