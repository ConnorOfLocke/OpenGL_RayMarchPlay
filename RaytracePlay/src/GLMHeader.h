#ifndef _GLM_HEADER_H_
#define _GLM_HEADER_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Plane
{
	vec3 direction;
	float distance;
};

#endif