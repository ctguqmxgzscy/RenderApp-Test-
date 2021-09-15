#version 330 core
out vec4 FragColor;
uniform vec4 outColor;
in vec3 myColor;

void main(){
	FragColor=vec4( myColor.x+outColor.x,
					myColor.y+outColor.y,
					myColor.z+outColor.z,
					1.0f);

}		