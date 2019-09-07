varying vec3 normal;
varying vec3 vertexPos;

void main()
{
	vec3 color;
	vec3 n = normalize(normal);
	vec3 eyeVec = normalize(-vertexPos);
	float check = dot(eyeVec, n);
	if((check <= 0.3) && (check >= -0.3))
	{
		color = vec3(0.0, 0.0, 0.0);
	}
	else
	{
		color = vec3(1.0, 1.0, 1.0);
	}
	
	gl_FragColor = vec4(color, 1.0);
}