#version 330 core

// Ouput data
out vec4 FragColor;

in vec2 texture_coord_face_aquarium;

uniform sampler2D texture_aquarium;

uniform int mode;

void main(){

        if(mode==1){
                FragColor = texture(texture_aquarium, texture_coord_face_aquarium);
        }
        else if(mode==2){
                FragColor = vec4(0.2, 0.2, 0.2, 0.2);
        }
        else if(mode==3){
                FragColor =vec4(0.2, 0.2,0.8,0.5);
        }
        

}

