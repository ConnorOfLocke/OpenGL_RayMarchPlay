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

	float time_since_start = 0.0f;
	unsigned int ray_trace_shader;

	unsigned int screen_quad_vao;
	unsigned int screen_quad_vbo;



	

};

#endif