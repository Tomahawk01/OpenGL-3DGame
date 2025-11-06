#version 460 core

in vec2 tex_coord;

layout(location = 0) out vec4 frag_color;

uniform sampler2D tex0; // scene
uniform sampler2D tex1; // ssao

void main()
{
	vec4 color = texture(tex0, tex_coord);
	vec4 occlusion = texture(tex1, tex_coord);

	frag_color = color * occlusion;
}
