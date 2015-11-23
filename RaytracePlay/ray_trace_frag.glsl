#version 410

out vec4 final_colour;

in vec2 frag_tex_coord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

uniform float aspect;
uniform vec2 screen_size;

vec4 init_circle_pos = vec4(0,0,10,1);
float circle_radius = 0.1f;


void main()
{
	mat4 view_proj = view * projection;
	vec4 eye = vec4(0,0,0,1);

	//puts the tex coord in [-1, 1]

	
	vec2 inv_pixel = 1 / screen_size;
	inv_pixel /= 8;

	for (float x = 0 ; x < 8 ; ++x)
	{
		for (float y = 0 ; y < 8 ; ++y)
		{
			float fx = x - 4;
			float fy = x - 4;

			vec4 pixel_pos = vec4(
			 ((frag_tex_coord.x + fx * inv_pixel.x) * 2.0f - 1.0f) * aspect,
			  (frag_tex_coord.y + fy * inv_pixel.y) * 2.0f - 1.0f, 0, 1);
			
			
			
			vec4 ray = normalize(pixel_pos - vec4(0.0f, 0.0f, -1.0f, 1.0f));

			//intersection with the sphere
			vec4 sphere_pos = init_circle_pos;

			vec4 center_to_orgin = normalize(eye - sphere_pos);

			float discriminant = dot(center_to_orgin, ray) * dot(center_to_orgin, ray) - 
								  (dot(center_to_orgin, center_to_orgin) - circle_radius * circle_radius);

			if (discriminant > 0.0) final_colour += vec4(1.f/64.f,1.f/64.f,1.f/64.f,0);
		}
	}
	//final_colour = ray;
}
