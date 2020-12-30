#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

out vec4 rgbaColor;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(aPos, 0.0f, 1.0f);
	rgbaColor = aColor;
}