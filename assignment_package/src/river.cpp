#include "river.h"
#include <math.h>
//***********************************lan lou 's L-river
#define M_PI 3.1415926535
River::River()
{
}
River::River(int x, int z, int riverType):
    origin_pos_x(x),origin_pos_z(z),River_Type(riverType),River_width(15),
    minX(0),maxX(0),minZ(0),maxZ(0)
{
    if(River_Type==2)
        Generate_Lsys_two();
    else
        Generate_Lsys_one();
}
//a generally straight L system that has no much branching in it
void River::Generate_Lsys_one()
{
    glm::vec2 origin_pos(origin_pos_x,origin_pos_z);
    int t=0;
    std::vector<Turtle_type> T_lst;
    T_lst.push_back(Fwd);
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Turn);
    T_lst.push_back(Turn);
    T_lst.push_back(Return_to_here);
    T_lst.push_back(Fwd);
    T_lst.push_back(Branching);
    T_lst.push_back(now_return);
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Fwd);
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Fwd);
    generate_river_1(T_lst,origin_pos,t);
}
//a L system which exhibits a much shorter and much more brached characterastic
void River::Generate_Lsys_two()
{
    glm::vec2 origin_pos(origin_pos_x,origin_pos_z);
    int t=0;
    std::vector<Turtle_type> T_lst;
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Branching);
    T_lst.push_back(Fwd);
    T_lst.push_back(Fwd);
    T_lst.push_back(Return_to_here);
    T_lst.push_back(Turn);
    T_lst.push_back(Branching);
    T_lst.push_back(now_return);
    T_lst.push_back(Branching);
    T_lst.push_back(Turn);
    T_lst.push_back(Turn);
    T_lst.push_back(Fwd);
    T_lst.push_back(Branching);
    T_lst.push_back(now_return);
    T_lst.push_back(Fwd);
    T_lst.push_back(Branching);
    T_lst.push_back(Turn);
    T_lst.push_back(Branching);
    T_lst.push_back(Fwd);
    T_lst.push_back(Return_to_here);
    T_lst.push_back(Turn);
    T_lst.push_back(Turn);
    T_lst.push_back(Fwd);
    T_lst.push_back(Branching);
    T_lst.push_back(Branching);
    T_lst.push_back(Fwd);
    T_lst.push_back(now_return);
    T_lst.push_back(Branching);
    T_lst.push_back(Fwd);
    T_lst.push_back(Turn);
    T_lst.push_back(Return_to_here);
    T_lst.push_back(Branching);
    T_lst.push_back(Turn);
    T_lst.push_back(Turn);
    T_lst.push_back(now_return);
    generate_river_1(T_lst,origin_pos,t);

}
void River::MakeRiver(glm::vec2 r)//change the block in designated location to water type
{
    int x=round(r[0]);
    int z=round(r[1]);
    is_river[std::make_tuple(x,0,z)]=true;
    minX=x<minX?x:minX;
    maxX=x>maxX?x:maxX;
    minZ=z<minZ?z:minZ;
    maxZ=z>maxZ?z:maxZ;

}
//take in the vector of the turtles to recursively generate the branching river
void River::generate_river_1(std::vector<Turtle_type> T_lst,glm::vec2 pos,int &Branching_times)
{
    float forwardLength;
    float angle=0.0f;
    glm::vec2 origin=pos;
    glm::vec2 returnpos;
    float returnangle;
    Branching_times++;
    for(unsigned int i=0;i<T_lst.size();i++)
    {
        Turtle_type Current_turtle=T_lst[i];
        switch (Current_turtle) {
        case Fwd:
        {
            forwardLength=rand()%20+5;
            glm::vec2 newpos;
            newpos[0]=origin[0]+cos(angle)*forwardLength;
            newpos[1]=origin[1]+sin(angle)*forwardLength;
            Compute_river_line(origin,newpos,Branching_times);
            origin=newpos;
            break;
        }
        case Branching:
        {
            int branchchance=rand()%100;
            if(branchchance>Branching_times*9)
            {
                generate_river_1(T_lst,origin,Branching_times);
            }
            break;
        }
        case Return_to_here:
        {
            returnpos=origin;
            returnangle=angle;
            break;
        }
        case now_return:
        {
            origin=returnpos;
            angle=returnangle;
            break;
        }
        case Turn:
        {
            float newangle=((float) rand() / (RAND_MAX))*60.0-30.0;
            newangle=glm::radians(newangle);
            angle=angle+newangle;
            if(angle+FLT_EPSILON>M_PI)
                angle-=M_PI;
            if(angle+FLT_EPSILON<-1*M_PI)
                angle+=M_PI;
            break;
        }
        }
    }
}

void River::generate_river_2(std::vector<Turtle_type> T_lst,glm::vec2 pos,int &Branching_times)
{
    float forwardLength;
    float angle=0.0f;
    glm::vec2 origin=pos;
    glm::vec2 returnpos;
    float returnangle;
    MakeRiver(origin);
    Branching_times++;
    for(unsigned int i=0;i<T_lst.size();i++)
    {
        Turtle_type Current_turtle=T_lst[i];
        switch (Current_turtle) {
        case Fwd:
        {
            forwardLength=rand()%8+2;
            //std::cout<<forwardLength<<std::endl;
            glm::vec2 newpos;
            newpos[0]=origin[0]+cos(angle)*forwardLength;
            newpos[1]=origin[1]+sin(angle)*forwardLength;
            Compute_river_line2(origin,newpos,Branching_times);
            origin=newpos;
            break;
        }
        case Branching:
        {
            int branchchance=rand()%100;
            if(branchchance>Branching_times*7)
            {
                generate_river_2(T_lst,origin,Branching_times);
            }
            break;
        }
        case Return_to_here:
        {
            returnpos=origin;
            returnangle=angle;
            break;
        }
        case now_return:
        {
            origin=returnpos;
            angle=returnangle;
            break;
        }
        case Turn:
        {
            float newangle=((float) rand() / (RAND_MAX))*120.0-60.0;
            newangle=glm::radians(newangle);
            angle=angle+newangle;
            if(angle+FLT_EPSILON>M_PI)
                angle-=M_PI;
            if(angle+FLT_EPSILON<-1*M_PI)
                angle+=M_PI;
            break;
        }
        }
    }
}
void River::Compute_river_line(glm::vec2 v1,glm::vec2 v2,int t)
{
    float x1=v1[0];
    float x2=v2[0];
    float y1=v1[1];
    float y2=v2[1];
    int tx,ty;
    int widthcontroller1=(rand()%4-2);
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
      if(steep)
      {
        std::swap(x1, y1);
        std::swap(x2, y2);
      }

      if(x1 > x2)
      {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }
      const float dx = x2 - x1;
      const float dy = fabs(y2 - y1);
      float error = dx / 2.0f;
      const int ystep = (y1 < y2) ? 1 : -1;
      int y = round(y1);
      const int maxX = round(x2);
      for(int x=round(x1); x<maxX; x++)
      {
            if(steep)
            {
                tx=y;
                ty=x;
            }
            else
            {
                tx=x;
                ty=y;
            }
            int widthcontroller=(rand()%2-1);
            int wd=River_width+widthcontroller+widthcontroller1-t;
            if(wd<0)
                wd=0;
            ty+=widthcontroller;
            for(int q=0;q<wd;q++)
            {
                is_river[std::make_tuple(tx,0,ty+q)]=true;
                if(tx<minX)
                    minX=tx;
                if(tx>this->maxX)
                    this->maxX=tx;
                if(ty+q<minZ)
                    minZ=ty+q;
                if(ty+q>maxZ)
                    maxZ=ty+q;
            }
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
      }
}
void River::Compute_river_line2(glm::vec2 v1,glm::vec2 v2,int t)
{
    float x1=v1[0];
    float x2=v2[0];
    float y1=v1[1];
    float y2=v2[1];
    int tx,ty;
    int widthcontroller1=(rand()%3-2);
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
      if(steep)
      {
        std::swap(x1, y1);
        std::swap(x2, y2);
      }

      if(x1 > x2)
      {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }
      const float dx = x2 - x1;
      const float dy = fabs(y2 - y1);
      float error = dx / 2.0f;
      const int ystep = (y1 < y2) ? 1 : -1;
      int y = round(y1);
      const int maxX = round(x2);
      for(int x=round(x1); x<maxX; x++)
      {

            if(steep)
            {
                tx=y;
                ty=x;
            }
            else
            {
                tx=x;
                ty=y;
            }
            int widthcontroller=(rand()%2-1);
            int wd=River_width+widthcontroller+widthcontroller1-t;
            if(wd<0)
                wd=0;
            ty+=widthcontroller;
            for(int q=0;q<wd;q++)
            {
                is_river[std::make_tuple(tx,0,ty+q)]=true;
                if(tx<minX)
                    minX=tx;
                if(tx>this->maxX)
                    this->maxX=tx;
                if(ty+q<minZ)
                    minZ=ty+q;
                if(ty+q>maxZ)
                    maxZ=ty+q;
            }
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
      }
}

void River::Get_river_bound(int &Xmin, int &Xmax, int &Zmin, int &Zmax)
{
    Xmin=minX;
    Xmax=maxX;
    Zmin=minZ;
    Zmax=maxZ;
}

