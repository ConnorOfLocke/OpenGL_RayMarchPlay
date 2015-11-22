#ifndef _GADGETS_H_
#define _GADGETS_H_

#include <glm\fwd.hpp>
#include <glm\ext.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;

class Gadgets
{
public:
	static void Initailise(	const int& max_tris = 32767,
							const int& max_lines = 32767);

	static void Destroy();
	static void Clear();

	static void Draw(const glm::mat4& projection_view);

	static void AddLine(const glm::vec4& from, const glm::vec4& to, const glm::vec4& from_colour, const glm::vec4& to_colour);
	static void AddLine(const glm::vec4& from, const glm::vec4& to, const glm::vec4& colour = vec4(1,1,1,1));

	static void AddTri(	const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c,
						const glm::vec4& a_colour, const glm::vec4& b_colour, const glm::vec4& c_colour);
	static void AddTri(const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c, const glm::vec4& colour);
	static void AddTri(const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c);


	static void AddLineCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform = glm::mat4(), const glm::vec4& colour = glm::vec4(1,1,1,1));
	static void AddFilledCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform = glm::mat4(), const glm::vec4& colour = glm::vec4(1, 1, 1, 1));
	static void AddCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform = glm::mat4(), const glm::vec4& fill_colour = glm::vec4(1, 1, 1, 1), const glm::vec4& line_colour = glm::vec4(1, 1, 1, 1));

	static void SetLineWidth(const float& a_line_width);

private:
	Gadgets();
	~Gadgets();

	void CreateShaders();

	static Gadgets* gadget_instance;

	int max_tris;
	int max_lines;
	int cur_tris;
	int cur_lines;

	float line_width;
	
	struct GadgetVert
	{
		float x, y, z, w;
		float r, g, b, a;
		GadgetVert()
		{
			x = y = z = w = r = g = b = a = 0.0f;
		}
		GadgetVert(glm::vec4 pos, glm::vec4 colour)
		{
			x = pos.x;
			y = pos.y;
			z = pos.z;
			w = pos.w;
			r = colour.x;
			g = colour.y;
			b = colour.z;
			a = colour.w;
		}
	};

	struct GadgetTri
	{
		GadgetVert a, b, c;
		GadgetTri()
		{}
		GadgetTri(const GadgetVert& a_a, const GadgetVert& a_b, const GadgetVert& a_c)
			: a(a_a), b(a_b), c(a_c)
		{}
	};

	struct GadgetLine
	{
		GadgetVert from, to;

		GadgetLine()
		{}
		GadgetLine(const GadgetVert& a_from, const GadgetVert& a_to)
			: from(a_from), to(a_to)
		{}

	};

	GadgetTri* gadget_tris;
	GadgetLine* gadget_lines;

	unsigned int tri_VAO;
	unsigned int tri_VBO;

	unsigned int line_VAO;
	unsigned int line_VBO;

	unsigned int flat_shader;
};


//controller object

//shapes with transforms attached
//

//draw spheres
//draw filled spheres
//draw cubes
//draw filled cubes
//draw lines
//draw gradient lines

//draw torus
//draw torus filled


#endif