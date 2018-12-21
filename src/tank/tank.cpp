#include "tank.h"
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mesh/mesh_utils.hpp>
#include <textures/texture_utils.hpp>

enum TextureType
{
    ALBEDO = 0,
    SPECULAR = 1,
    ROUGHNESS = 2,
    AMBIENT_OCCLUSION = 3,
    EMISSIVE = 4
};

Tank::Tank(glm::vec3 position)
{
    this->position = position;
    rotation = 0;
    health = 100;
    mesh = MeshUtils::LoadObj("assets/models/Tank_M1A1/Tank M1A1.obj");

    texture[ALBEDO] = TextureUtils::Load2DTextureFromFile("assets/textures/Metal_col.jpg");
    texture[SPECULAR] = TextureUtils::Load2DTextureFromFile("assets/textures/Metal_spc.jpg");
    texture[ROUGHNESS] = TextureUtils::Load2DTextureFromFile("assets/models/Tank_M1A1/tank4.jpg");
    texture[AMBIENT_OCCLUSION] = TextureUtils::Load2DTextureFromFile("assets/textures/Suzanne_ao.jpg");
    texture[EMISSIVE] = TextureUtils::SingleColor({0, 0, 0, 1});

    for (int i = 0; i < 5; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        texture[i]->bind();
    }
}

void Tank::setPosition(glm::vec3 position)
{
    this->position = position;
}
void Tank::decreaseHealth(double damage)
{
    this->health -= damage;
}
glm::vec3 Tank::getPosition()
{
    return position;
}
double Tank::getHealth()
{
    return health;
}
void Tank::setHealth(double health)
{
    this->health = health;
}
glm::mat4 Tank::draw()
{
    glm::mat4 model2_mat = glm::translate(glm::mat4(), {position.x, 6.05, position.z}) *
                           glm::scale(glm::mat4(), glm::vec3(0.01, 0.01, 0.01));
    model2_mat = model2_mat * glm::rotate(glm::mat4(), rotation, {0, 1, 0});
    return model2_mat;
}

void Tank::meshDraw()
{
    mesh->draw();
}