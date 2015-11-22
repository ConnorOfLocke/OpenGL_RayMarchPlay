#include <iostream>

#include "RayTraceApp.h"
#include "Camera.h"

int main()
{
	RayTrace_App app;


	if (!app.StartUp())
	{
		app.ShutDown();
		return -1;
	}

	while (app.Update())
		app.Draw();

	app.ShutDown();
	return 0;
}