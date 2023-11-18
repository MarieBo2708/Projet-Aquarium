#include "vector"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
class Water{
public:
    Water();
    glm::vec3 getCenter() const {return center;}
    void setCenter(glm::vec3 newCenter) {center = newCenter;}
    float getHeight() const {return height;}
    void setHeight(float newHeight) {height = newHeight;}
    float getWidth() const {return width;}
    void setWidth(float newWidth) {width = newWidth;}
    float getDepth() const {return depth;}
    void setDepth(float newDepth) {depth = newDepth;}
    long int getResolution() const {return resolution;}
    void setResolution(long int newResolution) {resolution = newResolution;}
    void resize(float h, float w, float d);
    void createAquarium();
    std::vector<glm::vec3> getSommets() {return sommets;}
    std::vector<unsigned short> getIndices() {return indices;}
    int mode = 1 ;



private:
    glm::vec3 center;
    float height;
    float width;
    float depth;
    long int resolution ;
    std::vector<glm::vec3> sommets;
    std::vector<unsigned short> indices;
};