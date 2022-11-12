#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <glew.h>
#include <glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "helpers/helper_gl.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <thread>
#include <future>

#include "image/texture.h"
#include "glfw/window.h"
#include "glfw/keyboard.h"
#include "glfw/mouse.h"
#include "cube/cube.h"
#include "opengl/vao.h"
#include "opengl/camera.h"
#include "glsl/program.h"

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

Window* window;

const static unsigned int W_WIDTH = 1280;
const static unsigned int W_HEIGHT = 720;
const static char* W_TITLE = "Rubik's cube solver";
static bool FULL_SCREEN = false;

static bool MIRROR = false;
const static glm::vec3 CAMERA_POS(0, 0, 30);
const static float FOV = 30.0f;
const static float ASPECT_RATIO = W_WIDTH / float(W_HEIGHT);
const static float NEAR = 0.1f;
const static float FAR = 100.0f;

void test() {

	glm::vec3 planeNorm(0, 1, 0);
	float planeDist = 1;

	glm::vec3 p0(1, 4, 0);
	glm::vec3 p1(0, 0, -10);

	glm::vec3 dir = p0 - p1;

	float t = planeDist - glm::dot(planeNorm, p1);
	t /= glm::dot(planeNorm, dir);

	glm::vec3 pi = p1 + t * dir;

	std::cout << pi.x << " " << pi.y << " " << pi.z << std::endl;
}

bool initGL() {
	glewExperimental = GL_TRUE;
	/* Initialize GLEW */
	GLenum err = glewInit();
	glGetError();
	if (err != GLEW_OK) {
		printf("Problem while initialising glew: %s \n", glewGetErrorString(err));
		exit(1);
	}

	SDK_CHECK_ERROR_GL();
	return true;
}

bool initGLFW() {
	if (!glfwInit()) exit(EXIT_FAILURE);
	// Set GLFW constants
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create GLFW context and window
	window = new Window(W_WIDTH, W_HEIGHT, ASPECT_RATIO, W_TITLE, FULL_SCREEN);
	window->makeCurrentContext();
	window->setSwapInterval(1);
	return true;
}

int main(int argc, char** argv) {
	test();
	std::cout << "$$$$$$$\\             $$\\       $$\\ $$\\             $$$$$$\\            $$\\	\n"
		<< "$$  __$$\\            $$ |      \\__|$$ |           $$  __$$\\           $$ |	\n"
		<< "$$ |  $$ | $$\\   $$\\ $$$$$$$\\  $$\\ $$ |  $$\\      $$ /  \\__| $$$$$$\   $$ |$$\\    $$\\  $$$$$$\\    $$$$$$\\	\n"
		<< "$$$$$$$  | $$ |  $$ |$$  __$$\\ $$ |$$ | $$  |     \\$$$$$$\\  $$  __$$\\ $$ |\\$$\\  $$  |$$  __$$\\  $$  __$$\\	\n"
		<< "$$  __$$ < $$ |  $$ |$$ |  $$ |$$ |$$$$$$  /       \\____$$\\ $$ /  $$ |$$ | \\$$\\$$  / $$$$$$$$ | $$ |  \\__|	\n"
		<< "$$ |  $$ | $$ |  $$ |$$ |  $$ |$$ |$$  _$$<       $$\\   $$ |$$ |  $$ |$$ |  \\$$$  /  $$   ____| $$ |	\n"
		<< "$$ |  $$ | \\$$$$$$  |$$$$$$$  |$$ |$$ | \\$$\\      \\$$$$$$  |\\$$$$$$  |$$ |   \\$  /   \\$$$$$$$\\  $$ |	\n"
		<< "\\__|  \\__|  \\______/ \\_______/ \\__|\\__|  \\__|      \\______/  \\______/ \\__|    \\_/     \\_______| \\__|	\n"
		<< std::endl;

	if (argc > 1) {

		for (int i = 1; i < argc; i++) {

			std::string_view arg = std::string_view(argv[i]);

			if (arg == "--mirror" || arg == "-m") {
				MIRROR = true;
			}
			else if (arg == "--fullscreen" || arg == "-fs") {
				FULL_SCREEN = true;
			}
			else if (arg == "/?") {

				std::cout << "[--mirror | -m] [-fullscreen | -fs]\n"
					<< "--mirror | -m			Use a mirror cube instead of rubiks cube\n"
					<< "--fullscreen | -fs		Display in full screen\n" << std::endl;
				exit(0);
			}
		}
	}

	char buffer[255] = "";
	char* execPath = _fullpath(buffer, argv[0], sizeof(buffer));

	std::string PROJ_PATH(execPath);
	size_t firstLoc = PROJ_PATH.find("\\RubikSolver\\");

	PROJ_PATH = PROJ_PATH.substr(0, firstLoc) + "\\RubikSolver\\";

	const char* TEXTURE_PATH = MIRROR ? "res/mirror.png" : "res/cubie.png";
	const float REFLECTIVITY = MIRROR ? 0.8f : 0.0f;
	const float SHINE_DAMPER = MIRROR ? 5.0f : 5.0f;

	initGLFW();
	initGL();

	// --> Enable proper alignement for Opengl
	stbi_set_flip_vertically_on_load(1);

	/* Creation of the texture. */
	Texture img((PROJ_PATH + TEXTURE_PATH).c_str());
	img.passToOpenGL();
	img.bind(0);

	/* Creation of the cube and it's model. */
	Vao vao((PROJ_PATH + "res/cubie.obj").c_str());
	rubik::Cube cube(MIRROR);

	/* Creation of the camera. */
	Camera cam;
	cam.createView(CAMERA_POS, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	cam.createPerspective(FOV, ASPECT_RATIO, NEAR, FAR);

	/* Creation of the shader program. */
	GLSLProgram program((PROJ_PATH + "src/shaders/vertex.glsl").c_str(),
		(PROJ_PATH + "src/shaders/fragment.glsl").c_str());
	program.compile();
	program.use();

	/* Bind the texture to the shader. */
	GLint texLocation = glGetUniformLocation(program._programId, "tex");
	glUniform1i(texLocation, 0);
	/* Bind the view and the projection matrix and the camera position to the shader. */
	GLint vpLocation = glGetUniformLocation(program._programId, "view_projection");
	glm::mat4 viewProjection = cam.getVP();
	glUniformMatrix4fv(vpLocation, 1, GL_FALSE, &viewProjection[0][0]);
	GLint camPosLocation = glGetUniformLocation(program._programId, "cameraPos");
	glUniform3f(camPosLocation, CAMERA_POS[0], CAMERA_POS[1], CAMERA_POS[2]);
	/* Bind the material settings to the shader. */
	GLint refLocation = glGetUniformLocation(program._programId, "reflectivity");
	glUniform1f(refLocation, REFLECTIVITY);
	GLint shineLocation = glGetUniformLocation(program._programId, "shineDamper");
	glUniform1f(shineLocation, SHINE_DAMPER);

	/* Bg color. */
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	/* Enables backface culling */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	/* Enables depth testing */
	glEnable(GL_DEPTH_TEST);

	/* Callbacks for controls. */
	glfwSetKeyCallback(window->getWindow(), Keyboard::turningCube_KeyCallback);
	glfwSetMouseButtonCallback(window->getWindow(), Mouse::mousebuttonCallback);

	/* Control information */
	std::cout << "\n=== Controls ===\n"
		<< "<Face turns>\n"
		<< "	Q(Red), W(Blue), E(Yellow)\n"
		<< "	A(Orange), S(Green), D(White)\n"
		<< "	Shift(Counter clockwise), Space(Double turn)\n"
		<< "<Others>\n"
		<< "	Enter(Solve the cube)\n"
		<< "	Backspace(Scramble the cube)\n"
		<< "	Left click drag(Turn the cube)\n" << std::endl;

	SDK_CHECK_ERROR_GL();

	int frame = 0;

	while (window->running()) {

		cube.update();
		cube.render(vao, program._programId);

		/* Mouse controls <Whole cube orientation> */
		Mouse::update(window->getWindow());
		glm::vec2 delta = Mouse::getDelta();
		if (delta[0] != 0 && delta[1] != 0)
			cube.turnCube(delta);

		/* Keyboard controls <Face turning, mixing and solving> */
		rubik::Move input = Keyboard::getMove(window->getWindow());
		if (input._type != rubik::MoveType::NONE && !cube.isSolving()) {
			cube.turnFace(input);
		}

		/* Solve the rubik's cube. Do this asynchronously since it can be long */
		if (glfwGetKey(window->getWindow(), GLFW_KEY_ENTER) && !cube.isSolving()) {
			std::thread solvingThread(&rubik::Cube::solve, &cube);
			solvingThread.detach();
			frame = 0;
		}

		/* Scramble the rubik's cube */
		if (glfwGetKey(window->getWindow(), GLFW_KEY_BACKSPACE) && frame % 5 == 0 && !cube.isSolving()) {
			cube.mix();
			frame = 0;
		}

		/* Display cubestate */
		if (glfwGetKey(window->getWindow(), GLFW_KEY_0) && frame % 10 == 0 && !cube.isSolving()) {
			std::cout << cube;
		}

		SDK_CHECK_ERROR_GL();

		/* Prepare for next frame. */
		window->swapBuffers();
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		frame++;
	}
}
