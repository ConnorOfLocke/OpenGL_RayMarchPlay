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

	float time_since_start = 0.0f;

};

#endif