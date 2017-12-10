#version 150

in vec4 vs_Pos;
out vec4 fs_Col;
void main(void)
{
    fs_Col = vec4(0,0,1,0.3);
    gl_Position = vs_Pos;
;
}

