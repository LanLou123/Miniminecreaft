#version 150

in vec4 vs_Pos;
out vec4 fs_Col;
void main(void)
{
    fs_Col = vec4(1,0,0,0.5);
    gl_Position = vs_Pos;
;
}

