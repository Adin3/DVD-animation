#version 330 core
in vec2 outCoords;
out vec4 fragColor;

uniform sampler2D inTexture;
uniform vec4 color;
void main()
{
	fragColor = texture(inTexture, outCoords) * color;
}
