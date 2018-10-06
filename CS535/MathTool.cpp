#include "MathTool.h"

bool FloatEqual(float a, float b)
{
	auto res = std::abs(a - b) < 1e-5;
	return res;
};

float Deg2Rad(float deg)
{
	return deg / 180.0f * PI;
}

float Rad2Deg(float rad)
{
	return rad / PI * 180.0f;
}

float Fract(float n)
{
	return n - static_cast<int>(n);
}