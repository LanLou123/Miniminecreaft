//#version 330

//layout (location = 0) in vec3 Position;
//layout (location = 1) in vec2 TexCoord;
//layout (location = 2) in vec3 Normal;

//in vec4 vs_Pos;             // The array of vertex positions passed to the shader

//uniform mat4 gWVP;

//uniform mat4 u_Model;       // The matrix that defines the transformation of the
//                            // object we're rendering. In this assignment,
//                            // this will be the result of traversing your scene graph.
//uniform mat4 u_View;        // The matrix that defines the camera's transformation.
//uniform mat4 u_Proj;        // The matrix that defines the camera's projection.


//out vec2 TexCoordOut;

//void main()
//{
//    gl_Position = u_Proj * u_View * u_Model * vec4(Position, 1.0);
//    TexCoordOut = TexCoord;
//}

//#version 330
////in vec4 fs_Col;

//// color to framebuffer
//out vec4 resultingColor;

//void main(void)
//{
//   // fragment shader by default is obligated to output color
//   resultingColor = vec4(1, 0, 0, 1);
//    //resultingColor = fs_Col;
//}

#version 330 core

// Ouput data
layout(location = 0) out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = gl_FragCoord.z;
}
