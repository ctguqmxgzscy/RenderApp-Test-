#version 330 core
//顶点数组布局，告诉GPU哪些是位置，哪些是颜色
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

//将要传给下一阶段着色器的数据
out vec3 oColor;

//能够从CPU中获得过来的数据
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	//告诉GPU将要绘制的顶点的位置就是传进来的位置值
	gl_Position=projection * view * model * vec4(aPos.x,aPos.y,aPos.z,1.0);
	//片段着色器将接受顶点着色器传入的颜色值
	oColor=aColor;
}
