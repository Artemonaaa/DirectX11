#pragma once

typedef _Return_type_success_(return >= 0) long HRESULT;

namespace Engine {
	auto CheckHResult(HRESULT HResult) -> void;
}
