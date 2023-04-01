#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine {
	inline GLFWwindow* GWindow;

	auto InitWindow() -> void;
	auto DeinitWindow() -> void;
}