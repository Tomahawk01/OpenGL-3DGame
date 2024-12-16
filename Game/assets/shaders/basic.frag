#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
out vec4 frag_color;

uniform sampler2D tex0;
uniform sampler2D tex1;

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
	vec3 eye;
};

layout(std140, binding = 1) uniform lights
{
    vec3 ambient;
	vec3 direction;
	vec3 direction_color;
	vec3 point;
	vec3 point_color;
	vec3 attenuation;
};

vec3 calc_ambient()
{
	return ambient;
}

vec3 calc_direction()
{
	vec3 light_dir = normalize(-direction);
	float diff = max(dot(normal, light_dir), 0.0);
	return diff * direction_color;
}

vec3 calc_point()
{
	float distance = length(point - frag_position.xyz);
	float att = 1.0 / (attenuation.x + (attenuation.y * distance) + (attenuation.z * (distance * distance)));

	vec3 light_dir = normalize(point - frag_position.xyz);
	float diff = max(dot(normal, light_dir), 0.0);

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(normalize(eye - frag_position.xyz), reflect_dir), 0.0), 32) * texture(tex1, tex_coord).r;

	return ((diff + spec) * att) * point_color;
}

void main()
{
	vec4 albedo = texture(tex0, tex_coord);
	vec3 amb_color = calc_ambient();
	vec3 dir_color = calc_direction();
	vec3 point_color = calc_point();

	frag_color = vec4((amb_color + dir_color + point_color) * albedo.rgb, 1.0);
}
