#pragma once

#include <glm/vec3.hpp>
#include <imgui.h>
#include <imfilebrowser.h>

#include "cube/cube.h"
#include "image/texture.h"
#include "opengl/camera.h"
#include "glsl/program.h"

class GameWindow;

class Application
{
    GameWindow *_window;
    int _width;
    int _height;
    float _aspectRatio;
    std::string _projPath;

    rubik::CubeType _type;
    std::string _texturePath;
    std::string _objPath;
    float _reflectivity;
    float _shineDamper;
    glm::vec3 _cameraPos;

    Texture _img;
    std::vector<Vao> _vaos;
    Camera _cam;
    GLSLProgram _program;
    rubik::Cube _cube;

    GLint _texLocation, _vpLocation, _camPosLocation, _refLocation, _shineLocation;

    int _frame;

    bool _cubeBrowserOpen;
    ImGui::FileBrowser _cubeBrowser;

    bool _algoBrowserOpen;
    ImGui::FileBrowser _algoBrowser;

public:
    static Application &getInstance();
    int launch();

private:
    Application();
    bool initGL();
    bool initGLFW();
    bool initImGui();

    void loadMeshes();

    void setCubeType(rubik::CubeType type);
    void applyCubeType();

    void renderImGui();
};