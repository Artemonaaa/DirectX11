#include <stdexcept>

#include <comdef.h>

#include "HResult.h"

namespace Engine {
	auto CheckHResult(HRESULT HResult) -> void {
		if (!SUCCEEDED(HResult)) {
			auto Error = _com_error(HResult);
			LPCTSTR ErrorMessage = Error.ErrorMessage();
			throw std::runtime_error(ErrorMessage);
		}
	}
}
