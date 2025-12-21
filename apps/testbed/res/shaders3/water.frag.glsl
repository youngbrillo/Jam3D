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
uniform sampler2D texture3;  // FS: normal map
uniform sampler2D texture4;  // FS: depth buffer



uniform vec3  viewPosition;
uniform float time;
uniform float waveMoveSpeed;
uniform vec2  waveMoveDirection1 = vec2(1.0, 1.0);
uniform vec2  waveMoveDirection2 = vec2(-0.3, 1.5);

uniform vec3 lightDirection = vec3(-1.0, 1.0,-1.0);
uniform vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

const float waveStrength = 0.02f; //how strong the distortion is
const float shinedamper = 4; //how much the water is shiny
const float reflectivityValue = 1.0;

const float nearPlane = 0.01;
const float farPlane = 100.0;
const float intersectionThreshold = 0.01f;

float LinearizeDepth(float depth) 
{    
    // return (2.0 * nearPlane ) / (farPlane + nearPlane - depth * (farPlane - nearPlane));	
    // return 2.0 * nearPlane * farPlane/ (farPlane + nearPlane - (2.0 * depth - 1.0) *(farPlane - nearPlane));
    float far = farPlane;
    float near = nearPlane;
    float zNDC = 2 * depth - 1;
    float zEye = (2* far * near)/ ((far + near) - zNDC * (far - near));
    float linearDepth = (zEye - near) / (far - near);
    return linearDepth;
}

void main()
{
    vec2 UV = fragTexCoord;
    vec2 ndc = (worldSpaceCoord.xy/worldSpaceCoord.w)/2.0 + vec2(0.5);
    vec2 reflectUV = vec2(ndc.x, -ndc.y);
    vec2 refractUV = vec2(ndc.x,  ndc.y);

    // float depth = texture(texture4, refractUV).r;
    // float floorDistance = LinearizeDepth(depth);
    // float waterDistance = LinearizeDepth(gl_FragCoord.z);
    // float waterDepth =  floorDistance - waterDistance;
    // float depthDifference = abs(waterDepth);

    float sceneDepth = texture(texture4, refractUV).r;   // linear depth from texture;
    float edgeDepth  = LinearizeDepth(gl_FragCoord.z);  //linaralized bubble depth
    float depthDifference = abs(edgeDepth - sceneDepth);
    // vec4 interectionColor = (depthDifference <= intersectionThreshold) ? vec4(1) : vec4(0);
    float normalizedEdgeDiff = clamp(depthDifference / intersectionThreshold, 0.0, 1.0); //[0, threshold] -> [0,1]
    vec4 interectionColor = mix(vec4(1), vec4(0), normalizedEdgeDiff); //white to transparent -> no hard edges!

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

    vec4 normalMapColor = texture(texture3, totalDistortion);
    vec3 normal = vec3(
        normalMapColor.r * 2.0 - 1.0,
        normalMapColor.b ,
        normalMapColor.g * 2.0 - 1.0
    );
    normal = normalize(normal); 


    vec3 reflectedLight = reflect(normalize(lightDirection), normal); //reflect the light from the water surface
    float specular = max(dot(reflectedLight, toCamera), 0.0);
    specular = pow(specular, shinedamper);
    vec3 specularHighlights = lightColor.rgb * specular * reflectivityValue;


    finalColor = mix(reflectColor, refractColor, fresnel);
    finalColor = mix(finalColor, colDiffuse, 0.24);
    finalColor += vec4(specularHighlights, 0.0);
    // finalColor += mix(finalColor, texture(texture4, fragTexCoord), 0.5f);
    // finalColor += vec4(vec3(waterDepth), 0);

    // if (floorDistance > waterDistance){
    //     finalColor = finalColor;// * vec4(vec3(waterDistance - floorDistance), 1);
    // }
    // else{
    //     // finalColor = texture(texture4, ndc);
    //     // finalColor = vec4(vec3(abs(waterDepth)/2.0), 1);
    //     finalColor += interectionColor;
    // }
    finalColor += vec4(vec3(interectionColor), 0);
}