#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aCoords;
out vec2 outCoords;
void main()
{
	gl_Position = vec4(aPos, 1.0f);
	outCoords = vec2(aCoords.x, aCoords.y);
}