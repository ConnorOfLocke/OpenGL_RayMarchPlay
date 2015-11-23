#ifndef _RAYTRACE_APP_H
#define _RAYTRACE_APP_H

#include "Application.h"

class RayTrace_App : Application
{
public:
	bool StartUp();
	void ShutDown();

	bool Update();
	void Draw();
private:

	void CreateScreenQuad();
	void CreateScreenQuadShaders(const char* v_shader_file_name, const char* f_shader_file_name);
	void RenderScreenQuad();

	unsigned int LoadShader(const char* shader_file_name, const unsigned int& shader_type);

	float time_since_start = 0.0f;
	unsigned int ray_trace_shader;

	unsigned int screen_quad_vao;
	unsigned int screen_quad_vbo;

};

#endif