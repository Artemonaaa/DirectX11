#include <format>

#include <spdlog/spdlog.h>

#include "Window.h"

namespace Engine {
	auto InitWindow() -> void {
		if (glfwInit() == GLFW_FALSE) {
			spdlog::critical("Could not initialize GLFW");
		}

		glfwSetErrorCallback([](int ErrorCode, const char* Description) {
			spdlog::error("GLFW error {}: {}\n", ErrorCode, Description);
		});

		GWindow = glfwCreateWindow(1000, 800, "My 2D game", NULL, NULL);
		if (GWindow == nullptr) {
			glfwTerminate();
			spdlog::critical("Could not create the window");
		}
	}

	auto DeinitWindow() -> void {
		glfwDestroyWindow(GWindow);
		glfwTerminate();
	}
}