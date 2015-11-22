#include "Camera.h"

/// <summary>
/// Initializes a new instance of the <see cref="Camera"/> class.
/// </summary>
/// <param name="position">Position in 3d Space</param>
/// <param name="lookAt">Position the camera is initially pointed towards</param>
/// <param name="fov">Field of Vision</param>
/// <param name="aspectRatio">The aspect ratio.</param>
/// <param name="a_near">Near Plane</param>
/// <param name="a_far">Far Plane</param>
Camera::Camera(	vec3 position,
				vec3 lookAt,
				float fov,
				float aspectRatio,
				float a_near,
				float a_far)
{
	projection = glm::perspective(fov, aspectRatio, a_near, a_far);
	view = glm::lookAt(position, lookAt, vec3(0, 1, 0));

	projection_view = projection * view;
	world = glm::inverse(view);
}

/// <summary>
/// Sets the perspective matrix for the camera.
/// </summary>
/// <param name="fov">The fov.</param>
/// <param name="aspectRatio">The aspect ratio.</param>
/// <param name="a_near">The a_near.</param>
/// <param name="a_far">The a_far.</param>
void Camera::setPerspective(float fov, float aspectRatio, float a_near, float a_far)
{
	projection = glm::perspective(fov, aspectRatio, a_near, a_far);
	projection_view = projection * view;
}

/// <summary>
/// Sets position to look at other position
/// </summary>
/// <param name="from">From.</param>
/// <param name="to">To.</param>
/// <param name="up">Up.</param>
void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	view = glm::lookAt(from, to, up);
	world = glm::inverse(view);

	projection_view = projection * view;
}

/// <summary>
/// Sets the position of the camera in 3d space
/// </summary>
/// <param name="position">The position.</param>
void Camera::setPosition(vec3 position)
{
	world[3].x = position.x;
	world[3].y = position.y;
	world[3].z = position.z;

	view = glm::inverse(world);
}

/// <summary>
/// Gets the inverse view transform
/// </summary>
/// <returns></returns>
mat4 Camera::getWorldTransform()
{
	world = glm::inverse(view);
	return world;
}
/// <summary>
/// Gets the current view matrix.
/// </summary>
/// <returns></returns>
mat4 Camera::getView()
{
	view = glm::inverse(world);
	return view;
}

/// <summary>
/// Gets the current projection matrix.
/// </summary>
/// <returns></returns>
mat4 Camera::getProjection()
{
	return projection;
}

/// <summary>
/// Gets the current projection view matrix.
/// </summary>
/// <returns></returns>
mat4 Camera::getProjectionView()
{
	projection_view = projection * view;
	return projection_view;
}

/// <summary>
/// Initializes an new improved instance with movement from mouse and keyboard
/// </summary>
/// <param name="position">Position in 3d Space</param>
/// <param name="lookAt">Position the camera is initially pointed towards</param>
/// <param name="fov">Field of Vision</param>
/// <param name="aspectRatio">The aspect ratio.</param>
/// <param name="a_near">Near Plane</param>
/// <param name="a_far">Far Plane</param>
/// <param name="a_speed">Speed in units per second</param>
FlyCamera::FlyCamera(vec3 position,
	vec3 lookAt,
	float fov,
	float aspectRatio,
	float a_near,
	float a_far,
	float a_speed)
{
	projection = glm::perspective(fov, aspectRatio, a_near, a_far);
	view = glm::lookAt(position, lookAt, vec3(0, 1, 0));

	projection_view = projection * view;
	world = glm::inverse(view);

	speed = a_speed;
}

/// <summary>
/// Sets the speed in units per second.
/// </summary>
/// <param name="a_speed">Speed in units per second</param>
void FlyCamera::SetSpeed(float a_speed)
{
	speed = a_speed;
}

/// <summary>
/// Moves the camera according to key presses and mouse movement
/// </summary>
/// <param name="dt">Delta time.</param>
void FlyCamera::Update(float dt)
{
	GLFWwindow* curWindow = glfwGetCurrentContext();
	float Sensitivity = 0.5f;

	float adjusted_speed = speed;
	if (glfwGetKey(curWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		adjusted_speed *= 2.0f;

	if (glfwGetKey(curWindow, GLFW_KEY_A) == GLFW_PRESS)
		world[3] -= world[0] * adjusted_speed * dt;

	if (glfwGetKey(curWindow, GLFW_KEY_D) == GLFW_PRESS)
		world[3] += world[0] * adjusted_speed * dt;

	if (glfwGetKey(curWindow, GLFW_KEY_Q) == GLFW_PRESS)
		world[3] += world[1] * adjusted_speed * dt;

	if (glfwGetKey(curWindow, GLFW_KEY_E) == GLFW_PRESS)
		world[3] -= world[1] * adjusted_speed * dt;

	if (glfwGetKey(curWindow, GLFW_KEY_W) == GLFW_PRESS)
		world[3] -= world[2] * adjusted_speed * dt;

	if (glfwGetKey(curWindow, GLFW_KEY_S) == GLFW_PRESS)
		world[3] += world[2] * adjusted_speed * dt;

	if (glfwGetMouseButton(curWindow, 1))
	{
		double x_delta, y_delta;

		glfwGetCursorPos(curWindow, &x_delta, &y_delta);

		x_delta -= (1280.0f / 2.0f);
		y_delta -= (720.0f / 2.0f);

		x_delta /= (1280.0f / 2.0f);
		y_delta /= (720.0f / 2.0f);

		x_delta *= -Sensitivity;
		y_delta *= -Sensitivity;

		glfwSetCursorPos(curWindow, 1280/2.0f, 720/2.0f);

		vec3 camera_right = (vec3)world[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		world[0] = rot * world[0];
		world[1] = rot * world[1];
		world[2] = rot * world[2];
	}
	view = glm::inverse(world);

	//view = glm::lookAt((vec3)(world[3].xyz), vec3(0,0,0), vec3(0, 1, 0));

	projection_view = projection * view;
}