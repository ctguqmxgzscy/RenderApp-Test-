#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 Tex;
  
struct Material{
    sampler2D diffuse;
    sampler2D specular;
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
#define NUM_POINT_LIGHTS 4

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
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 normal = normalize(Normal);
    vec3 result = CalcDirLight(dirLight,normal,viewDir);
    for(int i=0; i<NUM_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i],normal,FragPos,viewDir);
    result += CalcSpotLight(spotLight,normal,FragPos,viewDir);
    FragColor = vec4(result,1.0f);
} 
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    
    //diff
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir,normal),0.0);
    //spec
    vec3 temp = normalize(viewDir +lightDir);
    float spec = pow(max(dot(temp,normal),0.0),material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuse,Tex).rgb;
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse,Tex).rgb;
    vec3 specular = spec * light.specular * texture(material.specular,Tex).rgb;

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
    vec3 ambient = light.ambient * texture(material.diffuse,Tex).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse,Tex).rgb;
    vec3 specular = light.specular * spec * texture(material.specular,Tex).rgb;

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

    vec3 ambient = light.ambient * texture(material.diffuse,Tex).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse,Tex).rgb;
    vec3 specular = light.specular * spec * texture(material.specular,Tex).rgb;

    return (ambient + diffuse + specular) * intensity * attenuation;
}