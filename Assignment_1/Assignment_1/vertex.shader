
/*****
*
*	Johnny Mak
*	Comp 371 - Computer Graphics
*	Assignment #1
*
*****/


#version 330 core

layout (location = 0) in vec3 position;

out vec3 colorDepth;

 uniform mat4 model;
 uniform mat4 view;
 uniform mat4 projection; 

void main() {
	colorDepth = vec3(0.0f, 0.0f, 1.0f + position.z);
	gl_Position = projection * view * model * vec4(position, 1.0f);
    
}