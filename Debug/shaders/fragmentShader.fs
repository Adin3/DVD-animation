#version 330 core
in vec2 outCoords;
out vec4 fragColor;

uniform sampler2D inTexture;
void main()
{
	fragColor = texture(inTexture, outCoords) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
