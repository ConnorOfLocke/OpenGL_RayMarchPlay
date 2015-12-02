#include "RayTraceApp.h"
#include "Gadgets.h"
#include "Audio.h"

bool RayTrace_App::StartUp()
{
	if (!Application::StartUp())
		return false;

	Gadgets::Initailise();
	Gadgets::SetLineWidth(2.0f);
	camera = new FlyCamera(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0));
	
	CreateScreenQuad();
	CreateScreenQuadShaders("ray_trace_vert.glsl", "ray_trace_frag.glsl");

	Audio::Initailise();
	Audio::AddAudioListener(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0));
	int source = Audio::AddAudioSource(1.0f, 1.0f, vec3(0, 0, 1), vec3(0, 0, 0), true);
	int clip = Audio::AddAudioClip_wav("test.wav");

	Audio::AttachAudioClipToAudioSource(source, clip);

	return true;
}

void RayTrace_App::ShutDown()
{
	Gadgets::Destroy();
	Audio::Destroy();

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

	//fnow
	glDeleteProgram(ray_trace_shader);
	CreateScreenQuadShaders("ray_trace_vert.glsl", "ray_trace_frag.glsl");

	using namespace glm;
	RenderScreenQuad();
	
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
	Gadgets::Draw(camera->GetProjectionView());
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

void RayTrace_App::CreateScreenQuadShaders(const char* v_shader_file_name, const char* f_shader_file_name)
{
	unsigned int v_shader, f_shader;

	ray_trace_shader = glCreateProgram();

	v_shader = LoadShader(v_shader_file_name, GL_VERTEX_SHADER);
	f_shader = LoadShader(f_shader_file_name, GL_FRAGMENT_SHADER);

	glAttachShader(ray_trace_shader, v_shader);
	glAttachShader(ray_trace_shader, f_shader);
	
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	glLinkProgram(ray_trace_shader);

#if _DEBUG
	int check = GL_FALSE;
	glGetProgramiv(ray_trace_shader, GL_LINK_STATUS, &check);
	if (check == GL_FALSE)
	{
		int LogLength = 0;
		glGetProgramiv(ray_trace_shader, GL_INFO_LOG_LENGTH, &LogLength);

		char* Log = new char[LogLength];
		glGetProgramInfoLog(ray_trace_shader, LogLength, 0, Log);

		printf("Link Shaders failed! \n");
		printf("%s \n", Log);

		delete[] Log;
	}
#endif


}


unsigned int RayTrace_App::LoadShader(const char* shader_file_name, const unsigned int& shader_type)
{
	unsigned int return_shader;

	FILE* shader_file = fopen(shader_file_name, "rb");

	if (shader_file == 0)
	{
		printf("Cannot open shader file: ");
		printf(shader_file_name);
		printf("\n");
		fclose(shader_file);
		return -1;
	}

	//get file length
	fseek(shader_file, 0, SEEK_END);
	int shader_file_len = ftell(shader_file);
	fseek(shader_file, 0, SEEK_SET);

	//allocate space and update the file length
	char* s_source = new char[shader_file_len];
	shader_file_len = fread(s_source, 1, shader_file_len, shader_file);

	return_shader = glCreateShader(shader_type);
	glShaderSource(return_shader, 1, &s_source, &shader_file_len);
	glCompileShader(return_shader);

#if _DEBUG
	int check = GL_FALSE;
	glGetShaderiv(return_shader, GL_COMPILE_STATUS, &check);
	if (check == GL_FALSE)
	{
		int loglength;
		glGetShaderiv(return_shader, GL_INFO_LOG_LENGTH, &loglength);

		char* log = new char[loglength];
		glGetShaderInfoLog(return_shader, loglength, 0, log);

		printf(shader_file_name);
		printf(": Shader Load failed! \n");
		printf(log);
		printf("\n");

		delete[] log;
		delete[] s_source;
		fclose(shader_file);
		return -1;
	}
#endif
	delete[] s_source;
	fclose(shader_file);

	return return_shader;
}

void RayTrace_App::RenderScreenQuad()
{
	glUseProgram(ray_trace_shader);
	glEnable(GL_CULL_FACE);

	unsigned int world_un = glGetUniformLocation(ray_trace_shader, "world");
	unsigned int view_un = glGetUniformLocation(ray_trace_shader, "view");
	unsigned int projection_un = glGetUniformLocation(ray_trace_shader, "projection");
	unsigned int aspect_un = glGetUniformLocation(ray_trace_shader, "aspect");
	unsigned int screen_un = glGetUniformLocation(ray_trace_shader, "screen_size");
	
	glUniformMatrix4fv(world_un, 1, GL_FALSE, (float*)(&camera->GetWorld()));
	glUniformMatrix4fv(view_un, 1, GL_FALSE, (float*)(&camera->GetView()));
	glUniformMatrix4fv(projection_un, 1, GL_FALSE, (float*)(&camera->GetProjection()));
	glUniform1f(aspect_un, 1280.0f / 720.0f);
	glUniform2f(screen_un, 1280.0f, 720.0f);
	//uniform bollocks

	glBindVertexArray(screen_quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(float) * 36);

	glDisable(GL_CULL_FACE);
}

