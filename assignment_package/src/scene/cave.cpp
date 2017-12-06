
#include "cave.h"
#include <math.h>

#define M_PI 3.1415926535

Cave::Cave()
{}
Cave::Cave(int x, int y, int z)
{
    origin_pos_x = x;
    origin_pos_y = y;
    origin_pos_z = z;
}
