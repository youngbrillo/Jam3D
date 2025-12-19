#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

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

// Input lighting values
uniform Light mainLight = Light(
    LIGHT_DIRECTIONAL,
    vec3(10.0, 10.0, 10.0),
    vec3(0.0, 0.0, 0.0),
    vec4(1.0, 1.0, 1.0, 1.0)
);
uniform vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec3 viewPosition;

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
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPosition - fragPosition);
    vec3 specular = vec3(0.0);

    vec4 tint = colDiffuse * fragColor;
    // NOTE: Implement here your fragment shader code

    specular = calcLight(mainLight, lightDot, fragPosition, viewD, normal, specular);

    finalColor = (texelColor*((tint + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
    finalColor += texelColor*(ambient/10.0)*tint;

    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}
