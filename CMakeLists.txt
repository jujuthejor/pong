cmake_minimum_required(VERSION 3.21.2)

project(pong C)


set(sources src/main.c)

# TODO: set libs for Linux platform
if(WIN32)
	set(libs raylib opengl32 gdi32 winmm)
endif()

add_executable("${PROJECT_NAME}" ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99 C_EXTENSIONS OFF)

if(WIN32 AND MINGW)
	target_link_directories("${PROJECT_NAME}" PUBLIC "lib/mingw")
	target_include_directories("${PROJECT_NAME}" PUBLIC "include/mingw")
elseif(MSVC)
	target_link_directories("${PROJECT_NAME}" PUBLIC "lib/msvc")
	target_include_directories("${PROJECT_NAME}" PUBLIC "include/msvc")
endif()

target_link_libraries("${PROJECT_NAME}" ${libs})

if(UNIX OR MINGW)
	target_compile_options("${PROJECT_NAME}" PRIVATE -Wall -Wextra -Werror -std=c99 -pedantic)
	target_link_options("${PROJECT_NAME}" PRIVATE LINKER:--subsystem,windows)
elseif(MSVC)
	#target_compile_options("${PROJECT_NAME}" PRIVATE /W4 /Wall /WX)
	target_compile_options("${PROJECT_NAME}" PRIVATE /W4)
	target_link_options("${PROJECT_NAME}" PRIVATE LINKER:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()
