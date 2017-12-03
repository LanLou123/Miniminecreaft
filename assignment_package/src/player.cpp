
#include "player.h"
#include "iostream"
player::player():cam(nullptr),vertical_velocity(0),DisableFlyingCollision(0),upAngle(0),grounded(false),swimming(false)
{
    add_deg = 0;
}

void player::CheckRotateAboutRight(float deg)
{
//    add_deg+=deg;
//    std::cout<<add_deg<<std::endl;
    float temp = deg;

    cam->RotateAboutRight(deg);
    refresh(cam);
}

void player::SetMainCamera(Camera *in)
{
    Position = in->eye;
    cam = in;
}
void player::CheckRotateAboutUp(float deg)
{


    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(deg),up);
    ref = glm::vec3(rot * glm::vec4(ref - eye,1));
    ref = ref + eye;
    cam->RotateAboutUp(deg);
    refresh(cam);
}

void player::ChangeMode()
{
    DisableFlyingCollision ^= 1;
}

void player::Swim()
{
    swimming = true;
    external_force_a = - 2.0 /3.0 * gravity_acceleration;
}

void player::StopSwim()
{
    swimming = false;
    external_force_a = 0.f;
}

void player::CheckTranslateAlongLook(float amt)
{
//    if(swimming)
//    {
//        std::cout<<"w";
////        cam->TranslateAlongLook(amt);
//        cam->Translate_X_Y(amt);

//        refresh(cam);
//        return;
//    }
    refresh(cam);
    glm::vec3 character_size=glm::vec3(0.6,2,0.6);
    glm::vec3 pos1,pos2,pos3,pos4,pos5,pos6,p1,p2;
    glm::vec3 forward_v = glm::normalize(glm::vec3(look[0],0,look[2]));
    if(amt>=0)
    {
    p1=eye+amt*forward_v+0.5f*character_size[2]*forward_v+0.5f*character_size[0]*right;
    p2=eye+amt*forward_v+0.5f*character_size[2]*forward_v-0.5f*character_size[0]*right;
    }
    else
    {
        p1=eye+amt*forward_v-0.5f*character_size[2]*forward_v+0.5f*character_size[0]*right;
        p2=eye+amt*forward_v-0.5f*character_size[2]*forward_v-0.5f*character_size[0]*right;
    }
    pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos3=p1-0.25f*character_size[1]*world_up;
    pos4=p2-0.25f*character_size[1]*world_up;
    pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*world_up;
    if((input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== WATER)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== WATER)\
        ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== WATER)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== WATER)\
            ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== WATER)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== WATER)\
                ||(input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== LAVA)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== LAVA)\
                    ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== LAVA)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== LAVA)\
                        ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== LAVA)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== LAVA))
    {
        cam->Translate_X_Y(amt);
        //        cam->TranslateAlongLook(amt);
                refresh(cam);
                return;
    }

    if((input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))!= EMPTY)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))!=EMPTY)\
        ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))!= EMPTY)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))!= EMPTY)\
            ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))!= EMPTY)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))!= EMPTY))
    {

    }

    else
    {
    cam->Translate_X_Y(amt);
    //        cam->TranslateAlongLook(amt);
            refresh(cam);
    }
    }

bool player::roof_test()
{
    refresh(cam);
    glm::vec3 character_size = glm::vec3(0.6,2,0.6);
    if(vertical_velocity == 0)
        return false;
    glm::vec3 forward_v = glm::normalize(glm::vec3(look[0],0,look[2]));
    glm::vec3 pos5,pos6,pos7,pos8;
    pos5=eye+glm::vec3(0,(0.35f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
            -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
    pos6=eye+glm::vec3(0,(0.35f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
            +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
    pos7=eye+glm::vec3(0,(0.35f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
            -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
    pos8=eye+glm::vec3(0,(0.35f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
            +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;

    if((input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== WATER)|| (input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== WATER)\
        ||(input_terrain->getBlockAt(round(pos7[0]),round(pos7[1]),round(pos7[2]))== WATER)||(input_terrain->getBlockAt(round(pos8[0]),round(pos8[1]),round(pos8[2]))== WATER)\

                ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== LAVA)|| (input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== LAVA)\
                    ||(input_terrain->getBlockAt(round(pos7[0]),round(pos7[1]),round(pos7[2]))== LAVA)||(input_terrain->getBlockAt(round(pos8[0]),round(pos8[1]),round(pos8[2]))== LAVA))
    {
                return false;
    }

    if(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))!= EMPTY)
    {
        return true;

    }
    else if(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))!=EMPTY)
    {
        return true;
    }
    else if(input_terrain->getBlockAt(round(pos7[0]),round(pos7[1]),round(pos7[2]))!= EMPTY)
    {
        return true;
    }
    else if(input_terrain->getBlockAt(round(pos8[0]),round(pos8[1]),round(pos8[2]))!= EMPTY)
    {
        return true;
    }
}

bool player::bottom_test()
{
    refresh(cam);
       glm::vec3 character_size=glm::vec3(0.6,2,0.6);
       if(vertical_velocity>0)
           return false;
       glm::vec3 forward_v=glm::normalize(glm::vec3(look[0],0,look[2]));
       glm::vec3 pos1,pos2,pos3,pos4;
       pos1=eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
               -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
       pos2=eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)+(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
               +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
       pos3=eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
               -(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;
       pos4=eye-glm::vec3(0,(0.75f+BODYEDGE_ERROR)*character_size[1],0)-(0.5f-BLOCKEDGE_ERROR)*character_size[2]*forward_v\
               +(0.5f-BLOCKEDGE_ERROR)*character_size[0]*right;

       if((input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== WATER)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== WATER)\
           ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== WATER)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== WATER)\

                   ||(input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== LAVA)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== LAVA)\
                       ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== LAVA)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== LAVA))
       {
                   return false;
       }

       if(input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))!= EMPTY)
       {
           cam->TranslateAlongWorldY(round(pos1[1])+2 -cam->eye[1]);
           return true;
       }
       else if(input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))!=EMPTY)
       {
              cam->TranslateAlongWorldY(round(pos2[1])+2 -cam->eye[1]);
           return true;
       }
       else if(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))!= EMPTY)
       {
              cam->TranslateAlongWorldY(round(pos3[1])+2 -cam->eye[1]);
           return true;
       }
       else if(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))!= EMPTY)
       {
              cam->TranslateAlongWorldY(round(pos4[1])+2 -cam->eye[1]);
           return true;
       }
}

void player::get_terrain(Terrain *t)
{
    this->input_terrain=t;
}
void player::CheckTranslateAlongRight(float amt)
{
//    if(swimming)
//    {
//        cam->TranslateAlongRight(amt);
//        refresh(cam);
//    }
    refresh(cam);
    glm::vec3 pos1,pos2,pos3,pos4,pos5,pos6,p1,p2;
    glm::vec3 character_size=glm::vec3(0.6,2,0.6);
    glm::vec3 forward_v=glm::normalize(glm::vec3(look[0],0,look[2]));
    if(amt>0)
    {
        p1=eye+amt*right+0.5f*character_size[0]*right+0.5f*character_size[2]*forward_v;
        p2=eye+amt*right+0.5f*character_size[0]*right-0.5f*character_size[2]*forward_v;
    }
    else
    {
        p1=eye+amt*right-0.5f*character_size[0]*right+0.5f*character_size[2]*forward_v;
        p2=eye+amt*right-0.5f*character_size[0]*right-0.5f*character_size[2]*forward_v;
    }
    pos1=p1+(0.25f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos2=p2+(0.25f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos3=p1-0.25f*character_size[1]*world_up;
    pos4=p2-0.25f*character_size[1]*world_up;
    pos5=p1-(0.75f-BODYEDGE_ERROR)*character_size[1]*world_up;
    pos6=p2-(0.75f-BODYEDGE_ERROR)*character_size[1]*world_up;
    if((input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== WATER)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== WATER)\
        ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== WATER)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== WATER)\
            ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== WATER)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== WATER)\
                ||(input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))== LAVA)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))== LAVA)\
                    ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))== LAVA)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))== LAVA)\
                        ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))== LAVA)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))== LAVA))
    {
        cam->TranslateAlongRight(amt);
        //        cam->TranslateAlongLook(amt);
                refresh(cam);
                return;
    }
    if((input_terrain->getBlockAt(round(pos1[0]),round(pos1[1]),round(pos1[2]))!= EMPTY)|| (input_terrain->getBlockAt(round(pos2[0]),round(pos2[1]),round(pos2[2]))!=EMPTY)\
        ||(input_terrain->getBlockAt(round(pos3[0]),round(pos3[1]),round(pos3[2]))!= EMPTY)||(input_terrain->getBlockAt(round(pos4[0]),round(pos4[1]),round(pos4[2]))!= EMPTY)\
            ||(input_terrain->getBlockAt(round(pos5[0]),round(pos5[1]),round(pos5[2]))!= EMPTY)||(input_terrain->getBlockAt(round(pos6[0]),round(pos6[1]),round(pos6[2]))!= EMPTY))
    {}
    else{
        cam->TranslateAlongRight(amt);
        refresh(cam);
    }
}

void player::Jump()
{
    jump_tri = true;
    grounded = false;
    if(DisableFlyingCollision)
    {
        CheckTranslateAlongUp(0.1);
        return;
    }
    if(swimming)
    {

        vertical_velocity = 10.0f * 2.0f / 3.0f;
        return;
    }
//    if(fabs(-vertical_velocity) < 1e - 7)
//    {
//    external_force_a = 0;
    if(vertical_velocity == 0)
        vertical_velocity = 10.0f;
//    }
}

void player::Fall()
{
    refresh(cam);
    if(DisableFlyingCollision)
    {
        return;
    }
    else
    {
        if(roof_test())
        {
            if(vertical_velocity>=0)
                vertical_velocity=0;
        }
    float dis = vertical_velocity * time_step + 0.5 * (gravity_acceleration +external_force_a)*time_step*time_step;
    dis = dis>1?1:dis;
    dis = dis<-1?-1:dis;//consider wind resistance force
    cam->TranslateAlongWorldY(dis);

    if(bottom_test())
    {
        if(vertical_velocity<=0)
        vertical_velocity=0;
//        external_force_a=-gravity_acceleration;
    }

    else
    {
        vertical_velocity+=gravity_acceleration*time_step;
//        external_force_a=0;
    }
    }
}

void player::CheckTranslateAlongUp(float amt)
{
//    if(DisableFlyingCollision)
//    {
//        cam->TranslateAlongWorldY(amt);
////        cam->TranslateAlongUp(amt);
//        refresh(cam);
//        return;
//    }
//    refresh(cam);
//    glm::vec3 translation = world_up * amt;
//    glm::vec3 temp_eye = eye + translation;
//    int x = (int)(temp_eye[0]>0?temp_eye[0]:temp_eye[0]-1);
//    int y = (int)(temp_eye[1]>0?temp_eye[1]:temp_eye[1]-1);
//    int z = (int)(temp_eye[2]>0?temp_eye[2]:temp_eye[2]-1);
//    if(input_terrain->getBlockAt(x,y,z)==EMPTY)
//    {
//        cam->TranslateAlongWorldY(amt);
//     refresh(cam);
//    }
    refresh(cam);
    if(DisableFlyingCollision)
    {
        if(bottom_test())
        {
        amt = amt>0?amt:0;
        cam->TranslateAlongWorldY(amt);
        }
        else
            cam->TranslateAlongWorldY(amt);
    }
}

void player::refresh(Camera *Maincam)
{
    cam = Maincam;
    this->fovy = Maincam->fovy;
    this->width = Maincam->width;
    this->height = Maincam->height;
    this->aspect = Maincam->aspect;
    this->eye = Maincam->eye;
    this->ref = Maincam->ref;
    this->look = Maincam->look;
    this->up = Maincam->up;
    this->right = Maincam->right;
    this->world_up = Maincam->world_up;
    this->V = Maincam->V;
    this->H = Maincam->H;
}
