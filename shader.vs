#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor;

// uniform float horOffset;

out vec3 ourColor;
  
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Coordinates should be in clip space
    // gl_Position = mat4(1.0f) * model * vec4(aPos, 1.0f);
    // gl_Position = projection * model * vec4(aPos, 1.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    
    // ourColor = aColor; // set ourColor to the input color we got from the vertex data
    
    ourColor = aColor;
}
