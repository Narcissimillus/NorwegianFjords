#version 430
layout(lines) in;
layout(triangle_strip, max_vertices = 78) out;

uniform mat4 View;
uniform mat4 Projection;

uniform vec3 control_p1, control_p2, control_p3, control_p4;
uniform int no_of_generated_points;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec3 w_position;
layout(location = 2) out vec3 w_normal;
flat out int textureNo;


vec3 rotateY(vec3 point, float u)
{
	float x = point.x * cos(u) - point.z *sin(u);
	float z = point.x * sin(u) + point.z *cos(u);
	return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
	return vec3(point.x + t, point.y, point.z);
}

vec3 translateY(vec3 point, float t)
{
	return vec3(point.x, point.y + t, point.z);
}

vec3 bezier(float t, vec3 control_p1, vec3 control_p2, vec3 control_p3, vec3 control_p4)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

vec3 get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float i_x, float i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        i_x = p0_x + (t * s1_x);
        i_y = p0_y + (t * s1_y);
        return vec3(i_x, 0, i_y);
    }

    return vec3(0, 0, 0); // No collision
}

void main()
{
	float thickness = 1.5f;
	
	// Main River
	textureNo = 1;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		w_position = bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(0, 0);
		gl_Position = Projection * View * vec4(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 1); EmitVertex();
		w_position = translateX(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(1, 0);
		gl_Position = Projection * View * vec4(translateX(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness), 1); EmitVertex();
		w_position = bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(0, 1);
		gl_Position = Projection * View * vec4(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 1); EmitVertex();
		w_position = translateX(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(1, 1);
		gl_Position = Projection * View * vec4(translateX(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness), 1); EmitVertex();
	}
	EndPrimitive();
	
	// Right Main River's Mountain
	textureNo = 2;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		texture_coord = vec2(0, 0);
		gl_Position = Projection * View * vec4(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 1); EmitVertex();
		texture_coord = vec2(1, 0);
		gl_Position = Projection * View * vec4(translateY(translateX(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), -1.5f * thickness), 2f), 1); EmitVertex();
		texture_coord = vec2(0, 1);
		gl_Position = Projection * View * vec4(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 1); EmitVertex();
		texture_coord = vec2(1, 1);
		gl_Position = Projection * View * vec4(translateY(translateX(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), -1.5f * thickness), 2f), 1); EmitVertex();
	}
	EndPrimitive();

	// Left Main River's Mountain
	textureNo = 2;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		texture_coord = vec2(0, 0);
		gl_Position = Projection * View * vec4(translateX(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness), 1); EmitVertex();
		texture_coord = vec2(1, 0);
		gl_Position = Projection * View * vec4(translateY(translateX(bezier(i/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 2.5f * thickness), 2f), 1); EmitVertex();
		texture_coord = vec2(0, 1);
		gl_Position = Projection * View * vec4(translateX(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), thickness), 1); EmitVertex();
		texture_coord = vec2(1, 1);
		gl_Position = Projection * View * vec4(translateY(translateX(bezier((i+1)/no_of_generated_points, control_p1, control_p2, control_p3, control_p4), 2.5f * thickness), 2f), 1); EmitVertex();
	}
	EndPrimitive();
}
