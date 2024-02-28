#include "app.h"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "helper_gl.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imfilebrowser.h>

#include <thread>
#include <unistd.h>

#include "ui/window.h"
#include "ui/keyboard.h"
#include "ui/mouse.h"
#include "meshes/meshsplitter.h"
#include "logging/algoparser.h"

static GLFWmonitor* MONITOR;
const static unsigned int W_WIDTH = 1080;
const static unsigned int W_HEIGHT = 720;
const static char *W_TITLE = "Rubik's cube solver";
static bool FULL_SCREEN = false;

const static float FOV = 30.0f;
const static float ASPECT_RATIO = W_WIDTH / float(W_HEIGHT);
const static float NEAR_DIST = 0.1f;
const static float FAR_DIST = 100.0f;

Application &Application::getInstance()
{
    static Application instance;
    return instance;
}

Application::Application() : _frame(0)
{
    srand(time(NULL));

    _projPath = std::string(DIRECTORY_PATH);

    initGLFW();
    initGL();
    initImGui();
    stbi_set_flip_vertically_on_load(1);

    _program = GLSLProgram((_projPath + "/src/shaders/vertex.glsl").c_str(),
                           (_projPath + "/src/shaders/fragment.glsl").c_str());
    _program.compile();
    _program.use();

    _texLocation = glGetUniformLocation(_program._programId, "tex");
    glUniform1i(_texLocation, 0);
    _vpLocation = glGetUniformLocation(_program._programId, "view_projection");
    _camPosLocation = glGetUniformLocation(_program._programId, "cameraPos");
    glUniform3f(_camPosLocation, 0, 0, 100);
    _refLocation = glGetUniformLocation(_program._programId, "reflectivity");
    _shineLocation = glGetUniformLocation(_program._programId, "shineDamper");

    setCubeType(rubik::CubeType::REGULAR);
    applyCubeType();
}

int Application::launch()
{
    glm::mat4 viewProjection = _cam.getVP();
    glUniformMatrix4fv(_vpLocation, 1, GL_FALSE, &viewProjection[0][0]);

    /* Bg color. */
    glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

    /* Enables backface culling */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    /* Enables depth testing and anti-aliasing */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    /* Callbacks for controls. */
    glfwSetKeyCallback(_window->getWindow(), Keyboard::turningCube_KeyCallback);
    glfwSetMouseButtonCallback(_window->getWindow(), Mouse::mouseButtonCallback);
    glfwSetScrollCallback(_window->getWindow(), Mouse::mouseScrollCallback);

    SDK_CHECK_ERROR_GL();

    while (_window->running())
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderImGuiMenuBar();

        _cube.update();
        _cube.render(_vaos, _program._programId);

        /* Mouse controls <Whole cube orientation> */
        Mouse::update(_window->getWindow());
        glm::vec2 drag = Mouse::getDrag();
        if (drag != glm::vec2(0.0f))
            _cube.turnCube(drag);

        /* Keyboard controls <Face turning, mixing and solving> */
        rubik::Move input = Keyboard::getMove(_window->getWindow());
        if (input._type != rubik::MoveType::NONE && !_cube.isSolving())
        {
            _cube.turnFace(input);
        }

        /* Solve the rubik's cube. Do this asynchronously since it can be long */
        if (glfwGetKey(_window->getWindow(), GLFW_KEY_ENTER) && !_cube.isSolving())
        {
            std::thread solvingThread(&rubik::Cube::solve, &_cube);
            solvingThread.detach();
            // wait?
            _frame = 0;
        }

        /* Scramble the rubik's cube */
        if (glfwGetKey(_window->getWindow(), GLFW_KEY_BACKSPACE) && _frame % 5 == 0 && !_cube.isSolving())
        {
            _cube.mix();
            _frame = 0;
        }

        float scroll = Mouse::getScroll();

        /* Zoom in */
        if (glfwGetKey(_window->getWindow(), GLFW_KEY_UP) || scroll > 0)
        {
            _cameraPos -= glm::vec3(0, 0, 2);
            _cam.createView(_cameraPos, glm::vec3(0), glm::vec3(0, 1, 0));

            viewProjection = _cam.getVP();
            glUniformMatrix4fv(_vpLocation, 1, GL_FALSE, &viewProjection[0][0]);
        }
        /* Zoom out */
        else if (glfwGetKey(_window->getWindow(), GLFW_KEY_DOWN) || scroll < 0)
        {
            _cameraPos += glm::vec3(0, 0, 2);
            _cam.createView(_cameraPos, glm::vec3(0), glm::vec3(0, 1, 0));

            viewProjection = _cam.getVP();
            glUniformMatrix4fv(_vpLocation, 1, GL_FALSE, &viewProjection[0][0]);
        }

        /* Display cubestate */
        if (glfwGetKey(_window->getWindow(), GLFW_KEY_0) && _frame % 10 == 0 && !_cube.isSolving())
        {
            std::cout << _cube;
        }

        SDK_CHECK_ERROR_GL();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Prepare for next frame. */
        _window->swapBuffers();
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _frame++;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

bool Application::initGL()
{
    glewExperimental = GL_TRUE;
    /* Initialize GLEW */
    GLenum err = glewInit();
    glGetError();
    if (err != GLEW_OK)
    {
        printf("Problem while initialising glew: %s \n", glewGetErrorString(err));
        exit(1);
    }

    SDK_CHECK_ERROR_GL();

    return true;
}

bool Application::initGLFW()
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    // Set GLFW constants
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    MONITOR = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(MONITOR);
    std::cout << "Dimensions: " << mode->width << "x" << mode->height << std::endl;
    // Create GLFW context and window
    _window = new GameWindow(mode->width, mode->height, mode->width / float(mode->height), W_TITLE, FULL_SCREEN);
    _window->makeCurrentContext();
    _window->setSwapInterval(1);
    return true;
}

bool Application::initImGui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    _cubeBrowser = ImGui::FileBrowser();
    _cubeBrowser.SetTitle("Search for OBJ");
    _cubeBrowser.SetTypeFilters({".obj"});

    _cubeBrowser.Open();
    _cubeBrowserOpen = false;

    _algoBrowser = ImGui::FileBrowser();
    _algoBrowser.SetTitle("Search for algorithm");
    _algoBrowser.SetTypeFilters({".algo"});

    _algoBrowser.Open();
    _algoBrowserOpen = false;

    return true;
}

void Application::loadMeshes()
{
    _vaos.clear();

    splr::MeshData originalMesh;
    bool found = splr::loadObj(_objPath, originalMesh);

    if (!found)
    {
        std::cerr << "ERROR: The mesh file " << _objPath << " could not be opened." << std::endl;
    }

    splr::MeshSplitter splitter(originalMesh);

    /// SLICING OF THE MESH IN THE CASE OF A SPLIT MESH ///
    if (_type == rubik::CubeType::SPLIT)
    {
        splitter.splitMeshIntoRubik();
    }

    for (splr::MeshData mesh : splitter.getMeshes())
    {
        _vaos.push_back(Vao(mesh));
    }
}

void Application::setCubeType(rubik::CubeType type)
{
    _type = type;

    switch (_type)
    {
    case rubik::CubeType::MIRROR:
        _texturePath = _projPath + "/res/Textures/mirror.png";
        _objPath = _projPath + "/res/cubie.obj";
        _cameraPos = glm::vec3(0, 0, 30);
        _reflectivity = 0.8f;
        _shineDamper = 5.0f;
        break;

    case rubik::CubeType::SPLIT:
        _texturePath = _projPath + "/res/Textures/cubie.png";
        if (_objPath == "" || _objPath == _projPath + "/res/cubie.obj")
            _objPath = _projPath + "/res/ghost.obj";
        _cameraPos = glm::vec3(0, 0, 40);
        _reflectivity = 0.8f;
        _shineDamper = 0.6f;
        break;

    default:
        _texturePath = _projPath + "/res/Textures/cubie.png";
        _objPath = _projPath + "/res/cubie.obj";
        _cameraPos = glm::vec3(0, 0, 30);
        _reflectivity = 0.0f;
        _shineDamper = 5.0f;
        break;
    }
}

void Application::applyCubeType()
{
    _img = Texture(_texturePath.c_str());
    _img.passToOpenGL();
    _img.bind(0);

    _cam.createView(_cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    _cam.createPerspective(FOV, 1920.0 / 1200.0, NEAR_DIST, FAR_DIST);

    _cube.changeType(_type);
    loadMeshes();

    glUniform1f(_refLocation, _reflectivity);
    glUniform1f(_shineLocation, _shineDamper);
}

void Application::renderImGuiMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Mode"))
        {
            if (ImGui::MenuItem("change"))
            {
                switch (_type)
                {
                case rubik::CubeType::REGULAR:
                    setCubeType(rubik::CubeType::MIRROR);
                    break;
                default:
                    setCubeType(rubik::CubeType::REGULAR);
                    break;
                }
                applyCubeType();
            }

            if (ImGui::MenuItem("split"))
            {
                _cubeBrowserOpen = true;
                _cubeBrowser.Open();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Algorithms"))
        {
            if (ImGui::MenuItem("Import Algorithms"))
            {
                _algoBrowserOpen = true;
                _algoBrowser.Open();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        if (_cubeBrowserOpen)
        {
            _cubeBrowser.Display();

            if (_cubeBrowser.HasSelected())
            {
                _objPath = _cubeBrowser.GetSelected();

                setCubeType(rubik::CubeType::SPLIT);
                applyCubeType();

                _cubeBrowser.ClearSelected();
                _cubeBrowserOpen = false;
            }
        }
        if (_algoBrowserOpen)
        {
            _algoBrowser.Display();

            if (_algoBrowser.HasSelected())
            {
                std::string algoPath = _algoBrowser.GetSelected();

                std::vector<rubik::Move> algo = parsing::parseAlgorithm(algoPath);

                for (auto move : algo)
                {
                    _cube.turnFace(move);
                }

                _algoBrowser.ClearSelected();
                _algoBrowserOpen = false;
            }
        }
    }
}
