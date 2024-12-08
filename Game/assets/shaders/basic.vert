#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 tex_coord;

uniform mat4 model;

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    tex_coord = uv;
}
