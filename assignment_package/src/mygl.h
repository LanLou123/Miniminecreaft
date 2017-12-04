#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include <scene/worldaxes.h>
#include "camera.h"
#include <scene/terrain.h>
#include"player.h"
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <texture.h>

#include <scene/quad.h>

#include <QMutex>

class MyGL : public OpenGLContext
{
    Q_OBJECT
private:
    Cube* mp_geomCube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    WorldAxes* mp_worldAxes; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    ShaderProgram* mp_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram* mp_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    ShaderProgram* mp_progLiquid;

    int timecount;
    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera* mp_camera;
    Terrain* mp_terrain;
    player player1;
    bool showmouse;
    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;
    int time_count;
    float Time_elapsed;
    int64_t msec;
    void MoveMouseToCenter(); // Forces the mouse position to the screen's center. You should call this
                              // from within a mouse move event after reading the mouse movement so that
                              // your mouse stays within the screen bounds and is always read.
    int m_time;



    Quad* m_QuadBoard;

    float add_deg;

    Texture *surfaceMap;
    Texture *normalMap;
    Texture *greyScaleMap;
    Texture *glossPowerMap;
    Texture *duplicateMap;

    QMutex* chunkMutex;

    QMutex* checkingMutex;

    bool drawWater;



    TerrainAtBoundary* terrainGenerator1;
    TerrainAtBoundary* terrainGenerator2;
    TerrainAtBoundary* terrainGenerator3;
    TerrainAtBoundary* terrainGenerator4;
    TerrainAtBoundary* terrainGenerator5;
    TerrainAtBoundary* terrainGenerator6;
    TerrainAtBoundary* terrainGenerator7;
    TerrainAtBoundary* terrainGenerator8;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();

    // For multi-Threading
    std::vector<Chunk*> *chunkToAdd;

    void startThreads(int normalX, int normalZ);

    void checkBoundBool(bool &xminus, bool &xplus, bool &zminus, bool &zplus);

protected:
    void keyPressEvent(QKeyEvent *e);


    void mousePressEvent(QMouseEvent *me);

    void RayCubeIntersection(glm::vec3 cubeCenter, float& tNear, float& tFar);

    glm::ivec3 CubeToRemove(bool &valid);

    glm::ivec3 CubeToAdd(bool &valid);

    void ExtendBoundary(bool xminus, bool xplus, bool zminus, bool zplus);


    bool flag_moving_forward;
    bool flag_moving_backward;
    bool flag_moving_right;
    bool flag_moving_left;
    bool flag_rotate_right;
    bool flag_rotate_left;
    bool flag_rotate_up;
    bool flag_rotate_down;
    bool flag_walking;
    bool flag_moving_up;
    bool flag_moving_down;
    float speed;
    void keyReleaseEvent(QKeyEvent *e);

    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void walk_begin();
    void walk_end();
    void moving();

    void CheckforLiquid(bool& touch, bool& inside, bool &eyeGlass, BlockType& liquidType);

private slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();
};


#endif // MYGL_H
