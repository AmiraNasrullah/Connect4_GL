
in vec4 color_out;
void
main()
{
    gl_FragColor = vec4(color_out.x,color_out.y,color_out.z,1);
}
