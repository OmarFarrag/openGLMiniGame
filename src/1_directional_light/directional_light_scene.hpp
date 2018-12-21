#ifndef DIRECTIONAL_LIGHT_SCENE_HPP
#define DIRECTIONAL_LIGHT_SCENE_HPP

#define GLM_FORCE_CXX11
#include <glm/glm.hpp>
#include "../tank/tank.h"

#include <scene.hpp>
#include <shader.hpp>
#include <mesh/mesh.hpp>
#include <textures/texture2d.hpp>
#include <camera/camera.hpp>
#include <camera/controllers/fly_camera_controller.hpp>

enum TextureType
{
    ALBEDO = 0,
    SPECULAR = 1,
    ROUGHNESS = 2,
    AMBIENT_OCCLUSION = 3,
    EMISSIVE = 4
};

class DirectionalLightScene : public Scene
{
  private:
    Shader *shader;
    Shader *skyShader;
    Mesh *ground;
    Mesh *sky;
    Mesh *model, *tankMesh;
    bool check = false;
    float num = 4.33337;
    Texture2D *TankText[5];
    Texture2D *metal[5];
    Texture2D *wood[5];
    Texture2D *asphalt[5];
    Texture2D *checkers[5];
    Camera *camera;
    GLuint mvpLoc;
    FlyCameraController *controller;
    glm::vec3 TankPosition;
    float TankRotation = 0;

    float sunYaw, sunPitch;

    Shader *terrainShader, *bulletShader;
    GLuint terrainmvpLoc, terraintexLoc, terraintilingLoc, terrainbottomLoc, terraintopLoc, bulletmvp, bullettexLoc;
    Mesh *plane;
    Texture2D *heightmap, *bottomTex, *topTex, *moonTex;
    std::vector<Mesh *> bullets = std::vector<Mesh *>();
    std::vector<glm::vec3> bulletsPositions, bulletsDirections;
    glm::vec3 cameraPosition;
    std::vector<Tank*> tanks;

  public:
    DirectionalLightScene(Application *app) : Scene(app) {}

    void Initialize() override;
    void Update(double delta_time) override;
    void Draw() override;
    void Finalize() override;

    void initMap();
    void drawMap(glm::mat4 VP);
    void initBullets();
    void addBullet();
    void drawBullet(glm::mat4 VP);
    void drawTank();
};

#endif