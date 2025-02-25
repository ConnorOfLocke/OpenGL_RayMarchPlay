#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GLMheader.h"
#include <GLFW/glfw3.h>

/// <summary>
/// Class to contain transforms and functionality of a 3d perspective camera
/// </summary>
class Camera
{
public:		
	Camera(vec3 position = vec3(0,0,-10), vec3 lookAt = vec3(0,0,0),
		float fov = 3.1415926f * 0.25f, float aspectRatio = (16.0f / 9.0f), float a_near = 1.0f, float a_far = 100.0f);
	
	virtual void Update(float dt) = 0;
	void SetPerspective(float fov, float aspectRatio, float a_near, float a_far);
	void SetLookAt(vec3 from, vec3 to, vec3 up);
	void SetPosition(vec3 position);

	mat4 GetWorld();
	mat4 GetView();
	mat4 GetProjection();
	mat4 GetProjectionView();

protected:
	mat4 world;
	mat4 view;
	mat4 projection;
	mat4 projection_view;
};

/// <summary>
/// Camera with added functionality of keyboard and mouse movement
/// </summary>
class FlyCamera : public Camera
{
public:
	FlyCamera(	vec3 position = vec3(0, 10, -10),
				vec3 lookAt = vec3(0, 0, 0),
				float fov = 3.1415926f * 0.25f,
				float aspectRatio = (16.0f / 9.0f),
				float a_near = 0.1f,
				float a_far = 100.0f,
				float a_speed = 5.0f);

	void SetSpeed(float a_speed);
	void Update(float dt);

private:
	float speed;
	vec3 up;
};

#endif
