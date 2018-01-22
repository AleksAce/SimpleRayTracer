#pragma once

#include "include/GL/glew.h"
#include "include/SDL.h"
#include "include/SDL_opengl.h"
#include "include/SDL_main.h"

#include "include/glm/gtc/type_ptr.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <float.h>

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb-master/stb_image.h"

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define PI 3.14159265359f

const int WindowWidth = 1920;
const int WindowHeight = 1080;

struct texture
{
	int Width;
	int Height;
	void *PixelData;
	GLuint TextureHandle;
};
struct material
{
	glm::vec4 Color;
	glm::vec4 ReflectionColor;
	// 1 is full specular
	float specular_value;
};
struct object_sphere
{
	//plane
	glm::vec3 Position;
	float Radius;
	material Material;
};
struct object_plane
{
	glm::vec3 Normal;
	float Origin;
	material Material;
};
struct scene
{
	
	int MaterialCount;
	material *Materials;

	int PlaneCount;
	object_plane* Planes;

	int SpheresCount;
	object_sphere* Sphere;
};

inline glm::vec3 Lerp(glm::vec3 A, float value, glm::vec3 B)
{
	return (1.0f - value)*A + value*B;
}
inline glm::vec3 Hadamard(glm::vec3 A, glm::vec3 B)
{
	return glm::vec3(A.x*B.x, A.y*B.y, A.z*B.z);
}
inline glm::vec4 Hadamard(glm::vec4 A, glm::vec4 B)
{
	return glm::vec4(A.x*B.x, A.y*B.y, A.z*B.z, A.a*B.a);
}
//Gamma Correction Source values = WIki
static float LinearTosRGB(float LinearValue)
{
	
	float sRGBValue;
	if (LinearValue < 0.0f)
	{
		LinearValue = 0.0f;
	}
	else if (LinearValue > 1.0f)
	{
		LinearValue = 1.0f;
	}

	if (LinearValue <= 0.031308f)
	{
		sRGBValue = LinearValue*12.92f;
	}
	else
	{
		sRGBValue = 1.055f*pow(LinearValue, 1.0f / 2.4f) - 0.055f;
	}
	return sRGBValue;
}