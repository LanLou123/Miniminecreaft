#ifndef CAVE
#define CAVE
#include <la.h>
#include <tuple>
#include <map>
#include <vector>

class Cave

{
public :
    Cave();
    Cave(int x, int y, int z);
    void generate_cave();
    void Get_cave_bound(int &Xmin, int &Xmax, int &Zmin, int &Zmax);
    std::map<std::tuple<int,int,int>,bool> is_cave;
    std::map<std::tuple<int,int,int>,bool> is_lavapool;
private:
    int origin_pos_x;
    int origin_pos_y;
    int origin_pos_z;
    int minX;
    int maxX;
    int minZ;
    int maxZ;

};
#endif // CAVE

