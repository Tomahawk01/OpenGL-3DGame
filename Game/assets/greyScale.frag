#version 460 core

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D tex0;

void main()
{
	vec3 input_color = texture(tex0, tex_coord).rgb;
    float grey = dot(input_color, vec3(0.299, 0.587, 0.114));

	frag_color = vec4(grey, grey, grey, 1.0);
}
