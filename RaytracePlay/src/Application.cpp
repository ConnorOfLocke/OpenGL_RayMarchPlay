#include "Application.h"

Application::Application()
{

}

bool Application::StartUp()
{
	if (!glfwInit())
		return false;
	
	this->window = glfwCreateWindow(1280, 720,
		"RayTracePlay", nullptr, nullptr);
	if (this->window == nullptr)
		return false;

	glfwMakeContextCurrent(this->window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(this->window);
		return false;
	}
	printf("Loaded OpenGL version: %d.%d \n", ogl_GetMajorVersion(), ogl_GetMinorVersion());

	//camera = new FlyCamera();
	return true;
}

void Application::ShutDown()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	delete camera;
}

bool Application::Update()
{
	return (!glfwWindowShouldClose(window));
}

void Application::Draw()
{
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}