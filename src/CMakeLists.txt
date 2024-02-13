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
"cube/move.cpp"
"ui/window.cpp"
"ui/keyboard.cpp"
"ui/mouse.cpp"
"glsl/program.cpp"
"glsl/shader.cpp"
"image/texture.cpp"
"meshes/cyclic.cpp"
"meshes/loader.cpp"
"meshes/splitter.cpp"
"meshes/triangulation.cpp"
"opengl/camera.cpp"
"opengl/vao.cpp"
"../deps/imgui/imgui.cpp"
"../deps/imgui/imgui_demo.cpp"
"../deps/imgui/imgui_draw.cpp"
"../deps/imgui/imgui_impl_opengl3.cpp"
"../deps/imgui/imgui_impl_glfw.cpp"
"../deps/imgui/imgui_tables.cpp"
"../deps/imgui/imgui_widgets.cpp"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_link_libraries(RubikSolver glfw3 glew32s opengl32)

elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(RubikSolver glfw ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

endif()

# TODO: Add tests and install targets if needed.
