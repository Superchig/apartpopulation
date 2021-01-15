#version 330 core
in vec4 rgbaColor;
in vec2 texCoords;
in float texIndex;
out vec4 color;

uniform sampler2D batchQuadTextures[2];

void main()
{    
    int index = int(texIndex);
    color = rgbaColor * texture(batchQuadTextures[index], texCoords);
    // color = vec4(rgbaColor);
}
