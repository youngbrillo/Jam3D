#version 330
// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct Light {
    int type;// = LIGHT_DIRECTIONAL;
    vec3 position;// = vec3(0.0, 10.0, 0.0);
    vec3 target;// = vec3(-1.0, -1.0, -1.0);
    vec4 color; //= vec4(1.0, 1.0, 1.0, 1.0);
};

// Input uniform values
uniform vec4 colDiffuse;      // FS: Diffuse color
uniform sampler2D texture0;   // FS: GL_TEXTURE0
uniform sampler2D texture1;   // FS: GL_TEXTURE1
uniform sampler2D texture2;   // FS: GL_TEXTURE2

uniform vec3 albedo = vec3(0,0.321569, 0.431373);
uniform vec3 albedo2 = vec3(97.3, 51.4, 47.5);//vec3(0, 0.47451, 0.764706);
uniform vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec3 viewPosition;
uniform float time;

// Input lighting values
uniform Light mainLight = Light(
    LIGHT_DIRECTIONAL,
    vec3(10.0, 10.0, 10.0),
    vec3(0.0, 0.0, 0.0),
    vec4(1.0, 1.0, 1.0, 1.0)
);

uniform vec2 waveDirection1 = vec2(2.0, 1.0);
uniform vec2 waveDirection2 = vec2(0.0, 1.0);
uniform float timeScale;
uniform float uvScale = 0.1f;

varying vec3 worldPosition;

float calcFresnel(float amount, vec3 normal, vec3 view)
{
	return pow((1.0 - clamp(dot(normalize(normal), normalize(view)), 0.0, 1.0 )), amount);
}

vec3 calcLight(Light _light, inout vec3 lightDot, vec3 fpos, vec3 viewD, vec3 normal, vec3 specular )
{
    vec3 light = vec3(0.0);

    if (_light.type == LIGHT_DIRECTIONAL)
    {
        light = -normalize(_light.target - _light.position);
    }
    if (_light.type == LIGHT_POINT)
    {
        light = normalize(_light.position - fpos);
    }

    float NdotL = max(dot(normal, light), 0.0);
    lightDot += _light.color.rgb*NdotL;
    float specCo = 0.0;
    if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine
    vec3 sp = specular + specCo;

    return sp;
}


void main()
{                             
    vec3 ALBEDO = albedo;    
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    //water shading
    vec2 waveTime1 = (time * waveDirection1) * timeScale;
    vec2 waveTime2 = (time * waveDirection2) * timeScale;
    vec3 normalBlend = mix(
            texture(texture0, (worldPosition.xz * uvScale) + waveTime1).rgb, 
            texture(texture2, (worldPosition.xz * uvScale) + waveTime2).rgb, 0.5);
    //calculate fresnel
    float freshnel = calcFresnel(50.0,normalize(fragNormal), viewDirection);
    vec3 surfaceColor = mix(albedo, albedo2, freshnel);
    vec3 lightDot = vec3(0.0);
    // vec3 normal = normalize(fragNormal);
    // vec3 normal = texture(texture2, fragTexCoord).rgb;
    vec3 normal = normalBlend;



    vec3 specular = vec3(0.0);

    specular = calcLight(mainLight, lightDot, fragPosition, viewDirection, normal, specular);



    ALBEDO = surfaceColor;

    vec4 tint = vec4(ALBEDO, 1.0);
    finalColor = tint + (vec4(specular, 1.0f)) * vec4(lightDot, 1.0);
    finalColor += (ambient/10.0f) * tint;
    // finalColor = vec4(albedo, 1.0);
    // finalColor = normalColor;
}