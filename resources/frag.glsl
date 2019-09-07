#version 120

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;
uniform float i1;
uniform float i2;

varying vec3 normal;
varying vec3 vertexPos;

void main()
{
	vec3 color;

    // need to normalize since barycentric interpolation makes interpolated vectors at the fragments shorter
	// than their values at the vertices
	vec3 n = normalize(normal);

	// computes the light vector given the light position(in camera space) and interpolated fragment position
	vec3 lightVec1 = normalize(lightPos1 - vertexPos);
	vec3 lightVec2 = normalize(lightPos2 - vertexPos);

	// computes diffuse component
	vec3 Diff1 = kd * max(0.0, dot(n,lightVec1));
	vec3 Diff2 = kd * max(0.0, dot(n,lightVec2));

	// computes eye vector
	vec3 eyeVec = normalize(-vertexPos);

	// computes half vector (Blinn variation of Phong Shading)
	vec3 halfVec1 = normalize(lightVec1 + eyeVec);
	vec3 halfVec2 = normalize(lightVec2 + eyeVec);

	// computes specular component
	vec3 Spec1 = ks * pow(max(0.0, dot(halfVec1,n)), s);
	vec3 Spec2 = ks * pow(max(0.0, dot(halfVec2,n)), s);

	// color = ambient + diffuse + specular
	vec3 sum1 = i1 * (ka + Diff1 + Spec1);
	color = (i2 * (ka + Diff2 + Spec2)) + sum1;
	
	gl_FragColor = vec4(color, 1.0);
}
