#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_face_aquarium;
layout(location = 1) in vec2 uv_face_aquarium;
layout(location = 2) in vec3 vertices_face_transparente_aquarium;
layout(location = 3) in vec3 vertices_water_aquarium;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
// Uniform transformations matrices
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform int mode;

out vec2 texture_coord_face_aquarium;


void main(){

        if(mode==1){
                texture_coord_face_aquarium = uv_face_aquarium;
                gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertices_face_aquarium.x, vertices_face_aquarium.y, vertices_face_aquarium.z, 1.0);
        }
        else if(mode==2){
                gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertices_face_transparente_aquarium.x, vertices_face_transparente_aquarium.y, vertices_face_transparente_aquarium.z, 1.0);
        }
        else if(mode==3){
                gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertices_water_aquarium.x, vertices_water_aquarium.y, vertices_water_aquarium.z, 1.0);

        }
        

}


