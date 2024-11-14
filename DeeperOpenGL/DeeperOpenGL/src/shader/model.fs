#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vPos;

struct Texture
{
    sampler2D diffuse1;
    sampler2D diffuse2;
    sampler2D diffuse3;

    sampler2D specular1;
    float k;
    
    sampler2D ambient1;
    
    sampler2D height1;

    samplerCube enviromentLight;
};
uniform Texture material;
uniform vec3 cameraPos;
uniform int haveEnviromentLight;
vec3 calculateLightDir(vec3 normal, vec3 camera_pos, vec3 object_pos)
{
    vec3 camera_to_object = normalize(object_pos - camera_pos);
    return normalize(reflect(camera_to_object, normal));
}
vec3 calculateDiffuse(vec3 diffuseM, vec3 normal, vec3 camera_pos, vec3 object_pos)
{
    
    if(haveEnviromentLight == 0)
    {
        return diffuseM;
        
    }
    else{
        vec3 object_reflect = calculateLightDir(normal, camera_pos, object_pos);
    
        vec3 eLight = vec3(1.0f);
    
    

        eLight = texture(material.enviromentLight, object_reflect).rgb;
    
    
        vec3 object_to_camera = normalize(camera_pos - object_pos);

        return diffuseM * eLight;
    }
    
}
vec3 calculateSpecular(vec3 specularM, vec3 normal, vec3 camera_pos, vec3 object_pos)
{
    if(haveEnviromentLight == 0)
    {
        return specularM;
        
    }
    else{
        vec3 object_reflect = calculateLightDir(normal, camera_pos, object_pos);
    
        vec3 eLight = vec3(1.0f);
    
    

        eLight = texture(material.enviromentLight, object_reflect).rgb;

        
        vec3 object_to_camera = normalize(camera_pos - object_pos);

        vec3 h = normalize(object_reflect + object_to_camera);
        return specularM * eLight;
    }

}
void main()
{
    

    vec3 diffuseColor1 = texture(material.diffuse1, vTexCoord).rgb;
    vec3 diffuseColor2 = texture(material.diffuse2, vTexCoord).rgb;
    vec3 diffuseColor3 = texture(material.diffuse3, vTexCoord).rgb;

    vec3 specularColor1 = texture(material.specular1, vTexCoord).rgb;
    float shininess = material.k;

    vec3 ambientColor1 = texture(material.ambient1, vTexCoord).rgb;

    vec3 heightNormal = texture(material.height1, vTexCoord).rgb;
    vec3 normal = heightNormal + vNormal;

    vec3 objectPos = (vPos / vPos.w).xyz;

    vec3 finalColor = calculateDiffuse(diffuseColor1, normal, cameraPos, objectPos) + calculateSpecular(specularColor1, normal, cameraPos, objectPos);
    FragColor = vec4(finalColor, 1.0);
    
}