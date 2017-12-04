FP Milestone1

Lan Lou: Implemented game engine update function and player physics.

Bowen Yang: Implemented efficient terrain rendering and chunking.

Jie Meng: Implemented procedural terrain.

Key binding:
Press W, A, S, D to move in four traditional horizontal directions, in the meantime , you will be able to run by holding shift, press 1, 2 to change fov, and press spacebar to jump in normal mode, press F to toggle on/off flying mode, in this mode, you can move vertically by either pressing spacebar(up) and E(down) or pressing Q(up) and E(down).

Additional words from each programmers:

Bowen Yang: Implementing chunks was a challenging task indeed. Given the fact that every one of the group would be using my code, it was quite tricky to design the public interface, so that my members could use my library in a graceful manner without tampering the internal states or abusing private utility functions. With a nicely designed enclosure, we could debug our code seperately.

The performance still requires improvement, though. I've already tried to optimize the performance by using spatial locality, changing the x, y, z dimension layout sequence so that the CPU accesses adjacent data more frequently.


Jie Meng:
For the terrain generation feature, I got some very useful information from "https://thebookofshaders.com/",  there I learned how to use FBM and other noise to generate procedural things like terrain.

Then I experimented with the scaling coefficients and OCTAVES to get satisfactory terrain form. After merged with the Chunk part, I use the interfaces that provides to finish the Add Terrain, Remove and add cube operations.

The hardest part of my work is the functions of removing and adding a cube, when removing a cube, I iterate the 4x4x4 (or 3x4x3, when player stands on the center of a cube) cubes surrounding, compute the ray intersection, find the nearest one, which is non-EMPTY, and remove it; When add a cube, I iterate the 4x4x4 cubes surrounding, cast a ray on it, for each one in front, check the intesecting face to find the place where a LAVA should be added. And when there is not EMPTY, jump to next cube place.

Generally saying, the restrictions on adding a cube are too much considering different situations.

This is the hardest part of my work, and I solve it just by patience.


Lan Lou: 
1, I initially had some problem conerning how to use the player flags to determine the movement of the player, I then decided to pass the flags boolean variables first to one function inside mygl which is used to determine what kind of movement the player is going to applied, like when moving forward and moving back ward is pressed at the same time, the player will stay idle, after that, I initialized a player according to the already exisited mp_camera object, and I used the functions inside the player, including check along look, check along right, and bottom test to apply the actual movement.

2, for the intersection part, I didn't spent a lot of time, I first recorded the face toward the look direction , and using 6 points on that face, I used ray marching to determine intersection, the same gose for translate along right, for the bottom test I used the bottom face's four points as basis to do ray marching.

FP Milestone2

Lan Lou: Implemented L-system river generation.

Bowen Yang: Implemented Texturing and Texture Animation.

Jie Meng: Swimming, Multithreaded Terrain Generation, and Sandbox Mode.

Lan Lou:

1,This time I'm in charge of L-system river generation, for this part, I begin by creating a river class and a turtle enumerator and for each different operation, like turn , moveforward there is a corresponding enum type to determine what kind of operation should be applied in the river generation function in river class, I then pushed back the different operations in a turtle vector created  as a member of an river instance with a pre_arranged order, when it comes to branch generation, it will recursively use the same river generation functiton and the width of the river would diminish in a larger speed.

2,The next part involves riverside generation, I initially thought about simply hard coding , then I realized that I can get the river side altitude by sampling the block types around water blocks ,if there are less water blocks around it, it would get higher, untill there are no water blocks in a specific range, as for the river bed, it's the same theory if there are more  blocks of water type around it, then it will sink deeper under the water.
