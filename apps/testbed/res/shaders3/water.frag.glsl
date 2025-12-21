#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec4 worldSpaceCoord;
in vec3 worldPosition;

out vec4 finalColor;


uniform vec4 colDiffuse;      // FS: base water color
uniform sampler2D texture0;   // FS: dudv map
uniform sampler2D texture1;   // FS: reflection texture
uniform sampler2D texture2;   // FS: refraction texture

const float waveStrength = 0.02f; //how strong the distortion is

uniform vec3  viewPosition;
uniform float time;
uniform float waveMoveSpeed;
uniform vec2  waveMoveDirection1 = vec2(1.0, 1.0);
uniform vec2  waveMoveDirection2 = vec2(-0.3, 1.5);


void main()
{
    vec2 UV = fragTexCoord;
    vec2 ndc = (worldSpaceCoord.xy/worldSpaceCoord.w)/2.0 + vec2(0.5);
    vec2 reflectUV = vec2(ndc.x, -ndc.y);
    vec2 refractUV = vec2(ndc.x,  ndc.y);


    vec2 distort1 = texture(texture0, UV + waveMoveDirection1 * waveMoveSpeed).rg * 2.0 - 1.0;
    vec2 distort2 = texture(texture0, UV + waveMoveDirection2 * waveMoveSpeed).rg * 2.0 - 1.0;
    distort1 *= waveStrength;
    distort2 *= waveStrength;

    vec2 totalDistortion = (distort1 + distort2);

    reflectUV += totalDistortion;
    reflectUV.x = clamp(reflectUV.x, 0.001, 0.999);
    reflectUV.y = clamp(reflectUV.y, -0.999, -0.001);
    
    refractUV += -totalDistortion; 
    refractUV = clamp(refractUV, 0.001, 0.999);


    vec4 reflectColor = texture(texture1, reflectUV);
    vec4 refractColor = texture(texture2, refractUV);

    vec3 toCamera = normalize(worldPosition - viewPosition);

    float fresnel = dot(toCamera, vec3(0,1,0));

    // vec4 texelColor = texture(texture0, UV);
    // finalColor = texelColor*colDiffuse*fragColor;
    finalColor = mix(reflectColor, refractColor, fresnel);
    finalColor = mix(finalColor, colDiffuse, 0.24);
}