#version 130
attribute vec3 vPosition;

in vec4 vColor;

out vec4 color_out;
uniform mat4 matrix;
uniform mat4 rot_matrix;
uniform mat4 translate_mat;
uniform mat4 translate_back_mat;
void
main()
{
    
    gl_Position = matrix*translate_back_mat*rot_matrix * translate_mat* vec4(vPosition, 1.0);
    color_out = vColor;
}
