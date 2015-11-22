#include "RayTraceApp.h"
#include "Gadgets.h"

bool RayTrace_App::StartUp()
{
	if (!Application::StartUp())
		return false;

	Gadgets::Initailise();
	Gadgets::SetLineWidth(2.0f);
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
	time_since_start += delta_time;

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

	mat4 tester = glm::translate(vec3(sinf(time_since_start), 0, cosf(time_since_start)));

	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(10, 0, 0, 1), vec4(1, 0, 0, 1), vec4(1, 0, 0, 1));
	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(0, 10, 0, 1), vec4(0, 1, 0, 1), vec4(0, 1, 0, 1));
	Gadgets::AddLine(vec4(0, 0, 0, 1), vec4(0, 0, 10, 1), vec4(0, 0, 1, 1), vec4(0, 0, 1, 1));

	Gadgets::AddCube(vec4(0, 0, 20, 1), vec3(1, 1, 1), tester, vec4(1, 0, 0, 1), vec4(1, 1, 1, 1));

	Gadgets::AddSphere(vec4(0, 0, -20, 1), 2.0f, 10, 10, tester, vec4(1.0f, 1.0f, 0.0f, 1.0f));

	return (Application::Update());
}

void RayTrace_App::Draw()
{
	Gadgets::Draw(camera->getProjectionView());
	Application::Draw();
}

void RayTrace_App::CreateScreenQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 270) * 0.5f;

	float vertex_data[] = 
	{
		-1, -1, 0, 1,	half_texel.x,			half_texel.y,
		1,	1, 0, 1,	1 - half_texel.x,		1 - half_texel.y,
		-1, 1, 0, 1,	half_texel.x,			1 - half_texel.y,

		-1, -1, 0, 1,	half_texel.x,			half_texel.y,
		1, -1, 0, 1,	1 - half_texel.x,		half_texel.y,
		1, 1, 0, 1,		1 - half_texel.x,		1 - half_texel.y,
	};

	glGenVertexArrays(1, &screen_quad_vao);
	glBindVertexArray(screen_quad_vao);

	glGenBuffers(1, &screen_quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, screen_quad_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex_coords

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
