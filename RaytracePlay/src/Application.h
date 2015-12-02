#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

#include "GLMheader.h"
#include "Camera.h"


class Application
{
public:
	Application();

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update();
	virtual void Draw();

protected:
	Camera *camera;
	GLFWwindow* window;

};


#endif

