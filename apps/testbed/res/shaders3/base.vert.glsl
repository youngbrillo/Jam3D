#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 mvp;             // VS: ModelViewProjection matrix
uniform mat4 matView;         // VS: View matrix
uniform mat4 matProjection;   // VS: Projection matrix
uniform mat4 matModel;        // VS: Model matrix
uniform mat4 matNormal;       // VS: Normal matrix

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}                                  