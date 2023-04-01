#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace Engine {
	namespace WRL = Microsoft::WRL;

	inline WRL::ComPtr<ID3D11Device> GDevice;
	inline WRL::ComPtr<ID3D11DeviceContext> GContext;

	inline WRL::ComPtr<IDXGISwapChain> GSwapChain;
	inline WRL::ComPtr<ID3D11RenderTargetView> GSwapChainRenderTargetView;

	auto CompileShader(std::string FilePath, const char* EntryPoint, const char* Target) -> WRL::ComPtr<ID3DBlob>;

	auto InitGraphics(HWND WindowHandle) -> void;
}
