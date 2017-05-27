


varying float NL; //pass this to the fragment shader


void main()
{
	gl_Position = ftransform();                  //perform transformations for currect vertex
	vec3 normal = gl_NormalMatrix * gl_Normal;   //get the current normal
	vec3 light = vec3(100.0, 100.0, 0.0);          //hard coded light position
	NL = dot(normal, normalize(light));         //N dot L
	gl_FrontColor = gl_Color;                    //pass current color to the fragment shader
}


