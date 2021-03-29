#version 140

uniform sampler2D texture;
uniform float time;

void main() {
    vec2 textureCoordinates = gl_TexCoord[0].xy;
    float alpha = texture2D(texture, textureCoordinates).a;

    vec3 color = texture2D(texture, textureCoordinates).rgb + vec3(0.0f, sin(time / 4.0f) * 0.1f + 0.1f, sin(time / 4.0f) * 0.2f + 0.1f);
    gl_FragColor = vec4(color / 1.5f, alpha);
}
