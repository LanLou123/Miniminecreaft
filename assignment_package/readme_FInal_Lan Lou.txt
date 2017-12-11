Lan lou:
--------------------------------------------------------------------------milestone 1
Game Engine Update Function and Player Physics :

1, I initially had some problem conerning how to use the player flags to determine the movement of the player, I then decided to pass the flags boolean variables first to one function inside mygl which is used to determine what kind of movement the player is going to applied, like when moving forward and moving back ward is pressed at the same time, the player will stay idle, after that, I initialized a player according to the already exisited mp_camera object, and I used the functions inside the player, including check along look, check along right, and bottom test to apply the actual movement.

2, for the intersection part, I didn't spent a lot of time, I first recorded the face toward the look direction , and using 6 points on that face, I used ray marching to determine intersection, the same gose for translate along right, for the bottom test I used the bottom face's four points as basis to do ray marching.

--------------------------------------------------------------------------milestone 2
L-System Rivers : 

1,This time I'm in charge of L-system river generation, for this part, I begin by creating a river class and a turtle enumerator and for each different operation, like turn , moveforward there is a corresponding enum type to determine what kind of operation should be applied in the river generation function in river class, I then pushed back the different operations in a turtle vector created  as a member of an river instance with a pre_arranged order, when it comes to branch generation, it will recursively use the same river generation functiton and the width of the river would diminish in a larger speed, besides I created the river shape using a enhanced bresenham's line algorithm which adjusts the river's width according to rivergeneration functions.

2,The next part involves riverside generation, I initially thought about simply hard coding , then I realized that I can get the river side altitude by sampling the block types around water blocks ,if there are less water blocks around it, it would get higher, untill there are no water blocks in a specific range, as for the river bed, it's the same theory, if there are more  blocks of water type around it, then it will sink deeper under the water.


--------------------------------------------------------------------------milestone 3
Caves , distance fog ,screen center :
1,I choosed the cave generation for the final project, because it has simmilar mechanism with the river generation, I first used std::map to store the correspondance data of world coordinates with the cave component types, but it's apparently not the best solution, ( it actually take large amount of memories to store the cave data(which is quit different with river, river would only require a 2d map structure and cave is 3d instead),I tried to change the cave data structure to enable the terrain directly alter(ie. without using map) the block types with the randomly generated cave components, it worked in the end. Besides, I used the perlin noise algorithms to generate the path of caves according to their present position , and I dig the cave between the former position and the present as a cylinder shape, I also added a large lava pool in the end of each cave, and some randomly generated iron ore and coal along the cave.
2,I also added a screencenter with a vbo for better aiming blocks.
3,I also did the distance fog, by taking the zfragment coord value to bring the fog effect.