#ifndef RIVER_H
#define RIVER_H
#include <la.h>
#include <tuple>
#include <map>
#include <vector>
enum Turtle_type{Fwd ,
                 Branching,
                 Return_to_here,
                 Turn,
                 now_return
                };
class River
{
public:
    River();
    River(int x,int z,int riverType);
    void Compute_river_line(glm::vec2 v1,glm::vec2 v2,int t);
    void Compute_river_line2(glm::vec2 v1,glm::vec2 v2,int t);
    std::map<std::tuple<int,int,int>, bool> is_river;
    void MakeRiver(glm::vec2);
    void Generate_Lsys_one();
    void Generate_Lsys_two();
    void generate_river_1(std::vector<Turtle_type> rList,glm::vec2 pos,int &Branching_times);
    void generate_river_2(std::vector<Turtle_type> rList,glm::vec2 pos,int &Branching_times);
    void Get_river_bound(int &Xmin,int&Xmax,int &Zmin,int &Zmax);
private:
    int origin_pos_x;
    int origin_pos_z;
    int River_Type;   //river type: 1 or 2
    int River_width;
    int minX;
    int maxX;
    int minZ;
    int maxZ;
};

#endif // RIVER_H

