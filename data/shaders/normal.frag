#version 140

uniform sampler2D texture;
uniform float time;

void main() {
    vec2 textureCoordinates = gl_TexCoord[0].xy;
    gl_FragColor = texture2D(texture, textureCoordinates);
}
