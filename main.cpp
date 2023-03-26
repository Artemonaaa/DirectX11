#include <iostream>
#include <format>
#include <stdexcept>
#include <array>
#include <comdef.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

namespace WRL = Microsoft::WRL;

WRL::ComPtr<ID3D11Device> GDevice;
WRL::ComPtr<ID3D11DeviceContext> GContext;

WRL::ComPtr<IDXGISwapChain> GSwapChain;
WRL::ComPtr<ID3D11RenderTargetView> GSwapChainRenderTargetView;

auto CheckHResult(HRESULT HResult) -> void {
	if (!SUCCEEDED(HResult)) {
		auto Error = _com_error(HResult);
		LPCTSTR ErrorMessage = Error.ErrorMessage();
		throw std::runtime_error(ErrorMessage);
	}
}

auto CompileShader(std::wstring FilePath, const char* EntryPoint, const char* Target) -> WRL::ComPtr<ID3DBlob> {
	WRL::ComPtr<ID3DBlob> ByteCode;
	WRL::ComPtr<ID3DBlob> ErrorMsgs;

	HRESULT HResult = D3DCompileFromFile(FilePath.c_str(), nullptr, nullptr, EntryPoint, Target, NULL, NULL, &ByteCode, &ErrorMsgs);
	if (!SUCCEEDED(HResult)) {
		if (ErrorMsgs != nullptr) {
			throw std::runtime_error(static_cast<const char*>(ErrorMsgs->GetBufferPointer()));
		} 
		else {
			CheckHResult(HResult);
		}
	}
	return ByteCode;
}
		
struct TVertex {
	glm::vec2 Position;
	glm::vec3 Color;
};

auto Main() -> void {
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
		return;
	}

	HWND WindowHandle = glfwGetWin32Window(Window);

	auto SwapChainDescription = DXGI_SWAP_CHAIN_DESC{
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
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&SwapChainDescription,
		&GSwapChain,
		&GDevice,
		nullptr,
		&GContext
	));

	WRL::ComPtr<ID3D11Resource> BackBuffer;
	CheckHResult(GSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &BackBuffer));

	CheckHResult(GDevice->CreateRenderTargetView(BackBuffer.Get(), nullptr, &GSwapChainRenderTargetView));

	std::array Vertices = {
		TVertex { .Position = glm::vec2(+0.0F, +0.5F), .Color = glm::vec3(1.0f, 0.0f, 0.0f), },
		TVertex { .Position = glm::vec2(+0.5F, -0.5F), .Color = glm::vec3(0.0f, 1.0f, 0.0f), },
		TVertex { .Position = glm::vec2(-0.5F, -0.5F), .Color = glm::vec3(0.0f, 0.0f, 1.0f), },
	};

	WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	{
		auto VertexBufferDescription = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(Vertices),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.StructureByteStride = sizeof(Vertices[0]),
		};

		auto VertexBufferSubresourceData = D3D11_SUBRESOURCE_DATA{
			.pSysMem = Vertices.data(),
		};

		CheckHResult(GDevice->CreateBuffer(&VertexBufferDescription, &VertexBufferSubresourceData, &VertexBuffer));
	}

	WRL::ComPtr<ID3DBlob> VertexShaderByteCode = CompileShader(L"../../../Shaders/Triangle.hlsl", "MainVS", "vs_5_0");
	WRL::ComPtr<ID3DBlob> PixelShaderByteCode = CompileShader(L"../../../Shaders/Triangle.hlsl", "MainPS", "ps_5_0");

	WRL::ComPtr<ID3D11VertexShader> VertexShader;
	CheckHResult(GDevice->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &VertexShader));

	WRL::ComPtr<ID3D11PixelShader> PixelShader;
	CheckHResult(GDevice->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &PixelShader));

	glm::ivec2 WindowSize;
	glfwGetWindowSize(Window, &WindowSize.x, &WindowSize.y);

	auto Viewport = D3D11_VIEWPORT {
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = static_cast<float>(WindowSize.x),
		.Height = static_cast<float>(WindowSize.y),
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};

	std::array InputElementDescs = {
		D3D11_INPUT_ELEMENT_DESC {
			.SemanticName = "POSITION",
			.SemanticIndex = 0U,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0U,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		},
		D3D11_INPUT_ELEMENT_DESC {
			.SemanticName = "COLOR",
			.SemanticIndex = 0U,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0U,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		},
	};

	WRL::ComPtr<ID3D11InputLayout> InputLayout;
	CheckHResult(GDevice->CreateInputLayout(InputElementDescs.data(), InputElementDescs.size(), VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &InputLayout))	;
		
	while (glfwWindowShouldClose(Window) == GLFW_FALSE) {
		glfwPollEvents();

		float Color[4] = { 0.0F, 1.0F, 0.0F, 1.0F };
		GContext->ClearRenderTargetView(GSwapChainRenderTargetView.Get(), Color);

		UINT Stride = sizeof(TVertex);
		UINT Offset = 0U;

		GContext->IASetVertexBuffers(0U, 1U, VertexBuffer.GetAddressOf(), &Stride, &Offset);

		GContext->VSSetShader(VertexShader.Get(), nullptr, 0);
		GContext->PSSetShader(PixelShader.Get(), nullptr, 0);

		GContext->OMSetRenderTargets(1U, GSwapChainRenderTargetView.GetAddressOf(), nullptr);

		GContext->RSSetViewports(1, &Viewport);

		GContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		GContext->IASetInputLayout(InputLayout.Get());

		GContext->Draw(3U, 0U);	

		GSwapChain->Present(1, 0);
	}

	glfwDestroyWindow(Window);
	glfwTerminate();
}

auto main() -> int {
	try {
		Main();
	}
	catch (const std::exception& Ex) {
		spdlog::error("{}", Ex.what());
	}
}
