cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy (SET CMP0141 NEW)
  set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

project ("Engine")

add_executable (
	Engine 
	"Source/main.cpp" 
	"External/imgui/imgui.cpp" 
	"External/imgui/imgui_demo.cpp" 
	"External/imgui/imgui_draw.cpp" 
	"External/imgui/imgui_tables.cpp" 
	"External/imgui/imgui_widgets.cpp"
	"External/imgui/backends/imgui_impl_dx11.cpp"
	"External/imgui/backends/imgui_impl_glfw.cpp"
  "Source/Graphics/Graphics.cpp" "Source/Graphics/Graphics.h" "Source/HResult.h" "Source/HResult.cpp" "Source/Graphics/Vertices.h" "Source/Window.h" "Source/Window.cpp")

target_include_directories (
	Engine 
	PRIVATE
	"External/imgui/"
	"External/imgui/backends/"
	"Source/"
)

set_property (TARGET Engine PROPERTY CXX_STANDARD 23)

add_subdirectory ("External/glfw")
add_subdirectory ("External/glm")
add_subdirectory ("External/spdlog")
add_subdirectory ("External/entt")
add_subdirectory ("External/nowide")

find_library (D3D11_LIBRARY d3d11.lib PATHS "$ENV{DXSDK_DIR}/Lib/x86" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x86")
find_library (D3D_COMPILER d3dcompiler.lib PATHS "$ENV{DXSDK_DIR}/Lib/x86" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x86")

target_link_libraries (Engine PRIVATE glfw)
target_link_libraries (Engine PRIVATE glm)
target_link_libraries (Engine PRIVATE spdlog)
target_link_libraries (Engine PRIVATE EnTT::EnTT)
target_link_libraries (Engine PRIVATE nowide)
target_link_libraries (Engine PRIVATE "${D3D11_LIBRARY}")
target_link_libraries (Engine PRIVATE "${D3D_COMPILER}")

target_compile_definitions (Engine PUBLIC GLFW_INCLUDE_NONE)
target_compile_definitions (Engine PUBLIC GLFW_EXPOSE_NATIVE_WIN32)
