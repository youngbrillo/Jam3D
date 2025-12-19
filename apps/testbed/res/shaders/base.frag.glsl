#version 330
in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;


uniform vec4 colDiffuse;      // FS: Diffuse color
uniform sampler2D texture0;   // FS: GL_TEXTURE0
uniform sampler2D texture1;   // FS: GL_TEXTURE1
uniform sampler2D texture2;   // FS: GL_TEXTURE2

void main()
{                                 
    vec4 texelColor = texture(texture0, fragTexCoord);
    finalColor = texelColor*colDiffuse*fragColor;
}