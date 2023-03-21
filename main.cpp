#include <iostream>
#include <format>
#include <stdexcept>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <spdlog/spdlog.h>
#include <glm/vec3.hpp>
#include <d3d11.h>
#include <wrl.h>

namespace WRL = Microsoft::WRL;

WRL::ComPtr<ID3D11Device> GDevice = nullptr;
WRL::ComPtr<ID3D11DeviceContext> GContext = nullptr;

WRL::ComPtr<IDXGISwapChain> GSwapChain = nullptr;
WRL::ComPtr<ID3D11RenderTargetView> GSwapChainRenderTargetView = nullptr;

auto CheckHResult(HRESULT HResult) -> void {
	if (!SUCCEEDED(HResult)) {
		throw std::runtime_error("Invalid HResult");
	}
}

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

	HWND WindowHandle = glfwGetWin32Window(Window);

	auto SwapChainDescription = DXGI_SWAP_CHAIN_DESC {
		.BufferDesc = DXGI_MODE_DESC {
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		},
		.SampleDesc = DXGI_SAMPLE_DESC {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,
		.OutputWindow = WindowHandle,
		.Windowed = true, 
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
	};

	CheckHResult(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&SwapChainDescription,
		&GSwapChain,
		&GDevice,
		nullptr,
		&GContext
	));

	ID3D11Resource* BackBuffer = nullptr;
	CheckHResult(GSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&BackBuffer)));
	assert(BackBuffer != nullptr);
	CheckHResult(GDevice->CreateRenderTargetView(BackBuffer, nullptr, &GSwapChainRenderTargetView));

	while (glfwWindowShouldClose(Window) == GLFW_FALSE) {
		glfwPollEvents();
		float Color[4] = { 0.0F, 1.0F, 0.0F, 1.0F };
		GContext->ClearRenderTargetView(GSwapChainRenderTargetView.Get(), Color);
		GSwapChain->Present(1, 0);
	}

	GSwapChain = nullptr;
	GDevice = nullptr;
	GContext = nullptr;

	glfwDestroyWindow(Window);
	glfwTerminate();
}
