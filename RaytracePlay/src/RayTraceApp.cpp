#include "RayTraceApp.h"
#include "Gadgets.h"

bool RayTrace_App::StartUp()
{
	if (!Application::StartUp())
		return false;

	Gadgets::Initailise();
	Gadgets::SetLineWidth(5.0f);
	camera = new FlyCamera(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0));
	
	return true;
}

void RayTrace_App::ShutDown()
{
	Gadgets::Destroy();

	Application::ShutDown();
}

bool RayTrace_App::Update()
{
	float delta_time = (float)glfwGetTime();
	glfwSetTime(0);
	Gadgets::Clear();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->Update(delta_time);

	using namespace glm;
	//create floor gadget
	for (float i = -10; i <= 10; i++)
	{
		Gadgets::AddLine(vec4(i, 0, -10, 1), vec4(i, 0, 10, 1), vec4(0.8f, 0.8f, 0.8f, 1.0f));
		Gadgets::AddLine(vec4(-10, 0, i, 1), vec4(10, 0, i, 1), vec4(0.8f, 0.8f, 0.8f, 1.0f));
	}

	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(10, 0, 0, 1), vec4(1, 0, 0, 1), vec4(1, 0, 0, 1));
	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(0, 10, 0, 1), vec4(0, 1, 0, 1), vec4(0, 1, 0, 1));
	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(0, 0, 10, 1), vec4(0, 0, 1, 1), vec4(0, 0, 1, 1));

	Gadgets::AddCube(vec4(0,0, 20, 1), vec3(1,1,1), mat4(), vec4(1,0,0,1), vec4(1,1,1,1));

	return (Application::Update());
}

void RayTrace_App::Draw()
{
	Gadgets::Draw(camera->getProjectionView());
	Application::Draw();
}