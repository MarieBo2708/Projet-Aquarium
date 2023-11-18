#include "Water.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

Water::Water()
{
    center = glm::vec3(0, 0, 0);
    height = 0.8;
    width = 1;
    depth = 1;
    resolution = 5;
}

void Water::resize(float h, float w, float d)
{
    height = h;
    width = w;
    depth = d;
}

void Water::createAquarium()
{
    float _minX = center[0] - width / 2;
    float _maxX = center[0] + width / 2;
    float _minY = center[1] - height / 2;
    float _maxY = center[1] + height / 2;
    float _minZ = center[2] - depth / 2;
    float _maxZ = center[2] + depth / 2;
    /*     5_____7
          /|    /|
         / |   / |
        1__|__3__|6
        |  4  | /
        |     |/
        0_____2
    */

    glm::vec3 sommet0 = glm::vec3(_minX, _minY, _minZ);
    glm::vec3 sommet1 = glm::vec3(_minX, _maxY, _minZ);
    glm::vec3 sommet2 = glm::vec3(_maxX, _minY, _minZ);
    glm::vec3 sommet3 = glm::vec3(_maxX, _maxY, _minZ);
    glm::vec3 sommet4 = glm::vec3(_minX, _minY, _maxZ);
    glm::vec3 sommet5 = glm::vec3(_minX, _maxY, _maxZ);
    glm::vec3 sommet6 = glm::vec3(_maxX, _minY, _maxZ);
    glm::vec3 sommet7 = glm::vec3(_maxX, _maxY, _maxZ);
    unsigned short cpt = 0;
    // for(size_t i = 0 ; i<resolution+1 ; i++){
    //     sommets.push_back(sommet0 + glm::vec3((float)i/resolution,0,0));
    //     sommets.push_back(sommet1 + glm::vec3((float)i/resolution,0,0));
    // }
    // for(size_t i = 0 ; i<resolution ; i++){
    //     indices.push_back(i*2);
    //     indices.push_back(i*2+1);
    //     indices.push_back((i+1)*2);
    //     indices.push_back(i*2+1);
    //     indices.push_back((i+1)*2+1);
    //     indices.push_back((i+1)*2);
    // }

    // for(size_t i = 0 ; i<resolution+1 ; i++){
    //     sommets.push_back(sommet2 + glm::vec3(0,0,(float)i/resolution));
    //     sommets.push_back(sommet3 + glm::vec3(0,0,(float)i/resolution));
    // }
    // size_t count = 2*(resolution+1) ;
    // for(size_t i = 0 ; i<resolution ; i++){
    //     indices.push_back(count + i*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2+1);
    //     indices.push_back(count + (i+1)*2);
    // }

    // for(size_t i = 0 ; i<resolution+1 ; i++){
    //     sommets.push_back(sommet6 - glm::vec3((float)i/resolution,0,0));
    //     sommets.push_back(sommet7 - glm::vec3((float)i/resolution,0,0));
    // }

    // count = 2*2*(resolution+1) ;
    // for(size_t i = 0 ; i<resolution ; i++){
    //     indices.push_back(count + i*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2+1);
    //     indices.push_back(count + (i+1)*2);
    // }

    // for(size_t i = 0 ; i<resolution ; i++){
    //     sommets.push_back(sommet4 - glm::vec3(0,0,(float)i/resolution));
    //     sommets.push_back(sommet5 - glm::vec3(0,0,(float)i/resolution));
    // }

    // count = 6*(resolution+1) ;
    // for(size_t i = 0 ; i<resolution-1 ; i++){
    //     indices.push_back(count + i*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2);
    //     indices.push_back(count + i*2+1);
    //     indices.push_back(count + (i+1)*2+1);
    //     indices.push_back(count + (i+1)*2);
    // }
    // indices.push_back(sommets.size()-1);
    // indices.push_back(1);
    // indices.push_back(0);

    // indices.push_back(sommets.size()-2);
    // indices.push_back(sommets.size()-1);
    // indices.push_back(0);

    // for(size_t i = 1 ; i<resolution-1 ; i++){
    //     // sommets.push_back(sommet0 - glm::vec3(0,0,(float)i/resolution));
    //     // sommets.push_back(sommet1 - glm::vec3(0,0,(float)i/resolution));
    //     for(size_t j = 1 ; j<resolution-1 ; j++){
    //         sommets.push_back(sommet0 + glm::vec3((float)j/resolution, 0, (float)i/resolution));
    //     }
    // }

    for (int i = 0; i < resolution + 1; i++)
    {
        for (int j = 0; j < resolution + 1; j++)
        {
            sommets.push_back(sommet0 + glm::vec3((float)i / resolution, 0, (float)j / resolution));
        }
    }

    size_t count = sommets.size();
    for (int i = 0; i < resolution + 1; i++)
    {
        for (int j = 0; j < resolution + 1; j++)
        {
            sommets.push_back(sommet1 + glm::vec3((float)i / resolution, 0, (float)j / resolution));
        }
    }

    // MUR DE DEVANT
    for (int i = 0; i < resolution; i++)
    {
        indices.push_back(i);
        indices.push_back(i + count);
        indices.push_back(i + 1);

        indices.push_back(i + count);
        indices.push_back(i + count + 1);
        indices.push_back(i + 1);
    }

    // MUR DU BAS
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            indices.push_back(i * (resolution + 1) + j);
            indices.push_back((i + 1) * (resolution + 1) + j);
            indices.push_back(i * (resolution + 1) + j + 1);

            indices.push_back((i + 1) * (resolution + 1) + j);
            indices.push_back((i + 1) * (resolution + 1) + j + 1);
            indices.push_back(i * (resolution + 1) + j + 1);
        }
    }

    /* // MUR DE DROITE
    for (int i = resolution; i > 0; i--)
    {
        indices.push_back((i + 1) * (resolution + 1) - 1);
        indices.push_back((i + 1) * (resolution + 1) - 1 + count);
        indices.push_back(i * (resolution + 1) - 1);

        indices.push_back((i + 1) * (resolution + 1) - 1 + count);
        indices.push_back(i * (resolution + 1) - 1 + count);
        indices.push_back(i * (resolution + 1) - 1);
    }

    // MUR DE GAUCHE
    for (int i = resolution; i > 0; i--)
    {

        indices.push_back(i * (resolution + 1));
        indices.push_back(i * (resolution + 1) + count);
        indices.push_back((i - 1) * (resolution + 1));

        indices.push_back(i * (resolution + 1) + count);
        indices.push_back((i - 1) * (resolution + 1) + count);
        indices.push_back((i - 1) * (resolution + 1));
    }

    // MUR DU HAUT
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            indices.push_back(count + i * (resolution + 1) + j);
            indices.push_back(count + (i + 1) * (resolution + 1) + j);
            indices.push_back(count + i * (resolution + 1) + j + 1);

            indices.push_back(count + (i + 1) * (resolution + 1) + j);
            indices.push_back(count + (i + 1) * (resolution + 1) + j + 1);
            indices.push_back(count + i * (resolution + 1) + j + 1);
        }
    }

    //MUR DU FOND
    for (int i = 0; i < resolution; i++)
    {
        indices.push_back(resolution * (resolution + 1) + i);
        indices.push_back(resolution * (resolution + 1) + i + count);
        indices.push_back(resolution * (resolution + 1) + i + 1);

        indices.push_back(resolution * (resolution + 1) + i + count);
        indices.push_back(resolution * (resolution + 1) + i + count + 1);
        indices.push_back(resolution * (resolution + 1) + i + 1);
    } */

    // for(int i = 0 ; i<resolution ; i++){
    //     // indices.push_back(count-i-1);
    //     // indices.push_back(count-i-1 + count);
    //     // indices.push_back(count-i-2);

    //     // indices.push_back(count-i-1 + count);
    //     // indices.push_back(count-i-2 + count);
    //     // indices.push_back(count-i-2);

    //     // indices.push_back(count-(resolution+1)*(i+1));
    //     // indices.push_back(count-(resolution+1)*(i+2) + count);
    //     // indices.push_back(count-(resolution+1)*(i+2));

    //     std::cout<<count-(resolution+1)*i<<" ; "<<count-(resolution+1)*i + count<<" ; "<<count-(resolution+1)*(i+1)<<std::endl ;

    // }
}