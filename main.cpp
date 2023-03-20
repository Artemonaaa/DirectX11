#include <iostream>
#include <format>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/vec3.hpp>

auto main() -> int {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Could not initialize GLFW\n";
	}

	glfwSetErrorCallback([](int ErrorCode, const char* Description) {
		std::cout << std::format("GLFW error {}: {}\n", ErrorCode, Description);
	});

	GLFWwindow* Window = glfwCreateWindow(640, 480, "My 2D game", NULL, NULL);
	if (Window == nullptr) {
		std::cout << "Could not create the window\n";
		glfwTerminate();
		return 0;
	}

	while (glfwWindowShouldClose(Window) == GLFW_FALSE) {
		glfwPollEvents();
	}

	glfwDestroyWindow(Window);
	glfwTerminate();
}
