#version 330 core
layout(location = 0) in vec3 iPos;
layout(location = 1) in vec2 iTex;

out vec2 oTex;

uniform mat4 transform;

void main(){
	gl_Position=transform*vec4(iPos,1.0f);
	oTex=iTex;
}