#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
in mat3 tbn;

out vec4 frag_color;

uniform sampler2D tex0;
uniform float gamma;

void main()
{
	vec3 hdr_color = texture(tex0, tex_coord).rgb;

	vec3 mapped = hdr_color / (hdr_color + vec3(1.0));

	mapped = pow(mapped, vec3(1.0 / gamma));
	frag_color = vec4(mapped, 1.0);
}
