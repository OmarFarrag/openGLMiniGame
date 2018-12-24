#include <application.hpp>
#include "directional_light_scene.hpp"

#include <mesh/mesh_utils.hpp>
#include <textures/texture_utils.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */

#define WHITE      \
    {              \
        1, 1, 1, 1 \
    }
#define QUAD_NORMAL \
    {               \
        0, 0, 1     \
    }

void DirectionalLightScene::initBlurandGrey()
{

    // Create quad to render fboTex on
    quad = new Mesh();
    quad->setup<Vertex>({
                            {{-1, -1, 0}, WHITE, {0, 0}, QUAD_NORMAL},
                            {{1, -1, 0}, WHITE, {1, 0}, QUAD_NORMAL},
                            {{1, 1, 0}, WHITE, {1, 1}, QUAD_NORMAL},
                            {{-1, 1, 0}, WHITE, {0, 1}, QUAD_NORMAL},
                        },
                        {0, 1, 2, 2, 3, 0});

    blurredShader = new Shader();
    blurredShader->attach("assets/shaders/blurred.vert", GL_VERTEX_SHADER);
    blurredShader->attach("assets/shaders/blurred.frag", GL_FRAGMENT_SHADER);
    blurredShader->link();
    // Create our additional frame buffer
    fbo = new FrameBuffer();

    unsigned int width = getApplication()->getWindowSize().x;
    unsigned int height = getApplication()->getWindowSize().y;

    //Create our render target
    fboTex = new Texture2D();
    fboTex->bind();
    fboTex->setup(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fbo->attach(fboTex, GL_COLOR_ATTACHMENT0);

    fboDepthTex = new Texture2D();
    fboDepthTex->bind();
    fboDepthTex->setup(GL_DEPTH24_STENCIL8, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    fbo->attach(fboDepthTex, GL_DEPTH_STENCIL_ATTACHMENT);

    if (fbo->isComplete())
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    useBlur = false;
}

void DirectionalLightScene::initPixalization()
{

    // Create quad to render fboTex on
    quadPixal = new Mesh();
    quadPixal->setup<Vertex>({
                                 {{-1, -1, 0}, WHITE, {0, 0}, QUAD_NORMAL},
                                 {{1, -1, 0}, WHITE, {1, 0}, QUAD_NORMAL},
                                 {{1, 1, 0}, WHITE, {1, 1}, QUAD_NORMAL},
                                 {{-1, 1, 0}, WHITE, {0, 1}, QUAD_NORMAL},
                             },
                             {0, 1, 2, 2, 3, 0});

    pixalShader = new Shader();
    pixalShader->attach("assets/shaders/pixal.vert", GL_VERTEX_SHADER);
    pixalShader->attach("assets/shaders/pixal.frag", GL_FRAGMENT_SHADER);
    pixalShader->link();
    // Create our additional frame buffer
    fboPixal = new FrameBuffer();

    unsigned int width = getApplication()->getWindowSize().x;
    unsigned int height = getApplication()->getWindowSize().y;

    //Create our render target
    fboTexPixal = new Texture2D();
    fboTexPixal->bind();
    fboTexPixal->setup(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fboPixal->attach(fboTexPixal, GL_COLOR_ATTACHMENT0);

    fboDepthTexPixal = new Texture2D();
    fboDepthTexPixal->bind();
    fboDepthTexPixal->setup(GL_DEPTH24_STENCIL8, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    fboPixal->attach(fboDepthTexPixal, GL_DEPTH_STENCIL_ATTACHMENT);

    if (fboPixal->isComplete())
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    usePixal = false;
}

void DirectionalLightScene::initBullets()
{
    bulletShader = new Shader();
    bulletShader->attach("assets/shaders/texture.vert", GL_VERTEX_SHADER);
    bulletShader->attach("assets/shaders/texture.frag", GL_FRAGMENT_SHADER);
    bulletShader->link();
    bulletmvp = glGetUniformLocation(bulletShader->getID(), "MVP");
    bullettexLoc = glGetUniformLocation(bulletShader->getID(), "tex");
    moonTex = TextureUtils::Load2DTextureFromFile("assets/textures/moon.jpg");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void DirectionalLightScene::addBullet()
{
    // glm::vec3 bulletpos=TankPosition;
    // bulletpos.y=6;
    bullets.push_back(MeshUtils::Sphere());
    bulletsPositions.push_back({0, 7, 0});
    bulletsDirections.push_back(controller->getDirection());
}
void DirectionalLightScene::addEnemyBullet(glm::vec3 enemyTankPosition)
{
    enemybullets.push_back(MeshUtils::Sphere());
    enemybulletsPositions.push_back(enemyTankPosition);

    enemyTankPosition[0] = enemyTankPosition[0] / enemyTankPosition.length();
    enemyTankPosition[1] = enemyTankPosition[1] / enemyTankPosition.length();
    enemyTankPosition[2] = enemyTankPosition[2] / enemyTankPosition.length();

    enemybulletsDirections.push_back(-enemyTankPosition);
}

void DirectionalLightScene::drawBullet(glm::mat4 VP)
{
    if (bullets.empty())
        return;
    moonTex->bind();
    bulletShader->use();

    for (size_t i = 0; i < bullets.size(); i++)
    {
        glm::mat4 moon_mat = glm::translate(glm::mat4(), bulletsPositions[i]) *
                             glm::rotate(glm::mat4(), glm::quarter_pi<float>(), {0, 0, 1}) *
                             glm::rotate(glm::mat4(), (float)glfwGetTime(), {0, 1, 0}) *
                             glm::scale(glm::mat4(), glm::vec3(0.01, 0.01, 0.01));
        glUniformMatrix4fv(bulletmvp, 1, GL_FALSE, glm::value_ptr(VP * moon_mat));
        bullets[i]->draw();
    }
}

void DirectionalLightScene::drawEnemyBullet(glm::mat4 VP)
{
    if (enemybullets.empty())
        return;
    moonTex->bind();
    bulletShader->use();

    for (size_t i = 0; i < enemybullets.size(); i++)
    {
        glm::mat4 moon_mat = glm::translate(glm::mat4(), enemybulletsPositions[i]) *
                             glm::rotate(glm::mat4(), glm::quarter_pi<float>(), {0, 0, 1}) *
                             glm::rotate(glm::mat4(), (float)glfwGetTime(), {0, 1, 0}) *
                             glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2));
        glUniformMatrix4fv(bulletmvp, 1, GL_FALSE, glm::value_ptr(VP * moon_mat));
        enemybullets[i]->draw();
    }
}

void DirectionalLightScene::initMap()
{
    terrainShader = new Shader();
    terrainShader->attach("assets/shaders/terrain.vert", GL_VERTEX_SHADER);
    terrainShader->attach("assets/shaders/terrain.frag", GL_FRAGMENT_SHADER);
    terrainShader->link();

    terrainmvpLoc = glGetUniformLocation(terrainShader->getID(), "MVP");
    terraintexLoc = glGetUniformLocation(terrainShader->getID(), "tex");
    terraintilingLoc = glGetUniformLocation(terrainShader->getID(), "tiling");
    terrainbottomLoc = glGetUniformLocation(terrainShader->getID(), "bottom");
    terraintopLoc = glGetUniformLocation(terrainShader->getID(), "top");

    plane = MeshUtils::SubdividedPlane();

    heightmap = TextureUtils::Load2DTextureFromFile("assets/textures/terrainnew.jpg");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bottomTex = TextureUtils::Load2DTextureFromFile("assets/textures/grass_ground_d.jpg");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

    topTex = TextureUtils::Load2DTextureFromFile("assets/textures/mntn_white_d.jpg");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

void DirectionalLightScene::drawMap(glm::mat4 VP)
{
    terrainShader->use();

    glActiveTexture(GL_TEXTURE0);
    heightmap->bind();
    glUniform1i(terraintexLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    bottomTex->bind();
    glUniform1i(terrainbottomLoc, 1);

    glActiveTexture(GL_TEXTURE2);
    topTex->bind();
    glUniform1i(terraintopLoc, 2);

    glUniform2f(terraintilingLoc, 20, 20);

    glm::mat4 ground_mat = glm::scale(glm::mat4(), glm::vec3(200, 25, 200));
    glUniformMatrix4fv(terrainmvpLoc, 1, GL_FALSE, glm::value_ptr(VP * ground_mat));
    plane->draw();
}

void DirectionalLightScene::Initialize()
{
    spawnCounter = -1;
    spawnDelay = 500;
    locationsCount = 25;
    tankLocations = new int[locationsCount];
    for (int i = 0; i < locationsCount; i++)
        tankLocations[i] = 0;
    maxPerColumn = 5;

    cameraPosition = {0, 7, 0};
    initMap();
    initBullets();
    initBlurandGrey();
    initPixalization();
    shader = new Shader();
    shader->attach("assets/shaders/directional.vert", GL_VERTEX_SHADER);
    shader->attach("assets/shaders/directional.frag", GL_FRAGMENT_SHADER);
    shader->link();

    skyShader = new Shader();
    skyShader->attach("assets/shaders/sky.vert", GL_VERTEX_SHADER);
    skyShader->attach("assets/shaders/sky.frag", GL_FRAGMENT_SHADER);
    skyShader->link();

    //TankPosition = {35, 60, 0};
    TankRotation = 0;

    ground = MeshUtils::Plane({0, 0}, {5, 5});
    sky = MeshUtils::Box();
    model = MeshUtils::LoadObj("assets/models/suzanne.obj");
    tankMesh = MeshUtils::LoadObj("assets/models/Tank_M1A1/Tank M1A1.obj");

    mvpLoc = glGetUniformLocation(shader->getID(), "MVP");

    TankText[ALBEDO] = TextureUtils::Load2DTextureFromFile("assets/textures/Metal_col.jpg");
    TankText[SPECULAR] = TextureUtils::Load2DTextureFromFile("assets/textures/Metal_spc.jpg");
    TankText[ROUGHNESS] = TextureUtils::Load2DTextureFromFile("assets/models/Tank_M1A1/tank4.jpg");
    TankText[AMBIENT_OCCLUSION] = TextureUtils::Load2DTextureFromFile("assets/textures/Suzanne_ao.jpg");
    TankText[EMISSIVE] = TextureUtils::SingleColor({0, 0, 0, 1});

    checkers[ALBEDO] = TextureUtils::CheckerBoard({2048, 2048}, {128, 128}, {1, 1, 1, 1}, {0, 0, 0, 1});
    checkers[SPECULAR] = TextureUtils::CheckerBoard({2048, 2048}, {128, 128}, {0.2f, 0.2f, 0.2f, 1}, {1, 1, 1, 1});
    checkers[ROUGHNESS] = TextureUtils::CheckerBoard({2048, 2048}, {128, 128}, {0.9f, 0.9f, 0.9f, 1}, {0.4f, 0.4f, 0.4f, 1});
    checkers[AMBIENT_OCCLUSION] = TextureUtils::SingleColor({1, 1, 1, 1});
    checkers[EMISSIVE] = TextureUtils::SingleColor({0, 0, 0, 1});

    camera = new Camera();
    glm::ivec2 windowSize = getApplication()->getWindowSize();
    camera->setupPerspective(glm::pi<float>() / 2, (float)windowSize.x / windowSize.y, 0.1f, 1000.0f);
    camera->setUp({0, 1, 0});

    controller = new FlyCameraController(this, camera);
    controller->setYaw(3);
    controller->setPitch(-glm::quarter_pi<float>());
    controller->setPosition(cameraPosition);

    //TankTex = TextureUtils::#include <textures/texture2d.hpp>Load2DTextureFromFile("assets/models/Tank_M1A1/tank4.jpg");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    sunYaw = sunPitch = glm::quarter_pi<float>();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.88f, 0.68f, 0.15f, 0.0f);
}

void DirectionalLightScene::Update(double delta_time)
{
    controller->update(delta_time);
    Keyboard *kb = getKeyboard();
    if (getKeyboard()->justPressed(GLFW_KEY_T))
        usePixal = !usePixal;

    //cameraPosition = controller->getPosition();

    float pitch_speed = 1.0f, yaw_speed = 1.0f;

    if (kb->isPressed(GLFW_KEY_I))
        sunPitch += (float)delta_time * pitch_speed;
    if (kb->isPressed(GLFW_KEY_K))
        sunPitch -= (float)delta_time * pitch_speed;
    if (kb->isPressed(GLFW_KEY_L))
        sunYaw += (float)delta_time * yaw_speed;
    if (kb->isPressed(GLFW_KEY_J))
        sunYaw -= (float)delta_time * yaw_speed;

    if (kb->isPressed(GLFW_KEY_G))
        TankPosition.z -= cos(TankRotation) * (float)delta_time * 30;
    if (kb->isPressed(GLFW_KEY_T))
        TankPosition.z += cos(TankRotation) * (float)delta_time * 30;
    if (kb->isPressed(GLFW_KEY_G))
        TankPosition.x -= sin(TankRotation) * (float)delta_time * 30;
    if (kb->isPressed(GLFW_KEY_T))
        TankPosition.x += sin(TankRotation) * (float)delta_time * 30;

    if (kb->isPressed(GLFW_KEY_R))
    {
        TankRotation -= (float)delta_time;
        TankRotation = glm::wrapAngle(TankRotation);
        //TankPosition=normalize(TankPosition);
    }
    if (kb->isPressed(GLFW_KEY_Y))
    {
        TankRotation += (float)delta_time;
        TankRotation = glm::wrapAngle(TankRotation);
        //TankPosition=normalize(TankPosition);
    }

    if (sunPitch < -glm::half_pi<float>())
        sunPitch = -glm::half_pi<float>();
    if (sunPitch > glm::half_pi<float>())
        sunPitch = glm::half_pi<float>();
    sunYaw = glm::wrapAngle(sunYaw);

    // Fireeeeeeeee
    if (controller->getMouse()->justPressed(GLFW_MOUSE_BUTTON_1))
    {
        addBullet();
    }
    //move bullets
    for (size_t i = 0; i < bullets.size(); i++)
    {

        bulletsPositions[i] = bulletsPositions[i] + glm::vec3{0.06 * bulletsDirections[i][0], 0.06 * bulletsDirections[i][1], 0.06 * bulletsDirections[i][2]};
    }
    for (size_t i = 0; i < enemybullets.size(); i++)
    {
        enemybulletsPositions[i] = enemybulletsPositions[i] + glm::vec3{0.06 * enemybulletsDirections[i][0], 0, 0.06 * enemybulletsDirections[i][2]};
    }

    for (size_t i = 0; i < tanks.size(); i++)
    {
        glm::vec3 pos = tanks[i]->getPosition();
        if (sqrt(pos.x * pos.x + pos.z * pos.z) > (10 + 6 * tanks[i]->getID()))
            tanks[i]->move(delta_time * 5);
        else
        {
            if (tanks[i]->canShoot())
            {
                addEnemyBullet(tanks[i]->getPosition());
            }
            tanks[i]->decreaseShootingCounter();
        }
    }
}

void DirectionalLightScene::fight()
{
    float distance;
    glm::vec3 tp, bp;
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bp = bulletsPositions[i];
        for (size_t j = 0; j < tanks.size(); j++)
        {
            tp = tanks[j]->getPosition();
            distance = sqrt((tp.x - bp.x) * (tp.x - bp.x) + (tp.y - bp.y) * (tp.y - bp.y) + (tp.z - bp.z) * (tp.z - bp.z));
            if (distance <= 1)
            {
                if (!tanks[j]->decreaseHealth(10))
                {
                    tanks.erase(tanks.begin() + j);
                }
                bulletsPositions.erase(bulletsPositions.begin() + i);
                bullets.erase(bullets.begin() + i);
                bulletsDirections.erase(bulletsDirections.begin() + i);
                break;
            }
        }
    }
    for (size_t i = 0; i < enemybullets.size(); i++)
    {
        bp = enemybulletsPositions[i];

        distance = sqrt((-bp.x) * (-bp.x) + (-bp.z) * (-bp.z));
        if (distance <= 1)
        {
            useBlur = true;
            playerHealth -= 10;
            if (playerHealth == 0)
            {
                this->endGame();
            }
            enemybulletsPositions.erase(enemybulletsPositions.begin() + i);
            enemybullets.erase(enemybullets.begin() + i);
            enemybulletsDirections.erase(enemybulletsDirections.begin() + i);
            break;
        }
    }
}

inline glm::vec3 getTimeOfDayMix(float sunPitch)
{
    sunPitch /= glm::half_pi<float>();
    if (sunPitch > 0)
    {
        float noon = glm::smoothstep(0.0f, 0.5f, sunPitch);
        return {noon, 1.0f - noon, 0};
    }
    else
    {
        float dusk = glm::smoothstep(0.0f, 0.25f, -sunPitch);
        return {0, 1.0f - dusk, dusk};
    }
}

void DirectionalLightScene::Draw()
{
    if (useBlur)
        fbo->bind();

    if (usePixal)
        fboPixal->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear colors and depth

    glm::mat4 VP = camera->getVPMatrix();
    glm::vec3 cam_pos = camera->getPosition();
    glm::vec3 sun_direction = glm::vec3(glm::cos(sunYaw), 0, glm::sin(sunYaw)) * glm::cos(sunPitch) + glm::vec3(0, glm::sin(sunPitch), 0);

    const glm::vec3 noonSkyColor = {0.53f, 0.81f, 0.98f};
    const glm::vec3 sunsetSkyColor = {0.99f, 0.37f, 0.33f};
    const glm::vec3 duskSkyColor = {0.04f, 0.05f, 0.19f};

    const glm::vec3 noonSunColor = {0.9f, 0.8f, 0.6f};
    const glm::vec3 sunsetSunColor = {0.8f, 0.6f, 0.4f};
    const glm::vec3 duskSunColor = {0.0f, 0.0f, 0.0f};

    glm::vec3 mix = getTimeOfDayMix(sunPitch);

    glm::vec3 skyColor = mix.x * noonSkyColor + mix.y * sunsetSkyColor + mix.z * duskSkyColor;
    glm::vec3 sunColor = mix.x * noonSunColor + mix.y * sunsetSunColor + mix.z * duskSunColor;

    shader->use();
    shader->set("VP", VP);
    shader->set("cam_pos", cam_pos);
    shader->set("light.color", sunColor);
    shader->set("light.direction", -sun_direction);
    shader->set("ambient", 0.5f * skyColor);

    shader->set("material.albedo", 0);
    shader->set("material.specular", 1);
    shader->set("material.roughness", 2);
    shader->set("material.ambient_occlusion", 3);
    shader->set("material.emissive", 4);

    shader->set("material.albedo_tint", {1, 1, 1});
    shader->set("material.specular_tint", {1, 1, 1});
    shader->set("material.roughness_scale", 1.0f);
    shader->set("material.emissive_tint", {1, 1, 1});

    float emissive_power = glm::sin((float)glfwGetTime()) + 1;
    shader->set("material.emissive_tint", glm::vec3(1, 1, 1) * emissive_power);

    //Draw SkyBox
    skyShader->use();
    skyShader->set("VP", VP);
    skyShader->set("cam_pos", cam_pos);
    skyShader->set("M", glm::translate(glm::mat4(), cam_pos));
    skyShader->set("sun_direction", sun_direction);
    skyShader->set("sun_size", 0.02f);
    skyShader->set("sun_halo_size", 0.02f);
    skyShader->set("sun_brightness", 1.0f);
    skyShader->set("sun_color", sunColor);
    skyShader->set("sky_top_color", skyColor);
    skyShader->set("sky_bottom_color", 1.0f - 0.25f * (1.0f - skyColor));
    skyShader->set("sky_smoothness", 0.5f);
    glCullFace(GL_FRONT);
    sky->draw();
    glCullFace(GL_BACK);

    drawMap(VP);
    drawBullet(VP);
    drawEnemyBullet(VP);
    spawnTank();
    drawTank();
    fight();
    glm::mat4 model2_mat = glm::translate(glm::mat4(), {0.3, 6, 0}) *
                           glm::scale(glm::mat4(), glm::vec3(0.01, 0.01, 0.01));
    model2_mat = model2_mat * glm::rotate(glm::mat4(), -controller->getYaw() + glm::half_pi<float>(), {0, 1, 0});
    shader->set("M", model2_mat);
    shader->set("M_it", glm::transpose(glm::inverse(model2_mat)));

    for (int i = 0; i < 5; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        TankText[i]->bind();
    }

    if (useBlur)
    {
        // Switch back to default frame buffer
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blurCounter -= 1;
        blurredShader->use();
        fboTex->bind();
        quad->draw();
        glClear(GL_DEPTH_BUFFER_BIT);
        if (blurCounter < 0)
        {
            useBlur = false;
            blurCounter = 20;
        }
    }
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    tankMesh->draw();
    if (usePixal)
    {
        // Switch back to default frame buffer
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pixalShader->use();
        fboTexPixal->bind();
        quadPixal->draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void DirectionalLightScene::Finalize()
{
    delete controller;
    delete camera;
    delete model;
    delete sky;
    delete ground;
    for (int i = 0; i < 5; i++)
    {
        delete TankText[i];
        delete checkers[i];
    }
    delete skyShader;
    delete shader;
}

void DirectionalLightScene::drawTank()
{
    if (tanks.empty())
        return;

    shader->use();

    for (size_t i = 0; i < tanks.size(); i++)
    {
        glm::mat4 model2_mat = tanks[i]->draw();
        shader->set("M", model2_mat);
        shader->set("M_it", glm::transpose(glm::inverse(model2_mat)));
        float emissive_power = glm::sin((float)glfwGetTime()) + 1;
        shader->set("material.emissive_tint", glm::vec3(1, 1, 1) * emissive_power);
        tanks[i]->meshDraw();
    }
}

void DirectionalLightScene::spawnTank()
{
    int r = 50;
    spawnCounter++;
    if (spawnCounter % spawnDelay)
        return;
    if (spawnDelay > 120)
        spawnDelay = spawnDelay * 0.93;

    int location = genRandom();
    if (location == -1)
        return;
    tankLocations[location]++;
    double theta = 2 * 3.141529 * location / locationsCount;

    glm::vec3 pos = {r * cos(theta), 6.05, r * sin(theta)};
    Tank *spawnedTank = new Tank(pos, tankMesh, 3.141529 * 3 * 0.5 - theta, tankLocations[location]);

    tanks.push_back(spawnedTank);
}

int DirectionalLightScene::genRandom()
{
    /* initialize random seed: */
    srand(time(NULL));
    int random;
    for (int i = 0; i < 10; i++)
    {
        random = rand() % locationsCount;
        if (tankLocations[random] != maxPerColumn)
        {
            return random;
        }
    }

    return -1;
}
