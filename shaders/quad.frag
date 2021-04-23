#version 330 core
in vec4 rgbaColor;
in vec2 texCoords;
in float texIndex;
out vec4 color;

// The size of this array is hard-coded,
// and must be manually changed if the QuadRenderer's
// corresponding constant is changed.
uniform sampler2D batchQuadTextures[32];

void main()
{
    int index = int(texIndex);
    color = rgbaColor * texture(batchQuadTextures[index], texCoords);
    // color = vec4(rgbaColor);
}
