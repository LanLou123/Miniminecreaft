# Miniminecreaft



## River:

![](https://github.com/LanLou123/Miniminecreaft/raw/master/river.gif)

I implemented the river feature of the terrain, in short, what I need to do in order to create a realistic river shape is through L-system algorithm, the entire process can be divided into a few steps:
- build a class named turtle, turtle is our agent for generating river, you can think the track he leaves carve out the river. Inside the class I created those enum type variables:

```
{FWD,
BRANCHING,
RETURN_TO_HERE,
TURN,
NOW_RETURN
}

```
- inside the enum, each element is corresponding to different types of operations, ```FWD ```simply means turtle moving forward, ```BRANCHING ```means create a branch for the river, ```RETURN_TO_HERE``` marks the node for the turtle to reaturn in the future, this one commonly comes with ```BRANCHING``` if we want to create the dealta river scene, accordingly, ```NOW_RETURN``` stands for the "return to node" operation, with all these different ops, I created a vector to store one predefined op list, and build a member function for the class, the detail of the algorithm would be too verbose to be put here, nevertheless, there is one thing need to be mentioned: the ```BRANCHING``` op is corresponding to a recursive action in the class : the code looks like this : 

```
  case Branching:
        {
            int branchchance=rand()%100;
            if(branchchance>Branching_times*9)
            {
                generate_river_1(T_lst,origin,Branching_times);
            }
            break;
        }
```

therefore you can treat this as a mutation of common DFS method 

- Carving out the river: by now, we just finished the basic lines of river, so how to generate the realistic scene above? what I did is doing a samping around the river boundary block after a bresham rasterization operation, and to be more specific, this is a quadratic sampling, the height of the river bank is the result of doing a square of the calculated distance.

## Cave

![](https://github.com/LanLou123/Miniminecreaft/raw/master/cave.gif)

Everything is simmilar to generation of the river, except that the cave is much simpler with less operations to consider. Basically, we use the perlin noise algorithm to calculate the heading of the turtle accoring to the former position of the turtle, and doing sampling as a disc around the heading direction. 



## Thread mutex for gradual map generation

![](https://github.com/LanLou123/Miniminecreaft/raw/master/threadmutex.gif)

In the early version of our game, one major problem is the generation of the map, before we actually get to the problem, let's first take a look at the data structure of the game:

```
chunk<-->chunk<-->chunk<-->...
 /\       /\       /\
 |        |         |
 |        |         |
 \/       \/       \/
chunk<-->chunk<-->chunk-->...
 |        |         |
 |        |         |
 \/      \/        \/
 .        .         .
 .        .         .
 .        .         .
```

each chunk is consist of 64X64X256 blocks, where 64 is the width and length of the chunk, and 256 is height,  and every chunk is connected to the chunks around it using four pointers, when we are generating new map, we used to access directly the neigbeour of the current chunk, however, loading 64X64X256 cubes is no small task, the program have to first create the instance of the chunk, for the current chunk, the calculation of cube positions, and indexing of the VBOs, and rendering of the faces would certainly cost a lot of time, what we want to do now are those:

- divide the chunk into 16 parts each having a size of 16X16X256.
- inherit the Qrunable and Qmutex class.
- start n number of threads in the main class, and attach the thread to the generation op(as a inherited class, it's actually attached naturally).
- use mutex to lock one thread while it's running, unlock it when finished.
using this method, we can ensure that the entire update process are not recquired done in one frame, (which by define is 1/60s), instead, they are speperated, and this would cerntainly make map loading smoother.




## collision and player physics

- collision detection: this is done by marching the ray from four point of the player geometry(which is a box) from the camera's perspective, if within a certain range, a environment cube is detected, the player would be pushed backward in the opposite direction.
- player physics: I used a flag to record the input of the user, for instance, if user press key ```W``` then correspodinglly, the moving forward flag is labled as true, and as every frame(1/60s) there would be a checking of the flags, player would move forward the distance of 1/60XDIS_PERSEC.
