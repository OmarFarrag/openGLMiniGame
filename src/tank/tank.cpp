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

Tank::Tank(glm::vec3 position, Mesh* tankMesh, float theta, int ID)
{
    this->position = position;
    this->ID = ID;
    rotation = theta;
    health = 100;
     mesh = tankMesh;
}

void Tank::setPosition(glm::vec3 position)
{
    this->position = position;
}

bool Tank::decreaseHealth(double damage)
{
    this->health -= damage;
    if(health <= 0) return false;
    return true;
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
    glm::mat4 model2_mat = glm::translate(glm::mat4(), {position.x, position.y, position.z}) *
                           glm::scale(glm::mat4(), glm::vec3(0.01, 0.01, 0.01));
    model2_mat = model2_mat * glm::rotate(glm::mat4(), rotation, {0, 1, 0});
    return model2_mat;
}

void Tank::move(float displacement)
{
    position.z += cos(rotation) * displacement;
    position.x += sin(rotation) * displacement;
}

void Tank::meshDraw()
{
    mesh->draw();
}

int Tank::getID()
{
    return ID;
}