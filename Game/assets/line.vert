#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out vec3 color;

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
    vec3 eye;
};

void main()
{
    gl_Position = projection * view * vec4(in_position, 1.0);
    color = in_color;
}
