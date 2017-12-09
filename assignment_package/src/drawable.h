#pragma once

#include <openglcontext.h>
#include <la.h>

//This defines a class which can be rendered by our shader program.
//Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
class Drawable
{
protected:
    int count;     // The number of indices stored in bufIdx.
    GLuint bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint bufPos; // A Vertex Buffer Object that we will use to store mesh vertices (vec4s)
    GLuint bufNor; // A Vertex Buffer Object that we will use to store mesh normals (vec4s)
    GLuint bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
                   // Instead, we use a uniform vec4 in the shader to set an overall color for the geometry
    GLuint bufUV; //UV buffer
    GLuint bufFlowVelocity; //Flag buffer indicating the flowing speed
    GLuint bufTangent;
    GLuint bufBiTangent;
    GLuint bufBlockType;


    int countF;
    GLuint bufIdxF;
    GLuint bufPosF;
    GLuint bufNorF;
    GLuint bufColF;
    GLuint bufUVF;
    GLuint bufFlowVelocityF;
    GLuint bufTangentF;
    GLuint bufBiTangentF;
    GLuint bufBlockTypeF;

    bool idxBound; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool posBound;
    bool norBound;
    bool colBound;
    bool uvBound;
    bool flowVelocityBound;
    bool tangentBound;
    bool bitangentBound;
    bool blockTypeBound;


    bool idxBoundF; // Set to TRUE by generateIdxF(), returned by bindIdxF().
    bool posBoundF;
    bool norBoundF;
    bool colBoundF;
    bool uvBoundF;
    bool flowVelocityBoundF;
    bool tangentBoundF;
    bool bitangentBoundF;
    bool blockTypeBoundF;

    OpenGLContext* context; // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                          // we need to pass our OpenGL context to the Drawable in order to call GL functions
                          // from within this class.

public:
    bool isCreated;

    Drawable(OpenGLContext* context);
    virtual ~Drawable();

    virtual void create() = 0; // To be implemented by subclasses. Populates the VBOs of the Drawable.
    void destroy(); // Frees the VBOs of the Drawable.

    // Getter functions for various GL data
    virtual GLenum drawMode();
    int elemCount();
    int elemCountF();

    // Call these functions when you want to call glGenBuffers on the buffers stored in the Drawable
    // These will properly set the values of idxBound etc. which need to be checked in ShaderProgram::draw()
    void generateIdx();
    void generatePos();
    void generateNor();
    void generateCol();
    void generateUV();
    void generateFlowVelocity();
    void generateTangent();
    void generateBiTangent();
    void generateBlockType();

    void generateIdxF();
    void generatePosF();
    void generateNorF();
    void generateColF();
    void generateUVF();
    void generateFlowVelocityF();
    void generateTangentF();
    void generateBiTangentF();
    void generateBlockTypeF();

    bool bindIdx();
    bool bindPos();
    bool bindNor();
    bool bindCol();
    bool bindUV();
    bool bindFlowVelocity();
    bool bindTangent();
    bool bindBiTangent();
    bool bindBlockType();

    bool bindIdxF();
    bool bindPosF();
    bool bindNorF();
    bool bindColF();
    bool bindUVF();
    bool bindFlowVelocityF();
    bool bindTangentF();
    bool bindBiTangentF();
    bool bindBlockTypeF();
};
