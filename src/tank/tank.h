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
    int ID;
    int maxBullets = 100;

  public:
    Tank(glm::vec3 position, Mesh*, float theta, int ID);
    void setPosition(glm::vec3 position);
    void rotate(float theta);
    void move(float displacement);
    bool decreaseHealth(double damage);
    int getID();
    glm::vec3 getPosition();
    double getHealth();
    void setHealth(double health);
    glm::mat4 draw();
    void meshDraw();
};