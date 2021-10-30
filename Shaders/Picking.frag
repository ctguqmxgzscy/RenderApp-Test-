//#version 410                                                                        
//#extension GL_EXT_gpu_shader4 : enable  

#version 330

uniform uint ModelIndex;
uniform uint MeshIndex;
out vec3 FragColor;
void main()
{
        FragColor = vec3(float(ModelIndex), float(MeshIndex),float( gl_PrimitiveID + 1));
}
