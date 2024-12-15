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

float MY_PI = 3.1415926;
vec3 calculateF(vec3 R0, float NdotH)
{
    vec3 F = R0 + (1.0 - R0) * pow( clamp((1.0 - NdotH), 0.0, 1.0), 5.0 );
    return F;
}

float calculateGSchlick(vec3 dir, vec3 h)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float Gschlick = max(dot(dir, h), 0.0) / (max(dot(dir, h), 0.0) * (1.0 - k) + k);
    return Gschlick;
}

float calculateG(vec3 viewDir, vec3 lightDir)
{
    vec3 h = normalize(viewDir + lightDir);
    float G = calculateGSchlick(viewDir, h) * calculateGSchlick(lightDir, h);
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

vec3 calculatePBR(vec3 F0, vec3 cameraDir, vec3 lightDir, float NdotV, float NdotL, float NdotH)
{
    
    //F
    vec3 F = calculateF(F0, NdotH);
    //G
    float G = calculateG(cameraDir, lightDir);
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
        float attenuation = 1.0 / length(lightMessage[i].position - fs_in.vFragPos);
        //only one light
        //fr = fms * fadd + fpbr
        vec3 fpbr = calculatePBR(F0, cameraDir, lightDir, NdotV, NdotL, NdotH);
        vec3 F = calculateF(F0, NdotH);
        vec3 fd = vec3(1.0) - F;
        fd *= (1.0 - metallic);
        fpbr += (fd * workAlbedo / MY_PI);
        objColor += fpbr * lightMessage[i].color * NdotL * attenuation;
    }

    //vec3 ambient = vec3(0.03) * workAlbedo;
    //objColor += ambient;
    objColor /= (objColor + vec3(1.0));
    FragColor = vec4(pow(objColor, vec3(1.0 / 2.2)), 1.0);
}