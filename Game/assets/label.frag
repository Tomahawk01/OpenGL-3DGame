#version 460 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D tex0;

void main()
{
	frag_color = texture(tex0, tex_coord);
}
