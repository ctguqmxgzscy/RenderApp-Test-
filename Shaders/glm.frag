#version 330 core
out vec4 FragColor;

in vec2 oTex;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture1, oTex), texture(texture2, oTex), 0.2);
}