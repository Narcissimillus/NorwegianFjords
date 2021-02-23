#version 430
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec3 w_position;
layout(location = 2) in vec3 w_normal;

flat in int textureNo;

uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec4 myReflect()
{
    // compute the reflection color value
	vec3 R = reflect(normalize(w_position - camera_position), normalize(w_normal));
	return texture(texture_cubemap, R);
}

void main()
{
	if(textureNo == 1)
	{
		// Combine reflection and object texture
		vec4 color = texture2D(texture_1, texture_coord);
		out_color = mix(color, myReflect(), 0.5f);
	}
	else
		out_color = texture2D(texture_2, texture_coord);
}