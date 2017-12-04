#include "mygl.h"
#include <la.h>
#include <QDateTime>
#include <QApplication>
#include <QKeyEvent>
#include <QThreadPool>
#include <QRunnable>
#include <QString>
#include <QMutex>
#include <iostream>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      mp_geomCube(new Cube(this)), mp_worldAxes(new WorldAxes(this)),

      m_QuadBoard(new Quad(this)),
      mp_progLiquid(new ShaderProgram(this)),
      mp_progLambert(new ShaderProgram(this)), mp_progFlat(new ShaderProgram(this)),
      mp_camera(new Camera()), mp_terrain(new Terrain()), player1(),timecount(0), m_time(0),
      surfaceMap(new Texture(this)), normalMap(new Texture(this)), greyScaleMap(new Texture(this)),

      chunkToAdd(new std::vector<Chunk*>()), chunkMutex(new QMutex()), checkingMutex(new QMutex()),
      drawWater(false),

      concurrentThreadNums(0),

      glossPowerMap(new Texture(this)), duplicateMap(new Texture(this))

{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    add_deg = 0;
    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(width()/2 , height() / 2)));
    setCursor(c); // Make the cursor invisible
    showmouse=true;
    speed = 4.0 / 60.0;
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


    delete m_QuadBoard;

    delete surfaceMap;
    delete normalMap;
    delete greyScaleMap;

    delete glossPowerMap;

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

    //

    mp_geomCube->create();
    mp_worldAxes->create();

    m_QuadBoard->create();

    // Create and set up the diffuse shader
    mp_progLambert->create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    mp_progFlat->create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Create a new shader program, to show the water effect
    mp_progLiquid->create(":/glsl/water.vert.glsl", ":/glsl/water.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    mp_progLambert->setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);


    //mp_terrain->CreateTestScene();


    //mp_terrain->CreateTestScene();
    //mp_terrain->GenerateTerrainAt(0,0,this);

    //
<<<<<<< HEAD

=======
>>>>>>> origin/JieMeng-MultiThreading
    //
    mp_terrain->GenerateFirstTerrain(this);

 
    msec = QDateTime::currentMSecsSinceEpoch();

    surfaceMap->create(":/texture/minecraft_textures_all.png");
    surfaceMap->load(SURFACE);
    surfaceMap->bind(SURFACE);

    normalMap->create(":/texture/minecraft_normals_all.png");
    normalMap->load(NORMAL);
    normalMap->bind(NORMAL);

    greyScaleMap->create(":/texture/minecraft_textures_all_grey_grass.png");
    greyScaleMap->load(GREYSCALE);
    greyScaleMap->bind(GREYSCALE);

    glossPowerMap->create(":/texture/minecraft_textures_glosspower.png");
    glossPowerMap->load(GLOSSINESS);
    glossPowerMap->bind(GLOSSINESS);

    duplicateMap->create(":/texture/minecraft_normals_mod.png");
    duplicateMap->load(DUPL);
    duplicateMap->bind(DUPL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.

    //*mp_camera = Camera(w, h, glm::vec3(mp_terrain->dimensions.x, mp_terrain->dimensions.y * 0.75, mp_terrain->dimensions.z),
    //                   glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y / 2, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));
  

    *mp_camera = Camera(w, h, glm::vec3((mp_terrain->dimensions.x)/2.0f, (mp_terrain->dimensions.y * 0.75)/1.2f,( mp_terrain->dimensions.z)-10.0f),

                       glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y * 0.75/1.2f, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));

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
    this->Time_elapsed = delta / 1000.0f;//time(sec) elapsed since last update

    // Try to get the lock
    //bool occupied = chunkMutex->tryLock();
    // If the mutex is not locked by other threads
 //   if(m_time%2 == 0)
 //   {
    std::cout<< " checking" << isCheckingForBoundary<<std::endl;

        if(chunkMutex->tryLock())
        {
            int chunkNum = chunkToAdd->size();
            std::cout << chunkNum << std::endl;
            if(chunkNum == 0)
            {
<<<<<<< HEAD
                concurrentThreadNums = 0;
=======

                checkingMutex->lock();
                //isCheckingForBoundary = false;
                numOfThreads = 0;
                checkingMutex->unlock();

>>>>>>> origin/JieMeng-MultiThreading
                chunkMutex->unlock();
            }

            else
            {
<<<<<<< HEAD
                std::cout<<chunkNum <<std::endl;
=======
               std::cout<<chunkNum <<std::endl;

>>>>>>> origin/JieMeng-MultiThreading
                int index = chunkNum-1;
                mp_terrain->addChunk2Map((*chunkToAdd)[index]);
                ((*chunkToAdd)[index])->create();
                chunkToAdd->pop_back();
                chunkMutex->unlock();
            }
        }
  //  }

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
    mp_progLambert->setTimeCount(m_time);
    mp_progLambert->setLookVector(mp_camera->eye);

    GLDrawScene();

    ++m_time;

    glDisable(GL_DEPTH_TEST);
    mp_progFlat->setModelMatrix(glm::mat4());
    mp_progFlat->draw(*mp_worldAxes);
    glEnable(GL_DEPTH_TEST);

    if(drawWater)
    {
        mp_progLiquid->draw(*m_QuadBoard);
    }
}

void MyGL::GLDrawScene()
{
    mp_progLambert->setModelMatrix(glm::mat4(1.0f));
    for (std::pair<int64_t, Chunk*> pair : this->mp_terrain->ChunkTable)
    {
        mp_progLambert->draw(*pair.second);
    }
    for (std::pair<int64_t, Chunk*> pair : this->mp_terrain->ChunkTable)
    {
        mp_progLambert->drawF(*pair.second);
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
        speed = 8.0/60.0;//the default speed for running
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
    bool touch = false;
    bool inside = false;
    bool eye = false;
    BlockType liquid = EMPTY;
    CheckforLiquid(touch, inside, eye, liquid);


    if(inside)
    {
        speed == speed * (2.0f / 3.0f);
        player1.Swim();
    }

    else
    {
        if(player1.swimming)
        {
            player1.StopSwim();
        }
    }


    drawWater = eye;

    // drawLava = ?

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

    if(concurrentThreadNums == 0)
    {
        bool xminus = false;
        bool xplus = false;
        bool zminus = false;
        bool zplus = false;
        checkBoundBool(xminus, xplus, zminus, zplus);
        if(xminus || xplus|| zminus||zplus)
        {
            isCheckingForBoundary = true;
            ExtendBoundary(xminus, xplus, zminus, zplus);
        }
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


void MyGL::RayCubeIntersection(glm::vec3 cubeCenter, float &tNear, float &tFar)
{
    float xMin = cubeCenter[0] - 0.5f;
    float xMax = cubeCenter[0] + 0.5f;
    float yMin = cubeCenter[1] - 0.5f;
    float yMax = cubeCenter[1] + 0.5f;
    float zMin = cubeCenter[2] - 0.5f;
    float zMax = cubeCenter[2] + 0.5f;

    // backup tNear and tFar
    float tempTnear = tNear;
    float tempTfar = tFar;

    glm::vec3 r0 = mp_camera->eye;
    glm::vec3 rd = mp_camera->eye + 3.f * glm::normalize(mp_camera->ref - mp_camera->eye);

    float t0 = 0.f;
    float t1 = 0.f;

    float x0 = r0[0];
    float xd = rd[0];
    if(glm::abs(x0 - xd) < 1e-5)//parallel with x slabs
    {
        if(x0 < xMin || x0 > xMax)//miss
        {
            return;
        }
    }
    else
    {
        t0 = (xMin - x0)/(xd - x0);
        t1 = (xMax - x0)/(xd - x0);
        if(t0 > t1)
        {
            std::swap(t0, t1);
        }
        if(t0 > tNear)
        {
            tNear = t0;
        }
        if(t1 < tFar)
        {
            tFar = t1;
        }
    }

    float y0 = r0[1];
    float yd = rd[1];
    if(glm::abs(y0 - yd) < 1e-5)//parallel with y slabs
    {
        if(y0 < yMin || y0 > yMax)//miss
        {
            tNear = tempTnear;
            tFar = tempTfar;
            return;
        }
    }
    else
    {
        t0 = (yMin - y0)/(yd - y0);
        t1 = (yMax - y0)/(yd - y0);
        if(t0 > t1)
        {
            std::swap(t0, t1);
        }
        if(t0 > tNear)
        {
            tNear = t0;
        }
        if(t1 < tFar)
        {
            tFar = t1;
        }
    }

    float z0 = r0[2];
    float zd = rd[2];
    if(glm::abs(z0 - zd) < 1e-5)//parallel with z slabs
    {
        if(z0 < zMin || z0 > zMax)//miss
        {
            tNear = tempTnear;
            tFar = tempTfar;
            return;
        }
    }
    else
    {
        t0 = (zMin - z0)/(zd - z0);
        t1 = (zMax - z0)/(zd - z0);
        if(t0 > t1)
        {
            std::swap(t0, t1);
        }
        if(t0 > tNear)
        {
            tNear = t0;
        }
        if(t1 < tFar)
        {
            tFar = t1;
        }
    }
}

glm::ivec3 MyGL::CubeToRemove(bool &valid)
{
    // determine the current location
    // iterate the surrounding cubes

    // Here we assume that all cubes center at integer coords

    // first find the grid location
    glm::vec3 gridLoc = glm::floor(mp_camera->eye);
    // according to the distance between this point and its floor, divide into two situations
    float distanceX = mp_camera->eye[0] - gridLoc[0];
    float distanceZ = mp_camera->eye[2] - gridLoc[2];
    glm::vec3 cubeToRemove = glm::vec3(0.f);

    bool flag_ValidCubes = false;
    if(distanceX > -1e-5 || distanceZ > -1e-5) // not standing in center of some cube, situation 1
    {
        float tNear = std::numeric_limits<float>::max();
        //float tFar = std::numeric_limits<float>::max() * (-1.f);
        cubeToRemove = glm::vec3(0.f);

        // iterate the surrounding blocks
        for(int i = -1; i < 3; i++)
        {
            for(int j = -1; j < 3; j++)
            {
                for(int k = -2; k < 2; k++)
                {
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc + glm::vec3(i * 1.f, k * 1.f, j * 1.f);

                    int x = (int)(cubeCenter[0]);
                    int y = (int)(cubeCenter[1]);
                    int z = (int)(cubeCenter[2]);
                    Chunk* ck = mp_terrain->getChunkAt(x,z);
                    if(ck != nullptr)
                    {
                        BlockType block = mp_terrain->getBlockAt(x,y,z);
                        if(block == EMPTY)
                        {
                            continue;
                        }
                    }
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempFar > -1e-5)
                    //if(tempNear < tNear && tempNear > -1e-5)
                    {
                        tNear = tempNear;
                        cubeToRemove = cubeCenter;
                        flag_ValidCubes = true;
                    }
                }
            }
        }
    }
    // else we are now standing on exactly the center of some cube
    // check for all surrounding cubes
    else
    {
        float tNear = std::numeric_limits<float>::max();
        //float tFar = std::numeric_limits<float>::max() * (-1.f);
        cubeToRemove = glm::vec3(0.f);

        // iterate the surrounding blocks
        for(int i = -2; i < 3; i++)
        {
            for(int j = -2; j < 3; j++)
            {
                for(int k = -2; k < 2; k++)
                {
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc  + glm::vec3(i * 1.f, k * 1.f, j * 1.f);

                    int x = (int)(cubeCenter[0]);
                    int y = (int)(cubeCenter[1]);
                    int z = (int)(cubeCenter[2]);
                    Chunk* ck = mp_terrain->getChunkAt(x,z);
                    if(ck != nullptr)
                    {
                        BlockType block = mp_terrain->getBlockAt(x,y,z);
                        if(block == EMPTY)
                        {
                            continue;
                        }
                    }
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempFar > -1e-5)
                    //if(tempNear < tNear && tempNear > -1e-5)
                    {
                        tNear = tempNear;
                        cubeToRemove = cubeCenter;
                        flag_ValidCubes = true;
                    }
                }
            }
        }
    }
    valid = flag_ValidCubes;

    int x = (int)(cubeToRemove[0]);
    int y = (int)(cubeToRemove[1]);
    int z = (int)(cubeToRemove[2]);
    return glm::ivec3(x,y,z);
}

glm::ivec3 MyGL::CubeToAdd(bool &valid)
{
    // determine the current location
    // iterate the surrounding cubes

    // Here we assume that all cubes center at integer coords

    // first find the grid location
    glm::vec3 gridLoc = glm::floor(mp_camera->eye);
    // according to the distance between this point and its floor, divide into two situations
    float distanceX = mp_camera->eye[0] - gridLoc[0];
    float distanceZ = mp_camera->eye[2] - gridLoc[2];
    glm::vec3 cubeToAdd = glm::vec3(0.f);

    bool flag_ValidCubes = false;
    if(distanceX > -1e-5 || distanceZ > -1e-5) // not standing in center of some cube, situation 1
    {

        float tNear = std::numeric_limits<float>::max();
        //float tFar = std::numeric_limits<float>::max() * (-1.f);
        cubeToAdd = glm::vec3(0.f);

        // iterate the surrounding blocks
        for(int i = -1; i < 3; i++)
        {
            for(int j = -1; j < 3; j++)
            {
                for(int k = -2; k < 2; k++)
                {
                    // center blocks, ignore
                    if(i > -1 && i < 2
                            && j > -1 && j < 2
                            && k > -2 && k < 1)
                    {
                        continue;
                    }
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc+ glm::vec3(i * 1.f, k * 1.f, j * 1.f);

                    int x = (int)(cubeCenter[0]);
                    int y = (int)(cubeCenter[1]);
                    int z = (int)(cubeCenter[2]);
                    Chunk* ck = mp_terrain->getChunkAt(x,z);
                    if(ck != nullptr)
                    {
                        BlockType block = mp_terrain->getBlockAt(x,y,z);
                        if(block == EMPTY)
                        {
                            continue;
                        }
                    }
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempFar > -1e-5)
                    //if(tempNear < tNear && tempNear > -1e-5)
                    {
                        // Find which face the intersection is on
                        // First get the intersection point
                        glm::vec3 r0 = mp_camera->eye;
                        glm::vec3 rd = mp_camera->eye + 3.f * glm::normalize(mp_camera->ref - mp_camera->eye);
                        glm::vec3 intersection = r0 + tempNear * (rd - r0);
                        // Find which face its on

                        if(glm::abs(glm::abs(intersection[0] - cubeCenter[0]) - 0.5f) < 1e-3)//intersect on some  x slab
                        {
                            if(intersection[0] < cubeCenter[0])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(1.f, 0.f, 0.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(1.f, 0.f, 0.f);
                            }
                        }
                        else if(glm::abs(glm::abs(intersection[1] - cubeCenter[1]) - 0.5f) < 1e-3)//intersect on some  y slab
                        {
                            if(intersection[1] < cubeCenter[1])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(0.f, 1.f, 0.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(0.f, 1.f, 0.f);
                            }
                        }
                        else //intersect on some  z slab
                        {
                            if(intersection[2] < cubeCenter[2])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(0.f, 0.f, 1.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(0.f, 0.f, 1.f);
                            }
                        }
                        if(       (glm::abs(cubeToAdd[0] - gridLoc[0])<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2])<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1])<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0] -1.f)<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2])<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1])<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0])<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2] - 1.f)<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1])<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0] -1.f)<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2] - 1.f)<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1])<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0])<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2])<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1] + 1.f)<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0] -1.f)<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2])<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1] + 1.f)<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0])<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2] - 1.f)<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1] + 1.f)<1e-5)
                                ||(glm::abs(cubeToAdd[0] - gridLoc[0] -1.f)<1e-5 && glm::abs(cubeToAdd[2] - gridLoc[2] - 1.f)<1e-5 && glm::abs(cubeToAdd[1] - gridLoc[1] + 1.f)<1e-5)
                                )
                        {
                            continue;
                        }
                        int x = (int)(cubeToAdd[0]);
                        int y = (int)(cubeToAdd[1]);
                        int z = (int)(cubeToAdd[2]);
                        if(mp_terrain->getBlockAt(x,y,z) != EMPTY)
                        {
                            continue;
                        }
                        tNear = tempNear;

                        flag_ValidCubes = true;
                    }
                }
            }
        }
    }
    // else we are now standing on exactly the center of some cube
    // check for all surrounding cubes
    else
    {
        float tNear = std::numeric_limits<float>::max();
        //float tFar = std::numeric_limits<float>::max() * (-1.f);
        cubeToAdd = glm::vec3(0.f);

        // iterate the surrounding blocks
        for(int i = -2; i <= 3; i++)
        {
            for(int j = -2; j <= 3; j++)
            {
                for(int k = -2; k <= 2; k++)
                {
                    // center blocks, ignore
                        if(i == 0
                                && j == 0
                                && k > -2 && k < 1)
                        {
                            continue;
                        }
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc + glm::vec3(i * 1.f, k * 1.f, j * 1.f);

                    int x = (int)(cubeCenter[0]);
                    int y = (int)(cubeCenter[1]);
                    int z = (int)(cubeCenter[2]);
                    Chunk* ck = mp_terrain->getChunkAt(x,z);
                    if(ck != nullptr)
                    {
                        BlockType block = mp_terrain->getBlockAt(x,y,z);
                        if(block == EMPTY)
                        {
                            continue;
                        }
                    }
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempFar > -1e-5)
                    //if(tempNear < tNear && tempNear > -1e-5)
                    {
                        // Find which face the intersection is on
                        // First get the intersection point
                        glm::vec3 r0 = mp_camera->eye;
                        glm::vec3 rd = mp_camera->eye + 3.f * glm::normalize(mp_camera->ref - mp_camera->eye);
                        glm::vec3 intersection = r0 + tempNear * (rd - r0);
                        // Find which face its on
                        float xOnFace = (int)(intersection[0]) + 0.5f;
                        float yOnFace = (int)(intersection[1]) + 0.5f;
                        float zOnFace = (int)(intersection[2]) + 0.5f;
                        if(glm::abs(intersection[0] - xOnFace) < 1e-3)//intersect on some  x slab
                        {
                            if(intersection[0] < cubeCenter[0])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(1.f, 0.f, 0.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(1.f, 0.f, 0.f);
                            }
                        }
                        else if(glm::abs(intersection[1] - yOnFace) < 1e-3)//intersect on some  y slab
                        {
                            if(intersection[1] < cubeCenter[1])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(0.f, 1.f, 0.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(0.f, 1.f, 0.f);
                            }
                        }
                        else //intersect on some  z slab
                        {
                            if(intersection[2] < cubeCenter[2])
                            {
                                cubeToAdd = cubeCenter - glm::vec3(0.f, 0.f, 1.f);
                            }
                            else
                            {
                                cubeToAdd = cubeCenter + glm::vec3(0.f, 0.f, 1.f);
                            }
                        }
                        if(glm::length(cubeToAdd - gridLoc) < 1e-4)
                        {
                            continue;
                        }
                        int x = (int)(cubeToAdd[0]);
                        int y = (int)(cubeToAdd[1]);
                        int z = (int)(cubeToAdd[2]);
                        if(mp_terrain->getBlockAt(x,y,z) != EMPTY)
                        {
                            continue;
                        }
                        tNear = tempNear;

                        flag_ValidCubes = true;
                    }
                }
            }
        }
    }
    valid = flag_ValidCubes;

    int x = (int)(cubeToAdd[0]);
    int y = (int)(cubeToAdd[1]);
    int z = (int)(cubeToAdd[2]);
    return glm::ivec3(x,y,z);
}


void MyGL::mousePressEvent(QMouseEvent *me)
{
    if(me->button() == Qt::LeftButton)
    {


        // get the blockType at this point
        // first get the chuck at
        bool valid = false;
        glm::ivec3 cubeToOperate = CubeToRemove(valid);
        if(valid == true)
        {
            int x = cubeToOperate[0];
            int y = cubeToOperate[1];
            int z = cubeToOperate[2];

            Chunk* chunk = mp_terrain->getChunkAt(x, z);
            if(chunk != nullptr)
            {
                // if  exist a chunk, get the blockType at this position(world)
                BlockType bt = mp_terrain->getBlockAt(x,y,z);
                // if now Empty, then set it into Empty
                if(bt != EMPTY)
                {

                    chunk->destroy();
                    mp_terrain->setBlockAt(x,y,z,EMPTY);
                    chunk->create();
                    int xRemainder = x - 16 * std::floor(x / 16.0f);
                    int zRemainder = z - 16 * std::floor(z / 16.0f);
                    if(xRemainder == 15)
                    {
                        Chunk* chunk2 = mp_terrain->getChunkAt(x + 1, z);
                        chunk2->destroy();
                        chunk2->create();
                    }
                    if(xRemainder == 0)
                    {
                        Chunk* chunk2 = mp_terrain->getChunkAt(x - 1, z);
                        chunk2->destroy();
                        chunk2->create();
                    }
                    if(zRemainder == 15)
                    {
                        Chunk* chunk2 = mp_terrain->getChunkAt(x, z + 1);
                        chunk2->destroy();
                        chunk2->create();
                    }
                    if(zRemainder == 0)
                    {
                        Chunk* chunk2 = mp_terrain->getChunkAt(x, z - 1);
                        chunk2->destroy();
                        chunk2->create();
                    }
                    update();
                }
            }
        }

    }
    else if(me->button() == Qt::RightButton)
    {
        // get the blockType at this point
        // first get the chuck at
        bool valid = false;
        glm::ivec3 cubeToOperate = CubeToAdd(valid);

        if(valid == true)
        {
            int x = cubeToOperate[0];
            int y = cubeToOperate[1];
            int z = cubeToOperate[2];

            Chunk* chunk = mp_terrain->getChunkAt(x, z);
            if(chunk != nullptr)
            {
                // if  exist a chunk, get the blockType at this position(world)
                BlockType bt = mp_terrain->getBlockAt(x,y,z);

                // if now Empty, then set it into Empty
                if(bt == EMPTY)
                {

                    chunk->destroy();
                    mp_terrain->setBlockAt(x,y,z,LAVA);
                    chunk->create();

                    update();
                }
            }
        }
    }
}

void NormalizeXZ(int x, int z, int &normalX, int &normalZ)
{
    normalX = 0;
    normalZ = 0;
    if(x >= 0)
    {
        normalX = x / 64;
        normalX *= 64;
    }
    else
    {
        normalX = (- x - 1) / 64 + 1;
        normalX *= -64;
    }
    if(z >= 0)
    {
        normalZ = z / 64;
        normalZ *= 64;
    }
    else
    {
        normalZ = (- z - 1) / 64 + 1;
        normalZ *= -64;
    }
}

void MyGL::startThreads(int normalX, int normalZ)
{
<<<<<<< HEAD
    terrainGenerator1 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator2 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator3 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator4 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator5 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator6 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator7 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator8 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);

    terrainGenerator9 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator10 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator11 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator12 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator13 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator14 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator15 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);
    terrainGenerator16 = new TerrainAtBoundary(0, 0, chunkMutex, checkingMutex, chunkToAdd, mp_terrain, this, &concurrentThreadNums);

=======

    terrainGenerator1 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator2 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator3 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator4 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator5 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator6 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator7 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator8 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);

    terrainGenerator9 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this, &numOfThreads);
    terrainGenerator10 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator11 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator12 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator13 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator14 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator15 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
    terrainGenerator16 = new TerrainAtBoundary(0, 0,chunkMutex,checkingMutex, chunkToAdd, mp_terrain, this,&numOfThreads);
>>>>>>> origin/JieMeng-MultiThreading


    terrainGenerator1->setLeftBottom(normalX, normalZ);
    terrainGenerator2->setLeftBottom(normalX + 16, normalZ);
    terrainGenerator3->setLeftBottom(normalX + 32, normalZ);
    terrainGenerator4->setLeftBottom(normalX + 48, normalZ);
    terrainGenerator5->setLeftBottom(normalX, normalZ + 16);
    terrainGenerator6->setLeftBottom(normalX + 16, normalZ + 16);
    terrainGenerator7->setLeftBottom(normalX + 32, normalZ + 16);
    terrainGenerator8->setLeftBottom(normalX + 48, normalZ + 16);

    terrainGenerator9->setLeftBottom(normalX, normalZ + 32);
    terrainGenerator10->setLeftBottom(normalX + 16, normalZ + 32);
    terrainGenerator11->setLeftBottom(normalX + 32, normalZ + 32);
    terrainGenerator12->setLeftBottom(normalX + 48, normalZ + 32);
    terrainGenerator13->setLeftBottom(normalX, normalZ + 48);
    terrainGenerator14->setLeftBottom(normalX + 16, normalZ + 48);
    terrainGenerator15->setLeftBottom(normalX + 32, normalZ + 48);
    terrainGenerator16->setLeftBottom(normalX + 48, normalZ + 48);


    QThreadPool::globalInstance()->start(terrainGenerator1);
    QThreadPool::globalInstance()->start(terrainGenerator2);
    QThreadPool::globalInstance()->start(terrainGenerator3);
    QThreadPool::globalInstance()->start(terrainGenerator4);
    QThreadPool::globalInstance()->start(terrainGenerator5);
    QThreadPool::globalInstance()->start(terrainGenerator6);
    QThreadPool::globalInstance()->start(terrainGenerator7);
    QThreadPool::globalInstance()->start(terrainGenerator8);

    QThreadPool::globalInstance()->start(terrainGenerator9);
    QThreadPool::globalInstance()->start(terrainGenerator10);
    QThreadPool::globalInstance()->start(terrainGenerator11);
    QThreadPool::globalInstance()->start(terrainGenerator12);
    QThreadPool::globalInstance()->start(terrainGenerator13);
    QThreadPool::globalInstance()->start(terrainGenerator14);
    QThreadPool::globalInstance()->start(terrainGenerator15);
    QThreadPool::globalInstance()->start(terrainGenerator16);
}

void MyGL::checkBoundBool(bool &xminus, bool &xplus, bool &zminus, bool &zplus)
{
<<<<<<< HEAD
    if(concurrentThreadNums != 0)
=======
    if(numOfThreads >= 16)
>>>>>>> origin/JieMeng-MultiThreading
    {
        return;
    }
    glm::vec3 gridLoc = glm::floor(mp_camera->eye);

    // check if there exist a chunk at x direction and z direction
    int x = gridLoc[0];
    int z = gridLoc[2];

// How to use getChunkAt
    Chunk* xDirChunk = mp_terrain->getChunkAt(x + 5, z);
    Chunk* xMinusDirChunk = mp_terrain->getChunkAt(x - 5, z);
    Chunk* zDirChunk = mp_terrain->getChunkAt(x, z + 5);
    Chunk* zMinusDirChunk = mp_terrain->getChunkAt(x, z - 5);
    if(xDirChunk == nullptr)
    {
        xplus = true;
    }
    else if(zDirChunk == nullptr)
    {
        zplus = true;
    }
    else if(xDirChunk == nullptr && zDirChunk == nullptr)
    {}
    else if(xMinusDirChunk == nullptr)
    {
        xminus = true;
    }
    else if( zMinusDirChunk == nullptr)
    {
        zminus = true;
    }
    else if(xMinusDirChunk == nullptr && zMinusDirChunk == nullptr)
    {}
    return;
}

void MyGL::ExtendBoundary(bool xminus, bool xplus, bool zminus, bool zplus)
{
    //std::cout<<isCheckingForBoundary<<std::endl;

    glm::vec3 gridLoc = glm::floor(mp_camera->eye);

    // check if there exist a chunk at x direction and z direction
    int x = gridLoc[0];
    int z = gridLoc[2];

//// How to use getChunkAt
//    Chunk* xDirChunk = mp_terrain->getChunkAt(x + 5, z);
//    Chunk* xMinusDirChunk = mp_terrain->getChunkAt(x - 5, z);
//    Chunk* zDirChunk = mp_terrain->getChunkAt(x, z + 5);
//    Chunk* zMinusDirChunk = mp_terrain->getChunkAt(x, z - 5);
    if(xplus)
    {
<<<<<<< HEAD
        checkingMutex->lock();
        concurrentThreadNums++;
        checkingMutex->unlock();
=======
>>>>>>> origin/JieMeng-MultiThreading

        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x + 5, z, normalX, normalZ);
        //mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        startThreads(normalX, normalZ);
    }
    else if(zplus)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x, z + 5, normalX, normalZ);
        //mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        startThreads(normalX, normalZ);
    }
//    else if(xDirChunk == nullptr && zDirChunk == nullptr)
//    {
//        isCheckingForBoundary = true;
//        int normalX = 0;
//        int normalZ = 0;
//        NormalizeXZ(x, z + 5, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//        NormalizeXZ(x + 5, z, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//        NormalizeXZ(x + 5, z + 5, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//    }
    // Minus situation
    else if(xminus)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x - 5, z, normalX, normalZ);
        //mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        startThreads(normalX, normalZ);
    }
    else if(zminus)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x, z - 5, normalX, normalZ);
        //mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        startThreads(normalX, normalZ);
    }
//    else if(xMinusDirChunk == nullptr && zMinusDirChunk == nullptr)
//    {
//        isCheckingForBoundary = true;
//        int normalX = 0;
//        int normalZ = 0;
//        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
//        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
//    }
    //update();
}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
    {
        speed = 4.0/60.0;
    }
    else if (e->key() == Qt::Key_W)
    {
        flag_moving_forward = 0;
        walk_end();
    }
    else if (e->key() == Qt::Key_S)
    {
        flag_moving_backward = 0;
                walk_end();
    }
    else if (e->key() == Qt::Key_D)
    {
        flag_moving_right = 0;
                walk_end();
    }
    else if (e->key() == Qt::Key_A)
    {
        flag_moving_left = 0;
                walk_end();
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
    add_deg += UPdeg;
    player1.CheckRotateAboutUp(-UPdeg);
    c.setPos(mapToGlobal(QPoint(width() / 2 , height() / 2 )));
    c.setShape(Qt::BlankCursor);
    setCursor(c);
}

void MyGL::wheelEvent(QWheelEvent *e)
{}

bool isLiquid(BlockType b)
{
    if(b == WATER || b == LAVA)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MyGL::CheckforLiquid(bool &touch, bool &inside, bool &eyeGlass, BlockType &liquidType)
{

    glm::vec3 eyePos = mp_camera->eye;

//    glm::vec3 look = glm::normalize(glm::vec3(mp_camera->look[0], 0.f, mp_camera->look[2]));
//    glm::vec3 right = glm::normalize(glm::vec3(mp_camera->right[0], 0.f, mp_camera->right[2]));
//    // four foot corners
//    glm::vec3 frontleft = eyePos + glm::vec3(0.f, -1.f, 0.f) + 0.8f * look - 0.4f * right;
//    glm::vec3 frontright = eyePos + glm::vec3(0.f, -1.f, 0.f) + 0.8f * look + 0.4f * right;

//    glm::vec3 backleft = eyePos + glm::vec3(-0.5f, -1.f, -0.5f);
//    glm::vec3 backright = eyePos + glm::vec3(-0.5f, -1.f, 0.5f);
//    BlockType flBlock = mp_terrain->getBlockAt(frontleft[0], frontleft[1], frontleft[2]);
//    BlockType frBlock = mp_terrain->getBlockAt(frontright[0], frontright[1], frontright[2]);
//    BlockType blBlock = mp_terrain->getBlockAt(backleft[0], backleft[1], backleft[2]);
//    BlockType brBlock = mp_terrain->getBlockAt(backright[0], backright[1], backright[2]);
//    // ground under foot
//    glm::vec3 footBottom = eyePos - glm::vec3(0.f, 1.5001f, 0.f);
//    BlockType groundBlock = mp_terrain->getBlockAt(footBottom[0], footBottom[1], footBottom[2]);
//    if(groundBlock == LAVA || groundBlock == WATER)
//    {
//        touch = true;
//    }
//    // front and back

//    if(isLiquid(frBlock) || isLiquid(flBlock) || isLiquid(blBlock) || isLiquid(brBlock))
//    {
//        touch = true;
//    }

    // right and left


    // foot position
    glm::vec3 footFloat = eyePos - glm::vec3(0.f, 1.f, 0.f);
    glm::vec3 foot = glm::floor(footFloat);
    BlockType footBlock = mp_terrain->getBlockAt(foot[0], foot[1], foot[2]);
    if(footBlock == LAVA || footBlock == WATER)
    {
        inside = true;
        touch = true;
    }

    glm::vec3 eyePosInt = glm::floor(eyePos);
    BlockType eyeBlock = mp_terrain->getBlockAt(eyePosInt[0], eyePosInt[1], eyePosInt[2]);
    if(eyeBlock == LAVA || eyeBlock == WATER)
    {
        eyeGlass = true;
        inside = true;
        touch = true;
        liquidType = eyeBlock;
    }
}

