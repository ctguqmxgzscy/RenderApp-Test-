#version 330 core

uniform uint ModelIndex;
uniform uint MeshIndex;
uniform float MeshSize;
uniform float IndicesSize;

out vec3 FragColor;
void main()
{
        FragColor = vec3(float(ModelIndex), float(MeshIndex/MeshSize),float( (gl_PrimitiveID + 1)/IndicesSize));
}
