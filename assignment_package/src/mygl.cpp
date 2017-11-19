#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

#include <iostream>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      mp_geomCube(new Cube(this)), mp_worldAxes(new WorldAxes(this)),
      mp_progLambert(new ShaderProgram(this)), mp_progFlat(new ShaderProgram(this)),
      mp_camera(new Camera()), mp_terrain(new Terrain())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
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

    //mp_terrain->CreateTestScene();


    //mp_terrain->CreateTestScene();
    //mp_terrain->GenerateTerrainAt(0,0,this);
    mp_terrain->GenerateFirstTerrain(this);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    //*mp_camera = Camera(w, h, glm::vec3(mp_terrain->dimensions.x, mp_terrain->dimensions.y * 0.75, mp_terrain->dimensions.z),
    //                   glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y / 2, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));
    *mp_camera = Camera(w, h, glm::vec3(mp_terrain->dimensions.x * (-0.1f), mp_terrain->dimensions.y * 0.65, mp_terrain->dimensions.z * (-0.1f)),
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
    update();
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mp_progFlat->setViewProjMatrix(mp_camera->getViewProj());
    mp_progLambert->setViewProjMatrix(mp_camera->getViewProj());

    GLDrawScene();

    glDisable(GL_DEPTH_TEST);
    mp_progFlat->setModelMatrix(glm::mat4());
    mp_progFlat->draw(*mp_worldAxes);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::GLDrawScene()
{
//    for(int x = 0; x < mp_terrain->dimensions.x; ++x)
//    {
//        for(int y = 0; y < mp_terrain->dimensions.y; ++y)
//        {
//            for(int z = 0; z < mp_terrain->dimensions.z; ++z)
//            {
//                BlockType t;
//                if((t = mp_terrain->m_blocks[x][y][z]) != EMPTY)
//                {
//                    switch(t)
//                    {
//                    case DIRT:
//                        mp_progLambert->setGeometryColor(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
//                        break;
//                    case GRASS:
//                        mp_progLambert->setGeometryColor(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
//                        break;
//                    case STONE:
//                        mp_progLambert->setGeometryColor(glm::vec4(0.5f));
//                        break;
//                    }
//                    mp_progLambert->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, y, z)));
//                    mp_progLambert->draw(*mp_geomCube);
//                }
//            }
//        }
//    }


    mp_progLambert->setModelMatrix(glm::mat4(1.0f));

    for (std::pair<int64_t, Chunk*> pair : this->mp_terrain->ChunkTable)

    {

        mp_progLambert->draw(*pair.second);

    }
}


void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        mp_camera->RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        mp_camera->RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        mp_camera->RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        mp_camera->RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        mp_camera->fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        mp_camera->fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        mp_camera->TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        mp_camera->TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        mp_camera->TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        mp_camera->TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        mp_camera->TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        mp_camera->TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        *mp_camera = Camera(this->width(), this->height());
    }
    mp_camera->RecomputeAttributes();
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

glm::ivec3 MyGL::CubeToOperate()
{
    // determine the current location
    // iterate the surrounding cubes

    // Here we assume that all cubes center at integer coords

    // first find the grid location
    glm::vec3 gridLoc = glm::floor(mp_camera->eye);
    std::cout<<mp_camera->eye[0]<<" "<<mp_camera->eye[1]<< " "<<mp_camera->eye[2]<<" "<<std::endl;
    std::cout<<gridLoc[0]<<" "<<gridLoc[1]<< " "<<gridLoc[2]<<" "<<std::endl;
    // according to the distance between this point and its floor, divide into two situations
    float distanceX = mp_camera->eye[0] - gridLoc[0];
    float distanceZ = mp_camera->eye[2] - gridLoc[2];
    glm::vec3 cubeToRemove = glm::vec3(0.f);
    if(distanceX > -1e-5 || distanceZ > -1e-5) // not standing in center of some cube, situation 1
    {
        std::cout<<"case 1"<<std::endl;
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
                    // center blocks, ignore
//                        if(i > -1 && i < 2
//                                && j > -1 && j < 2
//                                && k > -2 && k < 1)
//                        {
//                            continue;
//                        }
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc + glm::vec3(i * 1.f, k * 1.f, j * 1.f);
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempNear > -1e-5)
                    {
                        tNear = tempNear;
                        cubeToRemove = cubeCenter;
                    }
                }
            }
        }
    }
    // else we are now standing on exactly the center of some cube
    // check for all surrounding cubes
    else
    {
        std::cout<<"case 2"<<std::endl;
        float tNear = std::numeric_limits<float>::max();
        //float tFar = std::numeric_limits<float>::max() * (-1.f);
        cubeToRemove = glm::vec3(0.f);

        // iterate the surrounding blocks
        for(int i = -1; i < 2; i++)
        {
            for(int j = -1; j < 2; j++)
            {
                for(int k = -2; k < 2; k++)
                {
                    // center blocks, ignore
//                        if(i > -1 && i < 2
//                                && j > -1 && j < 2
//                                && k > -2 && k < 1)
//                        {
//                            continue;
//                        }
                    float tempNear = std::numeric_limits<float>::max() * (-1.f);
                    float tempFar = std::numeric_limits<float>::max();
                    glm::vec3 cubeCenter = gridLoc + glm::vec3(i * 1.f, k * 1.f, j * 1.f);
                    RayCubeIntersection(cubeCenter, tempNear, tempFar);
                    // if tNear > tFar, we miss the cube
                    if(tempNear > tempFar)
                    {
                        continue;
                    }
                    // else we hit the box, if its nearer than the current hit one, record its center coords and tNear.
                    if(tempNear < tNear && tempNear > -1e-5)
                    {
                        tNear = tempNear;
                        cubeToRemove = cubeCenter;
                    }
                }
            }
        }
    }
    int x = (int)(cubeToRemove[0]);
    int y = (int)(cubeToRemove[1]);
    int z = (int)(cubeToRemove[2]);
    return glm::ivec3(x,y,z);
}

void MyGL::mousePressEvent(QMouseEvent *me)
{
    if(me->button() == Qt::LeftButton)
    {


        // get the blockType at this point
        // first get the chuck at

        glm::ivec3 cubeToOperate = CubeToOperate();
        int x = cubeToOperate[0];
        int y = cubeToOperate[1];
        int z = cubeToOperate[2];
        std::cout<<x<<" "<<y<< " "<<z<<" "<<std::endl;
        //int64_t chunkX = Chunk::getChunkOrigin(x);

        //int64_t chunkZ = Chunk::getChunkOrigin(z);

        //Chunk* chunk = mp_terrain->getChunkAt(chunkX, chunkZ);
        //if(chunk != nullptr)
        //{
            // if  exist a chunk, get the blockType at this position(world)
            BlockType bt = mp_terrain->getBlockAt(x,y,z);

            std::cout<<bt<<std::endl;
            // if now Empty, then set it into Empty
            if(bt != EMPTY)
            {

//                for (std::pair<int64_t, Chunk*> pair : mp_terrain->ChunkTable)
//                {
//                    pair.second->destroy();
//                    pair.second->create();
//                }
                mp_terrain->setBlockAt(x,y,z,EMPTY);
                update();
            }
        //}
    }
    else if(me->button() == Qt::RightButton)
    {
        // get the blockType at this point
        // first get the chuck at

        glm::ivec3 cubeToOperate = CubeToOperate();
        int x = cubeToOperate[0];
        int y = cubeToOperate[1];
        int z = cubeToOperate[2];
        std::cout<<x<<" "<<y<< " "<<z<<" "<<std::endl;
        //int64_t chunkX = Chunk::getChunkOrigin(x);

        //int64_t chunkZ = Chunk::getChunkOrigin(z);

        //Chunk* chunk = mp_terrain->getChunkAt(chunkX, chunkZ);
        //if(chunk != nullptr)
        //{
            // if  exist a chunk, get the blockType at this position(world)
            BlockType bt = mp_terrain->getBlockAt(x,y,z);

            std::cout<<bt<<std::endl;
            // if now Empty, then set it into Empty
            if(bt == EMPTY)
            {

//                for (std::pair<int64_t, Chunk*> pair : mp_terrain->ChunkTable)
//                {
//                    pair.second->destroy();
//                    pair.second->create();
//                }
                mp_terrain->setBlockAt(x,y,z,LAVA);
                update();
            }
        //}
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

void MyGL::CheckForBoundary()
{
    glm::vec3 gridLoc = glm::floor(mp_camera->eye);

    // check if there exist a chunk at x direction and z direction
    int x = gridLoc[0];
    int z = gridLoc[2];

// How to use getChunkAt
    Chunk* xDirChunk = mp_terrain->getChunkAt(x + 5, z);
    Chunk* xMinusDirChunk = mp_terrain->getChunkAt(x - 5, z);
    Chunk* zDirChunk = mp_terrain->getChunkAt(x, z + 5);
    Chunk* zMinusDirChunk = mp_terrain->getChunkAt(x, z - 5);
    if(xDirChunk == nullptr && zDirChunk != nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x + 5, z, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
    if(xDirChunk != nullptr && zDirChunk == nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x, z + 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
    if(xDirChunk == nullptr && zDirChunk == nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x, z + 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        NormalizeXZ(x + 5, z, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        NormalizeXZ(x + 5, z + 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
    // Minus situation
    if(xMinusDirChunk == nullptr && zMinusDirChunk != nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x - 5, z, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
    if(xMinusDirChunk != nullptr && zMinusDirChunk == nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x, z - 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
    if(xMinusDirChunk == nullptr && zMinusDirChunk == nullptr)
    {
        int normalX = 0;
        int normalZ = 0;
        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
        NormalizeXZ(x - 5, z - 5, normalX, normalZ);
        mp_terrain->GenerateTerrainAt(normalX, normalZ, this);
    }
}
