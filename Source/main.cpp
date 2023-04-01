#include <iostream>
#include <format>
#include <stdexcept>
#include <array>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_glfw.h>

#include "Window.h"
#include "Graphics/Graphics.h"
#include "Graphics/Vertices.h"
#include "Graphics/ConstantBuffers.h"
#include "HResult.h"

namespace Engine {
	auto InitImGui(GLFWwindow* GWindow) -> void {
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGuiIO& IO = ImGui::GetIO();
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();
		IO.Fonts->AddFontFromFileTTF("../../../Fonts/RobotoMono-Regular.ttf", 14, nullptr, IO.Fonts->GetGlyphRangesCyrillic());

		ImGui::GetStyle().WindowRounding = 7.0f;
		ImGui::GetStyle().ChildRounding = 7.0f;
		ImGui::GetStyle().FrameRounding = 7.0f;
		ImGui::GetStyle().GrabRounding = 7.0f;
		ImGui::GetStyle().PopupRounding = 7.0f;
		ImGui::GetStyle().ScrollbarRounding = 7.0f;

		ImGui_ImplGlfw_InitForOther(GWindow, true);
		ImGui_ImplDX11_Init(GDevice.Get(), GContext.Get());
	}

	auto Main() -> void {
		InitWindow();
		
		InitGraphics(glfwGetWin32Window(GWindow));

		InitImGui(GWindow);

		std::array Vertices = {
			TVertex { .Position = glm::vec2(-0.5F, -0.5F), .Color = glm::vec3(1.0f, 0.0f, 0.0f), },
			TVertex { .Position = glm::vec2(-0.5F, +0.5F), .Color = glm::vec3(1.0f, 1.0f, 0.0f), },
			TVertex { .Position = glm::vec2(+0.5F, +0.5F), .Color = glm::vec3(0.0f, 1.0f, 0.0f), },
			TVertex { .Position = glm::vec2(+0.5F, -0.5F), .Color = glm::vec3(0.0f, 0.0f, 1.0f), },
		};
		WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		{
			auto VertexBufferDescription = D3D11_BUFFER_DESC {
				.ByteWidth = sizeof(Vertices),
				.Usage = D3D11_USAGE_DEFAULT,
				.BindFlags = D3D11_BIND_VERTEX_BUFFER,
				.StructureByteStride = sizeof(Vertices[0]),
			};

			auto VertexBufferSubresourceData = D3D11_SUBRESOURCE_DATA {
				.pSysMem = Vertices.data(),
			};

			CheckHResult(GDevice->CreateBuffer(&VertexBufferDescription, &VertexBufferSubresourceData, &VertexBuffer));
		}

		std::array Indices = {
			0U, 1U, 2U,
			0U, 2U, 3U,
		};
		WRL::ComPtr<ID3D11Buffer> IndexBuffer;
		{
			auto IndexBufferDescription = D3D11_BUFFER_DESC {
				.ByteWidth = sizeof(Indices),
				.Usage = D3D11_USAGE_DEFAULT,
				.BindFlags = D3D11_BIND_INDEX_BUFFER,
				.StructureByteStride = sizeof(Indices[0]),
			};

			auto IndexBufferSubresourceData = D3D11_SUBRESOURCE_DATA {
				.pSysMem = Indices.data(),
			};

			CheckHResult(GDevice->CreateBuffer(&IndexBufferDescription, &IndexBufferSubresourceData, &IndexBuffer));
		}

		WRL::ComPtr<ID3DBlob> VertexShaderByteCode = CompileShader("../../../Shaders/Triangle.hlsl", "MainVS", "vs_5_0");
		WRL::ComPtr<ID3DBlob> PixelShaderByteCode = CompileShader("../../../Shaders/Triangle.hlsl", "MainPS", "ps_5_0");

		WRL::ComPtr<ID3D11VertexShader> VertexShader;
		CheckHResult(GDevice->CreateVertexShader(VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), nullptr, &VertexShader));

		WRL::ComPtr<ID3D11PixelShader> PixelShader;
		CheckHResult(GDevice->CreatePixelShader(PixelShaderByteCode->GetBufferPointer(), PixelShaderByteCode->GetBufferSize(), nullptr, &PixelShader));

		glm::ivec2 WindowSize;
		glfwGetWindowSize(GWindow, &WindowSize.x, &WindowSize.y);

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
		CheckHResult(GDevice->CreateInputLayout(InputElementDescs.data(), InputElementDescs.size(), VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &InputLayout));

		auto RasterizerDescription = D3D11_RASTERIZER_DESC {
			.FillMode = D3D11_FILL_SOLID,
			.CullMode = D3D11_CULL_NONE
		};

		WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
		CheckHResult(GDevice->CreateRasterizerState(&RasterizerDescription, &RasterizerState));

		auto CPUCommonCB = TCommonCB {
			.Color = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F),
		};

		int Sorry = 63;

		WRL::ComPtr<ID3D11Buffer> CommonCB;
		{
			auto ConstantBufferDescription = D3D11_BUFFER_DESC {
				.ByteWidth = sizeof(CPUCommonCB),
				.Usage = D3D11_USAGE_DYNAMIC,
				.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
				.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			};

			CheckHResult(GDevice->CreateBuffer(&ConstantBufferDescription, nullptr, &CommonCB));
		}

		while (glfwWindowShouldClose(GWindow) == GLFW_FALSE) {
			glfwPollEvents();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::ColorEdit4("Square Color", &CPUCommonCB.Color.x);

			{
				auto Subresource = D3D11_MAPPED_SUBRESOURCE{};
				CheckHResult(GContext->Map(CommonCB.Get(), 0U, D3D11_MAP_WRITE_DISCARD, NULL, &Subresource));
				memcpy(Subresource.pData, &CPUCommonCB, sizeof(CPUCommonCB));
				GContext->Unmap(CommonCB.Get(), 0U);
			}

			float Color[4] = { 0.1F, 0.1F, 0.1F, 1.0F };
			GContext->ClearRenderTargetView(GSwapChainRenderTargetView.Get(), Color);

			UINT Stride = sizeof(TVertex);
			UINT Offset = 0U;

			GContext->IASetVertexBuffers(0U, 1U, VertexBuffer.GetAddressOf(), &Stride, &Offset);
			GContext->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0U);

			GContext->VSSetShader(VertexShader.Get(), nullptr, 0);
			GContext->PSSetShader(PixelShader.Get(), nullptr, 0);

			GContext->OMSetRenderTargets(1U, GSwapChainRenderTargetView.GetAddressOf(), nullptr);

			GContext->RSSetViewports(1, &Viewport);

			GContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			GContext->IASetInputLayout(InputLayout.Get());

			GContext->RSSetState(RasterizerState.Get());

			GContext->PSSetConstantBuffers(0U, 1U, CommonCB.GetAddressOf());

			GContext->DrawIndexed(Indices.size(), 0U, 0U);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			GSwapChain->Present(1, 0);
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		DeinitWindow();
	}
}

auto main() -> int {
	try {
		Engine::Main();
	}
	catch (const std::exception& Ex) {
		spdlog::error("{}", Ex.what());
	}
}
