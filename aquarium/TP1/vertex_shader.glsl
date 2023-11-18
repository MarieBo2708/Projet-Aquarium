#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
// Uniform transformations matrices
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


void main(){

        // TODO : Output position of the vertex, in clip space : MVP * position
        vec3 pos = vec3(vertices_position_modelspace.x, vertices_position_modelspace.y, vertices_position_modelspace.z);
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(pos, 1.0);

}


