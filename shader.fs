#version 330 core
out vec4 FragColor;  

in vec3 ourColor;

// uniform sampler2D texture1;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
