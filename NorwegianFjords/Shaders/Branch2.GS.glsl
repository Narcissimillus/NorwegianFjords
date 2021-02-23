#version 430
layout(lines) in;
layout(triangle_strip, max_vertices = 78) out;

uniform mat4 View;
uniform mat4 Projection;

uniform vec3 control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1;
uniform vec3 control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2;
uniform vec3 control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3;
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
	int index = 0;

	// Branch 2
	textureNo = 1;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		w_position = bezier(i / no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(0, 0);
		gl_Position = Projection * View * vec4(bezier(i/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), 1); EmitVertex();
		w_position = translateX(bezier(i / no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(1, 0);
		gl_Position = Projection * View * vec4(translateX(bezier(i/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness), 1); EmitVertex();
		w_position = bezier((i + 1) / no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(0, 1);
		gl_Position = Projection * View * vec4(bezier((i+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), 1); EmitVertex();
		w_position = translateX(bezier((i + 1) / no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
		w_normal = vec3(0, 1, 0);
		texture_coord = vec2(1, 1);
		gl_Position = Projection * View * vec4(translateX(bezier((i+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness), 1); EmitVertex();
	}
	EndPrimitive();


	// Mountain between brach 1 and 2
	textureNo = 2;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		vec3 p0 = translateX(bezier(i/no_of_generated_points, control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1), thickness);
		vec3 p1 = translateX(bezier((i+1)/no_of_generated_points, control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1), thickness);
		for(float j = 0; j < no_of_generated_points; j++)
		{
			vec3 p2 = bezier(j/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
			vec3 p3 = bezier((j+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
			vec3 startPoint = get_line_intersection(p0.x, p0.z, p1.x, p1.z, p2.x, p2.z, p3.x, p3.z, 0, 0);

			// We have an intersection
			if(startPoint.x != 0 || startPoint.z != 0)
			{
				// Right Branch 2's Mountain
				texture_coord = vec2(1, 1);
				gl_Position = Projection * View * vec4(startPoint, 1); EmitVertex();
				texture_coord = vec2(1, 0);
				gl_Position = Projection * View * vec4((p1.x + p3.x)/2, 0.25f, (p1.z + p3.z)/2, 1); EmitVertex();
				texture_coord = vec2(0, 0);
				gl_Position = Projection * View * vec4(p3.x, 0, p3.z, 1); EmitVertex();
				float height = 0.25f;
				float k = j + 1, l = i + 1;
				while(k != no_of_generated_points && l != no_of_generated_points)
				{
					vec3 m0 = translateX(bezier(l/no_of_generated_points, control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1), thickness);
					vec3 m1 = translateX(bezier((l+1)/no_of_generated_points, control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1), thickness);
					vec3 m2 = bezier(k/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
					vec3 m3 = bezier((k+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2);
					texture_coord = vec2(0, 0);
					gl_Position = Projection * View * vec4(m2, 1); EmitVertex();
					texture_coord = vec2(0, 1);
					gl_Position = Projection * View * vec4((m0.x + m2.x)/2, height, (m0.z + m2.z)/2, 1); EmitVertex();
					texture_coord = vec2(1, 0);
					gl_Position = Projection * View * vec4(m3, 1); EmitVertex();
					texture_coord = vec2(1, 1);
					if(height < 3f)
					{
						height += 0.25f;
					}
					gl_Position = Projection * View * vec4((m1.x + m3.x)/2, height, (m1.z + m3.z)/2, 1); EmitVertex();
					k++;
					l++;
				}
				EndPrimitive();
			}
		}
	}

	// Mountain between branch 2 and 3
	textureNo = 2;
	for(float i = 0; i < no_of_generated_points; i++)
	{
		vec3 p0 = translateX(bezier(i/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
		vec3 p1 = translateX(bezier((i+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
		for(float j = 0; j < no_of_generated_points; j++)
		{
			vec3 p2 = bezier(j/no_of_generated_points, control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3);
			vec3 p3 = bezier((j+1)/no_of_generated_points, control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3);
			vec3 startPoint = get_line_intersection(p0.x, p0.z, p1.x, p1.z, p2.x, p2.z, p3.x, p3.z, 0, 0);

			// We have an intersection
			if(startPoint.x != 0 || startPoint.z != 0)
			{
				// Left Branch 2's Mountain 
				texture_coord = vec2(1, 1);
				gl_Position = Projection * View * vec4(startPoint, 1); EmitVertex();
				texture_coord = vec2(1, 0);
				gl_Position = Projection * View * vec4((p1.x + p3.x)/2, 0.25f, (p1.z + p3.z)/2, 1); EmitVertex();
				texture_coord = vec2(0, 0);
				gl_Position = Projection * View * vec4(p1.x, 0, p1.z, 1); EmitVertex();
				float height = 0.25f;
				float k = j + 1, l = i + 1;
				while(k != no_of_generated_points && l != no_of_generated_points)
				{
					vec3 m0 = translateX(bezier(l/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
					vec3 m1 = translateX(bezier((l+1)/no_of_generated_points, control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2), thickness);
					vec3 m2 = bezier(k/no_of_generated_points, control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3);
					vec3 m3 = bezier((k+1)/no_of_generated_points, control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3);
					texture_coord = vec2(0, 0);
					gl_Position = Projection * View * vec4(m0, 1); EmitVertex();
					texture_coord = vec2(1, 0);
					gl_Position = Projection * View * vec4((m0.x + m2.x)/2, height, (m0.z + m2.z)/2, 1); EmitVertex();
					texture_coord = vec2(0, 1);
					gl_Position = Projection * View * vec4(m1, 1); EmitVertex();
					texture_coord = vec2(1, 1);
					if(height < 3f)
					{
						height += 0.25f;
					}
					gl_Position = Projection * View * vec4((m1.x + m3.x)/2, height, (m1.z + m3.z)/2, 1); EmitVertex();
					k++;
					l++;
				}
				EndPrimitive();
			}
		}
	}
}
