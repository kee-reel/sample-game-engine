add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_BUILD
)

### GLFW ###

add_subdirectory(glfw)

include_directories(
	glfw/include/GLFW/
	glew/include/
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set( OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        -lGL -lGLU -lXrandr -lXext -lX11 -lrt
        ${CMAKE_DL_LIBS}
        ${GLFW_LIBRARIES}
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set( OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        ${CMAKE_DL_LIBS}
        ${GLFW_LIBRARIES}
    )
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

### GLEW ###

add_library( glew STATIC
	glew/src/glew.c
)

target_link_libraries( glew
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

### Lua ###

add_subdirectory(lua)

### Sol2 (Lua and C++ integration) ###

add_subdirectory(sol2)

### Assimp ###

add_subdirectory( assimp )

include_directories(
	assimp/include/
)

### Json ###

add_subdirectory( json )

include_directories(
	json/include/
)

### boost ###

add_subdirectory(boost-cmake)

