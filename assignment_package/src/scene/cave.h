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

    Cave(int x, int y, int z,float initial_h_degree);

    glm::vec3 move_cave(glm::vec3 FormerPos);
    void Get_cave_bound(int &Xmin, int &Xmax, int &Zmin, int &Zmax);
    void generate_cave();
    std::map<std::tuple<int,int,int>,bool> is_cave;
    std::map<std::tuple<int,int,int>,bool> is_lavapool;
    std::map<std::tuple<int,int,int>,int> is_ore_type;
    void createLavaPool(glm::vec3 final_pos);
//    float random(glm::vec3 st);
    float noise(glm::vec3 st);
    float fbm(glm::vec3 st);
    int cave_radius;
    int cave_length;
    int moved_length;
    float min_rand;
    float max_rand;
    int minX;
    int maxX;
    int minZ;
    int maxZ;

private:
    float Hori_degree;
    int origin_pos_x;
    int origin_pos_y;
    int origin_pos_z;

};
#endif // CAVE

