#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 TIModelMatrix;
uniform mat4 LightSpaceMatrix; // 新增光源空間矩陣

out vec2 TexCoord;
// Normal of vertex in world space
out vec3 Normal;
// Position of vertex in world space
out vec3 FragPos;
// 將片段位置轉換到光源空間
out vec4 FragPosLightSpace;

// TODO#2: vertex shader / fragment shader
// Note:
//           1. how to write a vertex shader:
//              a. The output is gl_Position and anything you want to pass to the fragment shader. 
//                 (Apply matrix multiplication yourself)
//           2. how to write a fragment shader:
//              a. The output is FragColor (any var is OK)
//           3. Calculate
//              a. For direct light, lighting = ambient + diffuse + specular
//              b. For point light & spot light, lighting = ambient + attenuation * (diffuse + specular)
//              c. Final color = direct + point + spot if all three light are enabled
//           4. attenuation
//              a. Use formula from slides 'shading.ppt' page 20
//           5. For each light, ambient, diffuse and specular color are the same
//           6. diffuse = kd * max(normal vector dot light direction, 0.0)
//           7. specular = ks * pow(max(normal vector dot halfway direction), 0.0), shininess);
//           8. we've set all light parameters for you (see context.h) and definition in fragment shader
//           9. You should pre calculate normal matrix (trasfer normal from model local space to world space) 
//              in light.cpp rather than in shaders

void main() {
    // Transform position to world space
    FragPos = vec3(ModelMatrix * vec4(position, 1.0));
    
    // Transform normal to world space using the precomputed normal matrix
    // TIModelMatrix: The transpose of the inverse of the model matrix, precomputed to correctly transform normals to world space.
    Normal = mat3(TIModelMatrix) * normal;
    
    // Pass texture coordinates to fragment shader
    TexCoord = texCoord;

    // Transform position to clip space
    gl_Position = Projection * ViewMatrix * vec4(FragPos, 1.0);
}
