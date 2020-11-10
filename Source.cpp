//#define PGE_APPLICATION
//#include "GameEngine.hpp"

#include <tuple>

import stdex;
import matrix;
import vector;
import core;


constexpr auto screenWidth = 2560.0f;
constexpr auto screenHeight = 1440.0f;

constexpr auto fov = 90.0f;
constexpr auto aspectRatio = (screenHeight / screenWidth);
constexpr auto near = 0.1f;
constexpr auto far = 10.0f;

int main(void)
{	
	auto matProj = cla::mat::projection(fov, aspectRatio, near, far);

	return 0;
}