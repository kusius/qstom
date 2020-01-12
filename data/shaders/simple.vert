#version 330 core

in vec2 position;
in float alpha;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float _alpha;
void main()
{
    _alpha = alpha;
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
}
