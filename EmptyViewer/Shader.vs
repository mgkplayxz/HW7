#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 wPosition;
out vec3 wNormal;
out vec3 wColor;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(aPos, 1.0);
	wPosition = vec3(model * vec4(aPos, 1.0));
	wNormal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space
	wColor = aColor;
}