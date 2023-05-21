#version 330 core

out vec4 FragColor;

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

//Tangent Space 
in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
    vec3 Normal;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
    DirLight dirLight;
    PointLight pointLight;
    SpotLight spotLight;

} fs_in;

struct Material{
    bool isNormalMapping;
    bool isDiffuseMapping;
    bool isSpecularMapping;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    float shininess;
};

uniform Material material;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
   
    vec3 normal;
    if(material.isNormalMapping)
    {
        //Obtain normal from normal map in range [0,1]
        normal = texture(material.texture_normal1, fs_in.TexCoords).rgb;
        //Transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);
    }
    else
        normal = fs_in.Normal;

    //Calculate Light 
    vec3 result = CalcDirLight(fs_in.dirLight, normal, viewDir) + 
        CalcPointLight(fs_in.pointLight, normal, fs_in.TangentFragPos, viewDir);
    //Gamma Correction
    result = pow(result.rgb,vec3(1.0/2.2));
    FragColor = vec4(result,1.0f);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    
    //diff
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir,normal),0.0);
    //spec
    vec3 temp = normalize(viewDir +lightDir);
    float spec = pow(max(dot(temp,normal),0.0),material.shininess);

    vec3 ambient, diffuse, specular;
    //Ambient and diffuse
    if(material.isDiffuseMapping)
    {
        ambient = light.ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
        diffuse = diff * light.diffuse * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    }
    else
    {
        ambient = light.ambient * material.ambient;
        diffuse = diff * light.diffuse * material.diffuse;
    }
    //Specular
    if(material.isSpecularMapping)
        specular = spec * light.specular * texture(material.texture_specular1, fs_in.TexCoords).rgb;
    else
        specular = spec * light.specular * material.specular;

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
    float attenuation = 1.0/(dis * dis);

    vec3 ambient, diffuse, specular;
    //Ambient and diffuse
    if(material.isDiffuseMapping)
    {
        ambient = light.ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
        diffuse = light.diffuse * diff * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;    
    }
    else
    {
        ambient = light.ambient * material.ambient;
        diffuse = light.diffuse * diff *  material.diffuse;
    }
    //Specular
    if(material.isSpecularMapping)
        specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;
    else
        specular = light.specular * spec * material.specular;

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
    float theta = dot(lightDir,normalize(-light.direction));
    float epsilon = light.innerCutOut-light.outterCutOut;
    float intensity = clamp((theta-light.outterCutOut)/epsilon,0.0f,1.0f);
    //attenuation
    float dis = length(light.position - fragPos);
    float attenuation = 1.0 /(dis * dis);

    vec3 ambient, diffuse, specular;
    //Ambient and diffuse
    if(material.isDiffuseMapping)
    {
        ambient = light.ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
        diffuse = light.diffuse * diff * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    }
    else
    {
        ambient = light.ambient * material.ambient;
        diffuse = light.diffuse * diff *  material.diffuse;
    }
    //Specular
    if(material.isSpecularMapping)
        specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;
    else
        specular = light.specular * spec * material.specular;

    return (ambient + diffuse + specular) * intensity * attenuation;
}