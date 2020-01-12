#version 330 core

in float _alpha;

void main()
{
    gl_FragColor = vec4(1.0, 0.0, 0.0, _alpha);
}
