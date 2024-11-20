#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;


out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 resultColor = vec3(0.0);

    // 1. Directional Light
    if (dl.enable == 1) {
        // Normalize the direction of the light. The negative sign is used to invert the direction,
        // as the light direction points towards the surface in world space.
        vec3 lightDir = normalize(-dl.direction);

        // Calculate the diffuse component using the dot product of the normalized surface normal 
        // and the light direction. This simulates light scattering and ensures non-negative values 
        // (using max) for surfaces facing away from the light.
        float diff = max(dot(normal, lightDir), 0.0);

        // Compute the reflection direction of the light about the surface normal, 
        // used in specular lighting calculations.
        vec3 reflectDir = reflect(-lightDir, normal);

        // Calculate the specular component using the dot product of the view direction and 
        // the reflection direction, raised to the power of the material's shininess for controlling 
        // highlight sharpness. Use max to ensure non-negative values.
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        // Compute the ambient term, representing global illumination, 
        // by multiplying the material's ambient reflectivity with the light's color.
        vec3 ambient = material.ambient * dl.lightColor;

        // Compute the diffuse term, representing the light scattered across the surface, 
        // scaled by the material's diffuse reflectivity and the light's color.
        vec3 diffuse = material.diffuse * diff * dl.lightColor;

        // Compute the specular term, representing the shiny highlights on the surface, 
        // scaled by the material's specular reflectivity and the light's color.
        vec3 specular = material.specular * spec * dl.lightColor;

        resultColor += ambient + diffuse + specular;
    }

    // 2. Point Light
    if (pl.enable == 1) {
        vec3 lightDir = normalize(pl.position - FragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        float distance = length(pl.position - FragPos);
        float attenuation = 1.0 / (pl.constant + pl.linear * distance + pl.quadratic * distance * distance);

        vec3 ambient = material.ambient * pl.lightColor;
        vec3 diffuse = material.diffuse * diff * pl.lightColor;
        vec3 specular = material.specular * spec * pl.lightColor;

        resultColor += attenuation * (ambient + diffuse + specular);
    }

    // 3. Spotlight
    if (sl.enable == 1) {
        vec3 lightDir = normalize(sl.position - FragPos);
        float theta = dot(lightDir, normalize(-sl.direction));
        if (theta > sl.cutOff) {
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

            float distance = length(sl.position - FragPos);
            float attenuation = 1.0 / (sl.constant + sl.linear * distance + sl.quadratic * distance * distance);

            vec3 ambient = material.ambient * sl.lightColor;
            vec3 diffuse = material.diffuse * diff * sl.lightColor;
            vec3 specular = material.specular * spec * sl.lightColor;

            resultColor += attenuation * (ambient + diffuse + specular);
        }
    }

    // Apply texture
    vec4 texColor = texture(ourTexture, TexCoord);
    color = vec4(resultColor, 1.0) * texColor;

}
