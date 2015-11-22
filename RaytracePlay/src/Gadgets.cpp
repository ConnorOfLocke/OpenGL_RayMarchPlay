#include "Gadgets.h"
#include "gl_core_4_4.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Gadgets* Gadgets::gadget_instance = nullptr;

Gadgets::Gadgets()
{

}

Gadgets::~Gadgets()
{

}

void Gadgets::Initailise(const int& max_tris, const int& max_lines)
{
	gadget_instance = new Gadgets();
	gadget_instance->CreateShaders();

	gadget_instance->line_width = 1.0f;

	//create the buffers needed
	gadget_instance->gadget_tris = new GadgetTri[max_tris];
	gadget_instance->gadget_lines = new GadgetLine[max_lines];

	gadget_instance->max_tris = max_tris;
	gadget_instance->max_lines = max_lines;

	gadget_instance->cur_tris = 0;
	gadget_instance->cur_lines = 0;

	//bind and create the tri buffer
	glGenVertexArrays(1, &gadget_instance->tri_VAO);
	glBindVertexArray(gadget_instance->tri_VAO);

	glGenBuffers(1, &gadget_instance->tri_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, gadget_instance->tri_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GadgetTri) * max_tris,
						(void*)gadget_instance->gadget_tris, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //colour

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GadgetVert), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GadgetVert), (void*)(sizeof(float) * 4));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind the line buffer
	glGenVertexArrays(1, &(gadget_instance->line_VAO));
	glBindVertexArray(gadget_instance->line_VAO);
	glGenBuffers(1, &gadget_instance->line_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, gadget_instance->line_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GadgetLine) * max_tris,
		(void*)(gadget_instance->gadget_lines), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //colour

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GadgetVert), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GadgetVert), (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Gadgets::CreateShaders()
{
	const char* v_shader_text =
			"#version 410 \n \
			layout(location = 0) in vec4 position; \
			layout(location = 1) in vec4 colour; \
			uniform mat4 projection_view; \
			out vec4 frag_colour; \
			\
			void main() \
			{ \
				vec4 new_position = projection_view * position; \
				frag_colour = colour; \
				gl_Position = new_position; \
			}";
	int v_shader_text_len = strlen(v_shader_text);

	const char* f_shader_text =
			"#version 410 \n \
			in vec4 frag_colour; \
			out vec4 final_colour; \
			void main() \
			{ \
				final_colour = frag_colour; \
			}";
	int f_shader_text_len = strlen(f_shader_text);

	unsigned int vertex_handle = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);

	//Create the vertex shader
	glShaderSource(vertex_handle, 1, &v_shader_text, 0);
	glCompileShader(vertex_handle);

#if _DEBUG
	int check;
	glGetShaderiv(vertex_handle, GL_COMPILE_STATUS, &check);
	if (check == GL_FALSE)
	{
		int LogLength = 0;
		glGetShaderiv(vertex_handle, GL_INFO_LOG_LENGTH, &LogLength);

		char* Log = new char[LogLength];
		glGetShaderInfoLog(vertex_handle, LogLength, 0, Log);

		printf("Gadget vertex shader failed! \n");
		printf("\n %s \n", Log);

		delete[] Log;
	}
	else
		printf("Gadget v_shader loaded \n");
#endif

	//Create the fragment
	glShaderSource(fragment_handle, 1, &f_shader_text, 0);
	glCompileShader(fragment_handle);

#if _DEBUG
	glGetShaderiv(fragment_handle, GL_COMPILE_STATUS, &check);
	if (check == GL_FALSE)
	{
		int LogLength = 0;
		glGetShaderiv(fragment_handle, GL_INFO_LOG_LENGTH, &LogLength);

		char* Log = new char[LogLength];
		glGetShaderInfoLog(fragment_handle, LogLength, 0, Log);

		printf("Gadget fragment shader failed! \n");
		printf("\n %s \n", Log);

		delete[] Log;
	}
	else
		printf("Gadget f_shader loaded \n");
#endif

	unsigned int new_shader = glCreateProgram();

	//link em 
	glAttachShader(new_shader, vertex_handle);
	glAttachShader(new_shader, fragment_handle);

	glLinkProgram(new_shader);

#if _DEBUG
	glGetProgramiv(new_shader, GL_LINK_STATUS, &check);
	if (check == GL_FALSE)
	{
		int LogLength = 0;
		glGetProgramiv(check, GL_INFO_LOG_LENGTH, &LogLength);

		char* Log = new char[LogLength];
		glGetProgramInfoLog(flat_shader, LogLength, 0, Log);

		printf("Link Shaders failed! \n");
		printf("%s \n", Log);

		delete[] Log;
	}
	else
		printf("Gadget shader created! :D \n");
#endif

	gadget_instance->flat_shader = new_shader;

	//delete em
	//glDeleteShader(vertex_handle);
	//glDeleteShader(fragment_handle);
}

void Gadgets::Destroy()
{
	delete[] gadget_instance->gadget_tris;
	delete[] gadget_instance->gadget_lines;
}

void Gadgets::Clear()
{
	gadget_instance->cur_tris = 0;
	gadget_instance->cur_lines = 0;
}

void Gadgets::Draw(const glm::mat4& projection_view)
{
	glUseProgram(gadget_instance->flat_shader);
	glEnable(GL_CULL_FACE);

	unsigned int view_proj_un = glGetUniformLocation(gadget_instance->flat_shader, "projection_view");
	glUniformMatrix4fv(view_proj_un, 1, false, (float*)&projection_view);

	if (gadget_instance->cur_lines > 0)
	{
		float prev_line_width;
		glGetFloatv(GL_LINE_WIDTH, &prev_line_width);

		glLineWidth(gadget_instance->line_width);

		glBindVertexArray(gadget_instance->line_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, gadget_instance->line_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			sizeof(GadgetLine) * gadget_instance->cur_lines, (void*)gadget_instance->gadget_lines);

		glDrawArrays(GL_LINES, 0, gadget_instance->cur_lines * 2);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(prev_line_width);

	}

	if (gadget_instance->cur_tris > 0)
	{
		glBindVertexArray(gadget_instance->tri_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, gadget_instance->tri_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			sizeof(GadgetTri) * gadget_instance->cur_tris, (void*)gadget_instance->gadget_tris);

		glDrawArrays(GL_TRIANGLES, 0, gadget_instance->cur_tris * 3);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glDisable(GL_CULL_FACE);
}

void Gadgets::AddLine(const glm::vec4& from, const glm::vec4& to, const glm::vec4& from_colour, const glm::vec4& to_colour)
{
	gadget_instance->gadget_lines[gadget_instance->cur_lines++] = GadgetLine(GadgetVert(from, from_colour), GadgetVert(to, to_colour));
}

void Gadgets::AddLine(const glm::vec4& from, const glm::vec4& to, const glm::vec4& colour)
{
	gadget_instance->gadget_lines[gadget_instance->cur_lines++] = GadgetLine(GadgetVert(from, colour), GadgetVert(to, colour));
}

void Gadgets::AddTri(const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c,
	const glm::vec4& a_colour, const glm::vec4& b_colour, const glm::vec4& c_colour)
{
	gadget_instance->gadget_tris[gadget_instance->cur_tris++] = GadgetTri(GadgetVert(a_a, a_colour), GadgetVert(a_b, b_colour), GadgetVert(a_c, c_colour));
}

void Gadgets::AddTri(const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c)
{
	AddTri(a_a, a_b, a_c, vec4(1, 1, 1, 1));
}

void Gadgets::AddTri(const glm::vec4& a_a, const glm::vec4& a_b, const glm::vec4& a_c, const glm::vec4& colour)
{
	gadget_instance->gadget_tris[gadget_instance->cur_tris++] = GadgetTri(GadgetVert(a_a, colour), GadgetVert(a_b, colour), GadgetVert(a_c, colour));
}

void Gadgets::AddLineCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform, const glm::vec4& colour)
{
	vec4 top_front_left		= transform * center_pos + vec4(-extents.x * 0.5f,	  extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 top_front_right	= transform * center_pos + vec4( extents.x * 0.5f,	  extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 top_back_left		= transform * center_pos + vec4(-extents.x * 0.5f,	  extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 top_back_right		= transform * center_pos + vec4( extents.x * 0.5f,	  extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 bottom_front_left	= transform * center_pos + vec4(-extents.x * 0.5f,	 -extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 bottom_front_right	= transform * center_pos + vec4( extents.x * 0.5f,	 -extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 bottom_back_left	= transform * center_pos + vec4(-extents.x * 0.5f,	 -extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 bottom_back_right	= transform * center_pos + vec4( extents.x * 0.5f,	 -extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);

	//top face
	AddLine(top_front_left, top_front_right, colour);
	AddLine(top_front_right, top_back_right, colour);
	AddLine(top_back_right, top_back_left, colour);
	AddLine(top_back_left, top_front_left, colour);

	//bottom face
	AddLine(bottom_front_left,	bottom_front_right, colour);
	AddLine(bottom_front_right, bottom_back_right, colour);
	AddLine(bottom_back_right,	bottom_back_left, colour);
	AddLine(bottom_back_left,	bottom_front_left, colour);

	//sides
	AddLine(bottom_front_left,	top_front_left,	 colour);
	AddLine(bottom_front_right, top_front_right,  colour);
	AddLine(bottom_back_right,	top_back_right,	colour);
	AddLine(bottom_back_left,	top_back_left,	colour);
}

void Gadgets::AddFilledCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform, const glm::vec4& colour)
{
	using namespace glm;

	vec4 top_front_left		= transform * center_pos + vec4(-extents.x * 0.5f,	  extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 top_front_right	= transform * center_pos + vec4( extents.x * 0.5f,	  extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 top_back_left		= transform * center_pos + vec4(-extents.x * 0.5f,	  extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 top_back_right		= transform * center_pos + vec4( extents.x * 0.5f,	  extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 bottom_front_left	= transform * center_pos + vec4(-extents.x * 0.5f,	 -extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 bottom_front_right	= transform * center_pos + vec4( extents.x * 0.5f,	 -extents.y * 0.5f,	extents.z * 0.5f,	 1.0f);
	vec4 bottom_back_left	= transform * center_pos + vec4(-extents.x * 0.5f,	 -extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);
	vec4 bottom_back_right	= transform * center_pos + vec4( extents.x * 0.5f,	 -extents.y * 0.5f,	-extents.z * 0.5f,	 1.0f);

	//top face
	AddTri(top_back_left, top_front_left, top_front_right, colour);
	AddTri(top_back_left, top_front_right, top_back_right, colour);

	//bottom face
	AddTri(bottom_back_left, bottom_front_right, bottom_front_left, colour);
	AddTri(bottom_back_left, bottom_back_right, bottom_front_right, colour);

	//left face 
	AddTri(top_back_left, bottom_back_left, bottom_front_left, colour);
	AddTri(top_back_left, bottom_front_left, top_front_left, colour);

	//right face 
	AddTri(top_front_right, bottom_front_right, bottom_back_right, colour);
	AddTri(top_front_right, bottom_back_right, top_back_right, colour);

	//front face
	AddTri(top_front_left, bottom_front_left, bottom_front_right, colour);
	AddTri(top_front_left, bottom_front_right, top_front_right, colour);

	//back face
	AddTri(top_back_right, bottom_back_right, bottom_back_left, colour);
	AddTri(top_back_right, bottom_back_left, top_back_left, colour);
}

void Gadgets::AddCube(const glm::vec4& center_pos, const glm::vec3& extents, const glm::mat4& transform, const glm::vec4& fill_colour, const glm::vec4& line_colour)
{
	AddFilledCube(center_pos, extents, transform, fill_colour);
	AddLineCube(center_pos, extents, transform, line_colour);
}

void Gadgets::SetLineWidth(const float& a_line_width)
{
	gadget_instance->line_width = a_line_width;
}

