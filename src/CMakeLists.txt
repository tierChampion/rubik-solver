# CMakeList.txt : CMake project for RubikSolver, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (RubikSolver 
"app.cpp" 
"cube/cube.cpp"
"cube/cubemodel.cpp"
"cube/cubiemodel.cpp"
"cube/solver.cpp"
"cube/state.cpp"
"ui/window.cpp"
"glsl/program.cpp"
"glsl/shader.cpp"
"image/texture.cpp"
"meshes/cyclic.cpp"
"meshes/loader.cpp"
"meshes/splitter.cpp"
"meshes/triangulation.cpp"
"opengl/camera.cpp"
"opengl/vao.cpp"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_link_libraries(RubikSolver glfw3)
	target_link_libraries(RubikSolver glew32s)
	target_link_libraries(RubikSolver opengl32)
endif()

# TODO: Add tests and install targets if needed.
