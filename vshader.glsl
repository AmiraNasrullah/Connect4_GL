#version 130
attribute vec4 vPosition;

in vec4 vColor;

out vec4 color_out;

void
main()
{
    gl_Position = vPosition;
    color_out = vColor;
}
