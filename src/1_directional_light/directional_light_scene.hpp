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
#include <framebuffer.hpp>

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
    bool died=false;
    int blurCounter=20;

    int spawnCounter;
    int spawnDelay;     // Tank spawning speed
    int locationsCount; // Number of tank columns
    int *tankLocations; // Number of tanks in each column. Zeroes initially
    int maxPerColumn;   // Max number of tanks per column
    
    Texture2D *TankText[5];
    Texture2D *checkers[5];
    Camera *camera;
    GLuint mvpLoc;
    GLuint skyColorLoc1;
    GLuint skyColorLoc2;
    FlyCameraController *controller;
    glm::vec3 TankPosition;
    float TankRotation = 0;

    float sunYaw, sunPitch;

    Shader *terrainShader, *bulletShader,*blurredShader,*pixalShader;
    GLuint terrainmvpLoc, terraintexLoc, terraintilingLoc, terrainbottomLoc, terraintopLoc, bulletmvp, bullettexLoc;
    Mesh *plane,*quad,*quadPixal;
    Texture2D *heightmap, *bottomTex, *topTex, *moonTex,*fboTex, *fboDepthTex,*fboTexPixal,*fboDepthTexPixal;
    FrameBuffer* fboPixal;
    FrameBuffer* fbo;
    std::vector<Mesh *> bullets = std::vector<Mesh *>();
    std::vector<glm::vec3> bulletsPositions, bulletsDirections;
     std::vector<Mesh *> enemybullets = std::vector<Mesh *>();
    std::vector<glm::vec3> enemybulletsPositions, enemybulletsDirections;
    glm::vec3 cameraPosition;
    std::vector<Tank*> tanks;
    int playerHealth=1000;
    bool useBlur=false;
    bool usePixal=false;

  public:
    DirectionalLightScene(Application *app) : Scene(app) {}

    void Initialize() override;
    void Update(double delta_time) override;
    void Draw() override;
    void Finalize() override;

    void initMap();
    void drawMap(glm::mat4 VP, glm::vec3 skyColor);
    void initBullets();
    void addBullet();
    void addEnemyBullet(glm::vec3 enemyTankPosition);
    void drawBullet(glm::mat4 VP);
    void drawEnemyBullet(glm::mat4 VP);
    void drawTank();
    void spawnTank();
    void fight();
    int genRandom();
    void initBlurandGrey();
    void initPixalization();
   
};

#endif