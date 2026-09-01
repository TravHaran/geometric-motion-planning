uniform sampler2D source;
uniform vec2 texelStep;

void main()
{
    vec2 textureCoordinate = gl_TexCoord[0].xy;

    vec4 color = texture2D(source, textureCoordinate) * 0.227027;

    color += texture2D(
        source,
        textureCoordinate + texelStep * 1.384615
    ) * 0.316216;

    color += texture2D(
        source,
        textureCoordinate - texelStep * 1.384615
    ) * 0.316216;

    color += texture2D(
        source,
        textureCoordinate + texelStep * 3.230769
    ) * 0.070270;

    color += texture2D(
        source,
        textureCoordinate - texelStep * 3.230769
    ) * 0.070270;

    gl_FragColor = gl_Color * color;
}
