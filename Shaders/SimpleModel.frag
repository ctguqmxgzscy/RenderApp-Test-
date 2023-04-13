#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 Tex;

//定义了该片元着色器需要的所有CPU数据，包括材质和光照数据
struct Material{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight{
    float innerCutOut;
    float outterCutOut;

    float constant;
    float linear;
    float quadratic;

    vec3 position;
    vec3 center;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NUM_POINT_LIGHTS 2

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    //规范化观察者方向和顶点法线方向向量
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 normal = normalize(Normal);
    //使用平行光计算函数返回该顶点应该得到的颜色值
    vec3 result = CalcDirLight(dirLight,normal,viewDir);
    FragColor = vec4(result,1.0f);
} 
//平行光计算，采用Blin-Phong关照模型计算
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    
    //diff
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir,normal),0.0);
    //spec
    vec3 temp = normalize(viewDir +lightDir);
    float spec = pow(max(dot(temp,normal),0.0),material.shininess);

    vec3 ambient = light.ambient * material.diffuse * 0.1;
    vec3 diffuse = diff * light.diffuse * material.diffuse;
    vec3 specular = spec * light.specular * material.specular;
    //将计算得到的颜色返回
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    
    //diff
    vec3 lightDir = normalize(light.position-fragPos);
    float diff = max(dot(lightDir,normal),0.0);
    //spec
    vec3 temp = normalize(lightDir+viewDir);
    float spec = pow(max(dot(temp,normal),0.0),material.shininess);
    //attenuation
    float dis = length(light.position-fragPos);
    float attenuation = 1.0/(light.constant + light.linear * dis +
                        light.quadratic * (dis * dis));

    vec3 ambient = light.ambient * material.diffuse * 0.1;
    vec3 diffuse = diff * light.diffuse * material.diffuse;
    vec3 specular = spec * light.specular * material.specular;
    return (ambient + diffuse + specular) * attenuation;
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 viewDir){
    
    //diff
    vec3 lightDir = normalize(light.position-fragPos); 
    float diff = max(dot(lightDir,normal),0.0);
    //spec 
    vec3 temp = normalize(lightDir+viewDir);
    float spec = pow(max(dot(temp,normal),0.0),material.shininess);
    //limit
    float theta = max(dot(-lightDir,normalize(light.center)),0.0);
    float epsilon = light.innerCutOut-light.outterCutOut;
    float intensity = clamp((theta-light.outterCutOut)/epsilon,0.0f,1.0f);
    //attenuation
    float dis = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis)); 

    vec3 ambient = light.ambient * material.diffuse * 0.1;
    vec3 diffuse = diff * light.diffuse * material.diffuse;
    vec3 specular = spec * light.specular * material.specular;

    return (ambient + diffuse + specular) * intensity * attenuation;
}