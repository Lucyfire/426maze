varying float NL; //get this from the vertex shader

void main()
{
	gl_FragColor = NL * gl_Color;
}



