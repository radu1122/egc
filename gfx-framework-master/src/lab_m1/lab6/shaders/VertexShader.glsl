#version 330

//Input
// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_coordinate;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// TODO: output values to fragment shader
out vec3 frag_color;
out vec2 frag_coord;
out vec3 frag_normal;
out vec3 frag_pos;

void main()
{
	// TODO: send output to fragment shader
	frag_color = v_color;
	frag_pos = v_position;
	frag_normal = v_normal;
	frag_coord = v_coordinate;

	// TODO: compute gl_Position
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
