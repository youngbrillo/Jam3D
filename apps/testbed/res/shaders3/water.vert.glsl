#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec4 worldSpaceCoord;
out vec3 worldPosition;

uniform mat4 mvp;             // VS: ModelViewProjection matrix
uniform mat4 matView;         // VS: View matrix
uniform mat4 matProjection;   // VS: Projection matrix
uniform mat4 matModel;        // VS: Model matrix
uniform mat4 matNormal;       // VS: Normal matrix

const float tiling = 10.0;

void main()
{

    fragTexCoord = vertexTexCoord * tiling;
    fragColor = vertexColor;
    worldSpaceCoord = mvp * vec4(vertexPosition, 1.0);
    worldPosition = vec3(matModel * vec4(vertexPosition, 1.0));

    gl_Position = worldSpaceCoord;
}                                  