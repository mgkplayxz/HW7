#version 330 core
out vec4 FragColor;

in vec3 wPosition;
in vec3 wNormal;
in vec3 wColor;

vec4 Shading(vec3 wPosition, vec3 wNormal, vec3 wColor);

void main()
{
	FragColor = Shading(wPosition, wNormal, wColor);
}

vec4 Shading(vec3 wPosition, vec3 wNormal, vec3 wColor)
{
	return vec4(wColor, 1.0f);
}