#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 dir;
    vec3 color;
};

//struct PointLight {
//    vec3 pos;
//    vec3 color;
//
//    float K0;
//    float K1;
//    float K2;
//};

//struct SpotLight {
//    vec3 pos;
//    vec3 dir;
//    float cutOff;
//    float softOff;
//
//    float K0;
//    float K1;
//    float K2;
//
//    vec3 color;
//};

in vec3 Normal;
in vec3 FragPos;


uniform vec3 viewPos;
uniform vec3 objectColor;

uniform DirLight dirLight;
//uniform PointLight pointLights[];
//uniform SpotLight spotLight;

//uniform int NUM_OF_POINT_LIGHTS;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color;

    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    vec3 specular = vec3(0, 0, 0);

    if (dot(-lightDir, normal) < 0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = spec * light.color;
    }


    return (ambient + diffuse + specular) * objectColor;
}

//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
//    vec3 lightDir = normalize(light.pos - fragPos);
//
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * light.color;
//
//    float diff = max(dot(normal, lightDir), 0.0);
//    vec3 diffuse = diff * light.color;
//
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = spec * light.color;
//
//    float distance = length(light.pos - fragPos);
//    float attenuation = 1.0 / (light.K0 + light.K1 * distance + light.K2 * distance * distance);
//
//    return (ambient + diffuse + specular) * objectColor * attenuation;
//}
//
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
//    // ambient
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * light.color;
//
//    // diffuse
//    vec3 norm = normalize(normal);
//    vec3 lightDir = normalize(light.pos - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * light.color;
//    //     vec3 diffuse = vec3(0,0,0);
//
//    // specular
//    float specularStrength = 0.5;
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * light.color;
//    //     vec3 specular = vec3(0,0,0);
//
//
//    // spotlight
//    float theta = dot(lightDir, normalize(-light.dir));
//    float epsilon = (light.cutOff - light.softOff);
//    float intensity = clamp((theta - light.softOff)/epsilon, 0.0, 1.0);
//
//    diffuse *= intensity;
//    specular *= intensity;
//
//    //attenuation`
//    float distance = length(light.pos - FragPos);
//    float attenuation = 1.0 / (light.K0 + light.K1 * distance + light.K2 * distance *distance);
//
//
//    return (ambient + diffuse + specular) * objectColor * attenuation;
//}


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 res = CalcDirLight(dirLight, norm, viewDir);

    //    res += CalcPointLight(pointLights[0], norm, FragPos, viewDir);
    //
    //    res += CalcPointLight(pointLights[1], norm, FragPos, viewDir);
    //
    //    res += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(res, 1.0);
}