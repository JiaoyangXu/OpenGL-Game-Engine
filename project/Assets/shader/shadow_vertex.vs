#version 330 core

in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 ModelView;

void main()
{
    gl_Position = lightSpaceMatrix * ModelView * vec4(position, 1.0);
}  