#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangentU;
layout (location = 3) in vec2 aTexC;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    //防止model含有非等比缩放操作导致法线与模型表面不在垂直
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    gl_Position = projection * view * vec4(FragPos, 1.0);
}