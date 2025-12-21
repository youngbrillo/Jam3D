#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D depthTexture;
uniform bool flipY;

const float nearPlane = 0.01;
const float farPlane = 100.0;

// Output fragment color
out vec4 finalColor;
  
float LinearizeDepth(float depth) 
{
    // return (2.0 * nearPlane ) / (farPlane + nearPlane - depth * (farPlane - nearPlane));	
    float far = farPlane;
    float near = nearPlane;
    float zNDC = 2 * depth - 1;
    float zEye = (2* far * near)/ ((far + near) - zNDC * (far - near));
    float linearDepth = (zEye - near) / (far - near);
    return linearDepth;
}
void main()
{
    // Handle potential Y-flipping
    vec2 texCoord = fragTexCoord;
    if (flipY) texCoord.y = 1.0 - texCoord.y;

    // Sample depth
    float depth = texture(depthTexture, texCoord).r;

    // Linearize depth value
    // float linearDepth = (2.0*nearPlane)/(farPlane + nearPlane - depth*(farPlane - nearPlane));
    float linearDepth = LinearizeDepth(depth);
    // Output final color
    finalColor = vec4(vec3(linearDepth), 1.0);
}