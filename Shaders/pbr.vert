#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT{
	vec3 WorldPos;
    vec2 TexCoords;
    vec3 Normal;
} vs_out;


void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.WorldPos = vec3(model * vec4(position, 1.0f));
	vs_out.TexCoords = texCoords;
    vs_out.Normal = normalMatrix * normal; 
}


