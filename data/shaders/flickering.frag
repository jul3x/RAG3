#version 120

uniform sampler2D texture;
uniform float time;

void main() {
    vec2 textureCoordinates = gl_TexCoord[0].xy;
    float alpha = texture2D(texture, textureCoordinates).a;

    vec3 color = texture2D(texture, textureCoordinates).rgb;
    color = (cos(sin(time) * 6.28) / 5.0f + 0.7f) * color;
    gl_FragColor = vec4(color, alpha);
}
