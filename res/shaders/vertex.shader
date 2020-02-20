#version 330 core
layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aColor;

//out vec3 ourColor; // output a color to the fragment shader

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0f);
	//ourColor = aColor; // set ourColor to the input color we got from the vertex data
	//ourColor = vec3(0.0f, 0.0f, 0.0f);
}