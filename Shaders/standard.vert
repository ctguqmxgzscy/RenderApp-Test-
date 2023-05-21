#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight{
    float innerCutOut;
    float outterCutOut;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

out VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
    vec3 Normal;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
    DirLight dirLight;
    PointLight pointLight;
    SpotLight spotLight;
} vs_out;


void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.FragPos = vec3(model * vec4(position, 1.0f));
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(T, N);
	//Inverse Matrix TBN From World-Space to TBN-Space
	mat3 TBN = transpose(mat3(T, B, N));
    //DirLight
    vs_out.dirLight = dirLight;
    vs_out.dirLight.direction = TBN * dirLight.direction;
    //PointLight
    vs_out.pointLight = pointLight;
    vs_out.pointLight.position = TBN * pointLight.position;
    //SpotLight
    vs_out.spotLight = spotLight;
    vs_out.spotLight.position = TBN * spotLight.position;
    vs_out.spotLight.direction = TBN * spotLight.direction;

    vs_out.Normal = TBN * N;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;
}