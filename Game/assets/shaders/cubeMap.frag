#version 460 core

in vec3 tex_coord;

out vec4 frag_color;

uniform samplerCube tex0;

void main()
{
    frag_color = texture(tex0, tex_coord);
}
