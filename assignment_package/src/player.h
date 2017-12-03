#ifndef PLAYER
#define PLAYER
#include"camera.h"
#include"la.h"
#include"scene/terrain.h"


#define BODYEDGE_ERROR 0.05f
#define BLOCKEDGE_ERROR 0.02f
const float G = 9.8f / 60.0f;
const float time_step = 1.0f / 15.0f;
const float gravity_acceleration = -10.0f;

class player : public Camera
{
public:

    Camera *cam;
    float vertical_velocity;
    bool DisableFlyingCollision;
    glm::vec3 Position;
    float upAngle;
    Terrain *input_terrain;
    bool grounded = false;
    bool jump_tri = false;
    float external_force_a = 0;

    bool swimming;

    player();
    void Jump();
    void Fall();
    bool bottom_test();
    void ChangeMode();
    void refresh(Camera *Maincam);
    void CheckTranslateAlongLook(float amt);
    void CheckTranslateAlongRight(float amt);
    void CheckTranslateAlongUp(float amt);

    void SetMainCamera(Camera *in);
    void CheckRotateAboutUp(float deg);
    void CheckRotateAboutRight(float deg);
    void get_terrain(Terrain* t);

    void Swim();
    void StopSwim();
};
#endif // PLAYER

