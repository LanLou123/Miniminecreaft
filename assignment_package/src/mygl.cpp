#include "mygl.h"
#include <la.h>
#include <QDateTime>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      mp_geomCube(new Cube(this)), mp_worldAxes(new WorldAxes(this)),
      mp_progLambert(new ShaderProgram(this)), mp_progFlat(new ShaderProgram(this)),
      mp_camera(new Camera()), mp_terrain(new Terrain()), player1(),timecount(0)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(width()/2 , height() / 2)));
    setCursor(c); // Make the cursor invisible
    showmouse=true;
    speed = 15.0 / 60.0;
    flag_moving_forward = 0;
    flag_moving_backward = 0;
    flag_moving_right = 0;
    flag_moving_left = 0;
    flag_moving_up = 0;
    flag_moving_down = 0;
    flag_rotate_right = 0;
    flag_rotate_left = 0;
    flag_rotate_up = 0;
    flag_rotate_down = 0;
    flag_rotate_right = 0;
    flag_walking = 0;

    player1.SetMainCamera(mp_camera);
    player1.get_terrain(mp_terrain);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    mp_geomCube->destroy();

    delete mp_geomCube;
    delete mp_worldAxes;
    delete mp_progLambert;
    delete mp_progFlat;
    delete mp_camera;
    delete mp_terrain;
}


void MyGL::MoveMouseToCenter()
{
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    mp_geomCube->create();
    mp_worldAxes->create();

    // Create and set up the diffuse shader
    mp_progLambert->create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    mp_progFlat->create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    mp_progLambert->setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    mp_terrain->CreateTestScene();
    msec = QDateTime::currentMSecsSinceEpoch();
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    *mp_camera = Camera(w, h, glm::vec3((mp_terrain->dimensions.x)/2.0f, (mp_terrain->dimensions.y * 0.75)/1.2f,( mp_terrain->dimensions.z)-10.0f),
                       glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y / 2, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));

    glm::mat4 viewproj = mp_camera->getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    mp_progLambert->setViewProjMatrix(viewproj);
    mp_progFlat->setViewProjMatrix(viewproj);

    printGLErrorLog();
}


// MyGL's constructor links timerUpdate() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to use timerUpdate
void MyGL::timerUpdate()
{

    int64_t m = QDateTime::currentMSecsSinceEpoch();

    int64_t delta = m - msec;
    this->Time_elapsed += delta / 1000.0f;//time(sec) elapsed since last update
    update();
    moving();
    player1.Fall();
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mp_progFlat->setViewProjMatrix(mp_camera->getViewProj());
    mp_progLambert->setViewProjMatrix(mp_camera->getViewProj() );

    GLDrawScene();

    glDisable(GL_DEPTH_TEST);
    mp_progFlat->setModelMatrix(glm::mat4());
    mp_progFlat->draw(*mp_worldAxes);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::GLDrawScene()
{
    for(int x = 0; x < mp_terrain->dimensions.x; ++x)
    {
        for(int y = 0; y < mp_terrain->dimensions.y; ++y)
        {
            for(int z = 0; z < mp_terrain->dimensions.z; ++z)
            {
                BlockType t;
                if((t = mp_terrain->m_blocks[x][y][z]) != EMPTY)
                {
                    switch(t)
                    {
                    case DIRT:
                        mp_progLambert->setGeometryColor(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        break;
                    case GRASS:
                        mp_progLambert->setGeometryColor(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        break;
                    case STONE:
                        mp_progLambert->setGeometryColor(glm::vec4(0.5f));
                        break;
                    }
                    mp_progLambert->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, y, z)));
                    mp_progLambert->draw(*mp_geomCube);
                }
            }
        }
    }
}

//press W, A, S, D to move in four traditional horizontal directions, in the meantime
//, you will be able to run by holding shift, press 1, 2 to change fov,
//and press spacebar to jump in normal mode, press F to toggle on/off flying mode,
//in this mode, you can move vertically by either pressing spacebar(up) and E(down) or pressing Q(up)
//and E(down)--lan lou player part in milestone 1
void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
        speed = 55.0/60.0;//the default speed for running
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        flag_rotate_right = 1;
    } else if (e->key() == Qt::Key_Left) {
        flag_rotate_left = 1;
    } else if (e->key() == Qt::Key_Up) {
        flag_rotate_up = 1;
    } else if (e->key() == Qt::Key_Down) {
        flag_rotate_down = 1;
    } else if (e->key() == Qt::Key_1) {
        mp_camera->fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        mp_camera->fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        flag_moving_forward = 1;
        walk_begin();
    } else if (e->key() == Qt::Key_S) {
        flag_moving_backward = 1;
        walk_begin();
    } else if (e->key() == Qt::Key_D) {
        flag_moving_right = 1;
        walk_begin();
    } else if (e->key() == Qt::Key_A) {
        flag_moving_left = 1;
        walk_begin();
    } else if (e->key() == Qt::Key_Q)
    {
        flag_moving_up = 1;
    } else if (e->key() == Qt::Key_E)
    {
        flag_moving_down = 1;
    } else if (e->key() == Qt::Key_R)
    {
        *mp_camera = Camera(this->width(), this->height());
    }
    else if (e->key() == Qt::Key_Space)
    {
        flag_moving_up = 1;
        player1.Jump();
    }
    else if (e->key() == Qt::Key_F)
    {
        player1.ChangeMode();
    }
    mp_camera->RecomputeAttributes();
    update();
}
void MyGL::moving()
{
    if(flag_moving_backward&&flag_moving_forward)
    {}
    else if(flag_moving_forward)
    {

        player1.CheckTranslateAlongLook(speed);
    }
    else if(flag_moving_backward)
    {
        player1.CheckTranslateAlongLook(-speed);
    }
    if(flag_moving_left && flag_moving_right)
    {}
    else if(flag_moving_left)
    {
        player1.CheckTranslateAlongRight(-speed);
    }
    else if(flag_moving_right)
    {
        player1.CheckTranslateAlongRight(speed);
    }
    if (flag_moving_down && flag_moving_up)
    {}
    else if (flag_moving_up)
    {
        player1.CheckTranslateAlongUp(speed);
    }
    else if (flag_moving_down)
    {
        player1.CheckTranslateAlongUp(-speed);
    }
}
void MyGL::walk_begin()
{
    flag_walking = 1;
}
void MyGL::walk_end()
{
    if(flag_walking == 1)
        flag_walking = 0;
}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
    {
        speed = 15.0/60.0;
    }
    else if (e->key() == Qt::Key_W)
    {
        flag_moving_forward = 0;
    }
    else if (e->key() == Qt::Key_S)
    {
        flag_moving_backward = 0;
    }
    else if (e->key() == Qt::Key_D)
    {
        flag_moving_right = 0;
    }
    else if (e->key() == Qt::Key_A)
    {
        flag_moving_left = 0;
    }
    else if (e->key() == Qt::Key_Right)
    {
        flag_rotate_right = 0;
    }
    else if (e->key() == Qt::Key_Left)
    {
        flag_rotate_left = 0;
    }
    else if (e->key() == Qt::Key_Up)
    {
        flag_rotate_up = 0;
    }
    else if (e->key() == Qt::Key_Down)
    {
        flag_rotate_down = 0;
    }
    else if (e->key() == Qt::Key_Space)
    {
        flag_moving_up = 0;
    }
    else if (e->key() == Qt::Key_Q)
    {
        flag_moving_up = 0;
    }
    else if (e->key() == Qt::Key_E)
    {
        flag_moving_down = 0;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    if (!showmouse) return;
    double m_speed = 40;
    QCursor c = cursor();
    QPoint n=e->pos();
    float dx = n.x() - width() / 2;
    float dy = n.y() - height() / 2;
    float UPdeg = m_speed * dx / width();
    float RIGHTdeg = m_speed * dy / height();
    player1.CheckRotateAboutRight(-RIGHTdeg);
    player1.CheckRotateAboutUp(-UPdeg);
    c.setPos(mapToGlobal(QPoint(width() / 2 , height() / 2 )));
    c.setShape(Qt::BlankCursor);
    setCursor(c);
}

void MyGL::wheelEvent(QWheelEvent *e)
{}

//void MyGL::mousePressEvent(QMouseEvent *e)
//{for add and delete blocks}
