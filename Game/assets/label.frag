#version 460 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D tex0;

void main()
{
	vec4 albedo = texture(tex0, tex_coord);
	frag_color = vec4(albedo.rgb, 1.0);
}
