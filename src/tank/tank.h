#define GLM_FORCE_CXX11
#include <glm/glm.hpp>
#include <mesh/mesh.hpp>
#include <textures/texture2d.hpp>

class Tank
{
  private:
    double health;
    glm::vec3 position;
    float rotation;
    Mesh* mesh;
    Texture2D *tex;

  public:
    Tank(glm::vec3 position, Mesh*, Texture2D*, float theta);
    void setPosition(glm::vec3 position);
    void rotate(float theta);
    void decreaseHealth(double damage);
    glm::vec3 getPosition();
    double getHealth();
    void setHealth(double health);
    glm::mat4 draw();
    void meshDraw();
};