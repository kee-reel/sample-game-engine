# CMake entry point
cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( PROJECT_NAME sample-game-engine )

find_package( OpenGL REQUIRED )

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

add_subdirectory (libs)
include_directories(
	libs/glfw/include/
	libs/glew/include/
	libs/glm/
	libs/stb/
	libs/lua/upstream/include/
	libs/sol2/include/
	libs/assimp/include/
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	# Required for STB to work
	-DSTB_IMAGE_IMPLEMENTATION
)

set( HEADERS
	src/sge.h
	src/application.h
	src/resource_loader.h
	src/component.h
	src/game_object.h
	src/script.h
    #src/script_state.h
	src/mesh.h
	src/shader.h
	src/texture.h
	src/material.h
	src/transform.h
	src/camera.h
	src/light.h
	src/shader_config.h
	src/util.h
	src/includes.h
	src/tick_limiter.h

    #    src/ECS/ecs.h
    #    src/ECS/entity.h
    #    src/ECS/component.h
    #    src/ECS/transform.h
    #    src/ECS/render.h
    #    src/ECS/script.h
    #    src/ECS/system.h
    #    src/ECS/render_system.h
    #    src/ECS/script_system.h
)

set( SOURCES
	src/application.cpp
	src/resource_loader.cpp
	src/mesh.cpp
	src/shader.cpp
	src/texture.cpp
	src/material.cpp
	src/transform.cpp
	src/camera.cpp
	src/light.cpp
	src/shader_config.cpp
	src/util.cpp

    #    src/ECS/ecs.cpp
    #    src/ECS/entity.cpp
    #    src/ECS/component.cpp
    #    src/ECS/transform.cpp
    #    src/ECS/render.cpp
    #    src/ECS/script.cpp
)

include(GenerateExportHeader)
add_library( ${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} )
generate_export_header( ${PROJECT_NAME} )
target_link_libraries( ${PROJECT_NAME} PRIVATE
	glfw
	glew
    lua::lib
    sol2
    assimp
    nlohmann_json
)
target_include_directories( ${PROJECT_NAME} PRIVATE
	${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
)
