#version 330 core
out vec4 FragColor;

in vec4 rgbaColor;

void main()
{
	FragColor = rgbaColor;
}