#include <stdexcept>

#include <d3dcompiler.h>
#include <nowide/convert.hpp>

#include "HResult.h"

#include "Graphics.h"	

namespace Engine {

	auto CompileShader(std::string FilePath, const char* EntryPoint, const char* Target) -> WRL::ComPtr<ID3DBlob>
	{
		WRL::ComPtr<ID3DBlob> ByteCode;
		WRL::ComPtr<ID3DBlob> ErrorMsgs;

		HRESULT HResult = D3DCompileFromFile(nowide::widen(FilePath).c_str(), nullptr, nullptr, EntryPoint, Target, NULL, NULL, &ByteCode, &ErrorMsgs);
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

	auto InitGraphics(HWND WindowHandle) -> void {
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
	}

}
