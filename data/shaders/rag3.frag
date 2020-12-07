uniform sampler2D texture;
uniform float time;
uniform float rag3_time;

void main() {
    vec2 textureCoordinates = gl_TexCoord[0].xy;
    float alpha = texture2D(texture, textureCoordinates).a;

    vec3 color = texture2D(texture, textureCoordinates).rgb;
    vec3 desired_color = vec3(0.9, 0.9, 0.9) - color;

    if (time < 2.0)
    {
        float w1 = time;
        color = (color * (2 - w1) + desired_color * w1) / 2.0f;
    }
    else if (time < rag3_time - 1)
    {
        color = desired_color;
    }
    else
    {
        float w1 = time - rag3_time + 1;
        color = (color * w1 + desired_color * (1 - w1));
    }

    gl_FragColor = vec4(color, alpha);
}
