#version 420 core
layout(location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vTexCoord;
} fs_in;

struct Light
{
    vec3 position;
    vec3 color;
};

uniform vec3 cameraPos;
uniform Light lightMessage[4];
uniform float roughness;
uniform vec3 workAlbedo; //in this work to calculate init F0
uniform float metallic; // in this work to calculate init F0
uniform samplerCube enviromentMap;
uniform sampler2D prePBR;
uniform samplerCube specularEnviromentMap;
uniform int openIBL;

float MY_PI =  3.14159265359;
vec3 calculateF(vec3 R0, float HdotV)
{
    vec3 F = R0 + (1.0 - R0) * pow( clamp((1.0 - HdotV), 0.0, 1.0), 5.0 );
    return F;
}

float calculateGSchlick(float NdotV)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float Gschlick = NdotV / (NdotV * (1.0 - k) + k);
    return Gschlick;
}

float calculateG(vec3 viewDir, vec3 lightDir, vec3 normal)
{
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float G = calculateGSchlick(NdotV) * calculateGSchlick(NdotL);
    return G;
}

float calculateD(float NdotH)
{
    float a = roughness * roughness;
    float left = pow(NdotH, 2.0);
    float right = (a * a - 1.0);
    float denominator = MY_PI * pow(left * right + 1.0, 2.0);
    return a * a / denominator;
}

vec3 calculatePBR(vec3 F0, vec3 cameraDir, vec3 lightDir, float NdotV, float NdotL, float NdotH, float HdotV)
{
    
    //F
    vec3 F = calculateF(F0, HdotV);
    //G
    float G = calculateG(cameraDir, lightDir, fs_in.vNormal);
    //D
    float D = calculateD(NdotH);

    float denominator = 4.0 * NdotL * NdotV + 0.00001;

    return F * G * D / denominator;
}


void main()
{
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, workAlbedo, metallic);
    vec3 objColor = vec3(0.0);

    //cameraDir
    vec3 cameraDir = normalize(cameraPos - fs_in.vFragPos);

    for(int i = 0; i < 4; i++)
    {
        vec3 fr = vec3(0.0);
        vec3 lightDir = normalize(lightMessage[i].position - fs_in.vFragPos);
        vec3 h = normalize(cameraDir + lightDir);
        float NdotH = max(dot(h,fs_in.vNormal), 0.0);
        float NdotL = max(dot(lightDir, fs_in.vNormal), 0.0);
        float NdotV = max(dot(cameraDir, fs_in.vNormal), 0.0);
        float HdotV = max(dot(cameraDir, h), 0.0);
        float distance = length(lightMessage[i].position - fs_in.vFragPos);
        float attenuation = 1.0 / (distance * distance);
        //only one light
        //fr = fms * fadd + fpbr
        vec3 fpbr = calculatePBR(F0, cameraDir, lightDir, NdotV, NdotL, NdotH, HdotV);
        vec3 F = calculateF(F0, NdotH);
        vec3 fd = vec3(1.0) - F;
        fd *= (1.0 - metallic);

        fpbr += (fd * workAlbedo / MY_PI);
        objColor += fpbr * lightMessage[i].color * NdotL * attenuation;
    }
    vec3 F = calculateF(F0, max(dot(fs_in.vNormal, cameraDir), 0.0));
    vec3 KD = 1.0 - F;
    KD *= (1.0 - metallic);



    vec3 ambient = KD * texture(enviromentMap, normalize(fs_in.vNormal)).rgb * workAlbedo;
    vec3 R = reflect(-cameraDir, fs_in.vNormal);
    vec3 prefilterColor = textureLod(specularEnviromentMap, R, roughness * 4.0).rgb;
    vec2 specularPBR = texture(prePBR, vec2(max(dot(fs_in.vNormal, cameraDir), 0.0), roughness)).rg;
    
    vec3 ambientSpecular = prefilterColor * (specularPBR.x * F0 + specularPBR.y);
    ambient += ambientSpecular;

    if(openIBL == 1)
    {
        objColor += ambient;
    }
    
    objColor /= (objColor + vec3(1.0));
    FragColor = vec4(pow(objColor, vec3(1.0 / 2.2)), 1.0);
}