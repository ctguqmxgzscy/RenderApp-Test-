#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D ScreenTexture;

void main()
{
	FragColor = vec4(vec3(1.0-texture(ScreenTexture,TexCoords)),1.0f);
}