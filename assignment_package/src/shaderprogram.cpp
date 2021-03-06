#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <QTextStream>
#include <QDebug>


ShaderProgram::ShaderProgram(OpenGLContext *context)
    : vertShader(), fragShader(), prog(),
      attrPos(-1), attrNor(-1), attrCol(-1), attrUV(-1), attrFlowVelocity(-1),
      attrTangent(-1), attrBiTangent(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1),
      unifLookVector(-1),
      context(context)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, &vertSource, 0);
    context->glShaderSource(fragShader, 1, &fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    attrUV = context->glGetAttribLocation(prog, "vs_UV");
    attrFlowVelocity = context->glGetAttribLocation(prog, "vs_FlowVelocity");
    attrTangent = context->glGetAttribLocation(prog, "vs_Tangent");
    attrBiTangent = context->glGetAttribLocation(prog, "vs_BiTangent");
    attrBlockType = context->glGetAttribLocation(prog, "vs_BlockType");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");
    unifTime       = context->glGetUniformLocation(prog, "u_Time");
    unifLookVector = context->glGetUniformLocation(prog, "u_LookVector");

    unifSamplerSurface = context->glGetUniformLocation(prog, "u_Surface");
    unifSamplerNormal = context->glGetUniformLocation(prog, "u_Normal");
    unifSamplerGreyscale = context->glGetUniformLocation(prog, "u_Greyscale");
    unifSamplerGloss = context->glGetUniformLocation(prog, "u_GlossPower");
    unifSamplerDuplicate = context->glGetUniformLocation(prog, "u_Duplicate");


    unifDimensions = context->glGetUniformLocation(prog, "u_Dimensions");
    unifEye = context->glGetUniformLocation(prog, "u_Eye");
    unifTime = context->glGetUniformLocation(prog, "u_Time");

    // for shadow map
    unifShadowViewProjMat = context->glGetUniformLocation(prog,"u_ShadowViewProjMat");
    //m_textureLocation = context->glGetUniformLocation(prog, "gShadowMap");
    unifShadowMat = context->glGetUniformLocation(prog, "u_shadowMat");
    //m_samplerLocation = context->glGetUniformLocation(prog, "gSampler");
    m_shadowMapLocation = context->glGetUniformLocation(prog, "u_shadowMap");
    // end

}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
    context->glUniformMatrix4fv(unifViewProj,
                    // How many matrices to pass
                       1,
                    // Transpose the matrix? OpenGL uses column-major, so no.
                       GL_FALSE,
                    // Pointer to the first element of the matrix
                       &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable &d)
{
    useMe();

    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrUV != -1 && d.bindUV()) {
        context->glEnableVertexAttribArray(attrUV);
        context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
    }
    if (attrFlowVelocity != -1 && d.bindFlowVelocity()) {
        context->glEnableVertexAttribArray(attrFlowVelocity);
        context->glVertexAttribPointer(attrFlowVelocity, 2, GL_FLOAT, false, 0, NULL);
    }
    if (attrTangent != -1 && d.bindTangent()) {
        context->glEnableVertexAttribArray(attrTangent);
        context->glVertexAttribPointer(attrTangent, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrBiTangent != -1 && d.bindBiTangent()) {
        context->glEnableVertexAttribArray(attrBiTangent);
        context->glVertexAttribPointer(attrBiTangent, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrBlockType != -1 && d.bindBlockType()) {
        context->glEnableVertexAttribArray(attrBlockType);
        context->glVertexAttribIPointer(attrBlockType, 1, GL_INT, 0, NULL);
    }
//    if (unifSamplerSurface != -1) context->glUniform1i(unifSamplerSurface, SURFACE);
//    if (unifSamplerNormal != -1) context->glUniform1i(unifSamplerNormal, NORMAL);
//    if (unifSamplerGreyscale != -1) context->glUniform1i(unifSamplerGreyscale, GREYSCALE);
//    if (unifSamplerGloss != -1) context->glUniform1i(unifSamplerGloss, GLOSSINESS);
//    if (unifSamplerDuplicate != -1) context->glUniform1i(unifSamplerDuplicate, DUPL);

    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);
    if (attrFlowVelocity != -1) context->glDisableVertexAttribArray(attrFlowVelocity);
    if (attrTangent != -1) context->glDisableVertexAttribArray(attrTangent);
    if (attrBiTangent != -1) context->glDisableVertexAttribArray(attrBiTangent);
    if (attrBlockType != -1) context->glDisableVertexAttribArray(attrBlockType);

    context->printGLErrorLog();
}

void ShaderProgram::drawF(Drawable &d)
{
    if (attrPos != -1 && d.bindPosF()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrNor != -1 && d.bindNorF()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrCol != -1 && d.bindColF()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrUV != -1 && d.bindUVF()) {
        context->glEnableVertexAttribArray(attrUV);
        context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
    }
    if (attrFlowVelocity != -1 && d.bindFlowVelocityF()) {
        context->glEnableVertexAttribArray(attrFlowVelocity);
        context->glVertexAttribPointer(attrFlowVelocity, 2, GL_FLOAT, false, 0, NULL);
    }
    if (attrTangent != -1 && d.bindTangentF()) {
        context->glEnableVertexAttribArray(attrTangent);
        context->glVertexAttribPointer(attrTangent, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrBiTangent != -1 && d.bindBiTangentF()) {
        context->glEnableVertexAttribArray(attrBiTangent);
        context->glVertexAttribPointer(attrBiTangent, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrBlockType != -1 && d.bindBlockTypeF()) {
        context->glEnableVertexAttribArray(attrBlockType);
        context->glVertexAttribIPointer(attrBlockType, 1, GL_INT, 0, NULL);
    }
    if (unifSamplerSurface != -1) context->glUniform1i(unifSamplerSurface, SURFACE);
    if (unifSamplerNormal != -1) context->glUniform1i(unifSamplerNormal, NORMAL);
    if (unifSamplerGreyscale != -1) context->glUniform1i(unifSamplerGreyscale, GREYSCALE);
    if (unifSamplerGloss != -1) context->glUniform1i(unifSamplerGloss, GLOSSINESS);
    if (unifSamplerDuplicate != -1) context->glUniform1i(unifSamplerDuplicate, DUPL);

    d.bindIdxF();
    context->glDrawElements(d.drawMode(), d.elemCountF(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);
    if (attrFlowVelocity != -1) context->glDisableVertexAttribArray(attrFlowVelocity);
    if (attrTangent != -1) context->glDisableVertexAttribArray(attrTangent);
    if (attrBiTangent != -1) context->glDisableVertexAttribArray(attrBiTangent);
    if (attrBlockType != -1) context->glDisableVertexAttribArray(attrBlockType);

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }
}

void ShaderProgram::setTimeCount(int time)
{
    useMe();
    if(unifTime != -1)
    {
        context->glUniform1i(unifTime, time);
    }
}

void ShaderProgram::setLookVector(glm::vec3 look)
{
    useMe();
    //glm::vec4 lookExtended = glm::vec4(look, 0.0f);
    if(unifLookVector != -1)
    {
        context->glUniform3fv(unifLookVector, 1, &look[0]);
    }
}

// For shadow mapping

void ShaderProgram::SetShadowMapView(const glm::mat4 & WVP)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifShadowViewProjMat != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifShadowViewProjMat,
                                    1,
                                    GL_FALSE,
                                    &WVP[0][0]);
    }
}

//void ShaderProgram::SetTextureUnit(unsigned int TextureUnit)
//{
//    useMe();
//    if(m_textureLocation != -1)
//    {
//        context->glUniform1i(m_textureLocation, TextureUnit);
//    }
//}

void ShaderProgram::SetShadowMat(const glm::mat4 &ShadowMat)
{
    useMe();
    if(unifShadowMat != -1){
        context->glUniformMatrix4fv(unifShadowMat,
                                    1,
                                    GL_FALSE,
                                    &ShadowMat[0][0]);

    }
}


void ShaderProgram::SetTextureUnit(uint TextureUnit)
{
    useMe();
    if(m_samplerLocation != -1)
    {
        context->glUniform1i(m_samplerLocation, TextureUnit);
    }

}


void ShaderProgram::SetShadowMapTextureUnit(uint TextureUnit)
{
    useMe();
    if(m_shadowMapLocation != -1)
    {
        context->glUniform1i(m_shadowMapLocation, TextureUnit);
    }
}
