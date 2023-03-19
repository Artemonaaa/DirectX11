cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy (SET CMP0141 NEW)
  set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

project ("DirectX11")

add_executable (DirectX11 "main.cpp")

set_property (TARGET DirectX11 PROPERTY CXX_STANDARD 23)

add_subdirectory ("External/glfw")

target_link_libraries (DirectX11 PRIVATE glfw)

target_compile_definitions (DirectX11 PUBLIC GLFW_INCLUDE_NONE)