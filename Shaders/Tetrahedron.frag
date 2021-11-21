#version 330 core

in vec3 oColor;
out vec4 FragColor;

void main(){
	//告诉该片元的颜色值为oColor
	FragColor=vec4(oColor,1.0f);
}		