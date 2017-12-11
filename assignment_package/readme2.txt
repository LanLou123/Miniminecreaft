FP Milestone2

Lan Lou: Implemented L-system river generation.

Bowen Yang: Implemented Texturing and Texture Animation.

Jie Meng: Swimming, Multithreaded Terrain Generation, and Sandbox Mode.

Key binding:
Press W, A, S, D to move in four traditional horizontal directions, 
in the meantime , you will be able to run by holding shift, press 1, 2 to change fov, 
and press spacebar to jump in normal mode, 

press F to toggle on/off SANDBOX mode, in this mode, you can move vertically by either pressing spacebar(up) and E(down) or pressing Q(up) and E(down),
and you can also go through all blocks without collision.


Lan Lou:

1,This time I'm in charge of L-system river generation, for this part, I begin by creating a river class and a turtle enumerator and for each different operation, like turn , moveforward there is a corresponding enum type to determine what kind of operation should be applied in the river generation function in river class, I then pushed back the different operations in a turtle vector created  as a member of an river instance with a pre_arranged order, when it comes to branch generation, it will recursively use the same river generation functiton and the width of the river would diminish in a larger speed, besides I created the river shape using a enhanced bresenham's line algorithm which adjusts the river's width according to rivergeneration functions.

2,The next part involves riverside generation, I initially thought about simply hard coding , then I realized that I can get the river side altitude by sampling the block types around water blocks ,if there are less water blocks around it, it would get higher, untill there are no water blocks in a specific range, as for the river bed, it's the same theory, if there are more  blocks of water type around it, then it will sink deeper under the water.


Bowen Yang:

Bowen Yang: I was in charge of implementing OpenGL texturing. To make the alpha blending of the  
half-transparent water and lava less buggy, I used a two-pass rendering technique. 
That is to say, render opaque blocks first, and render transparent ones with another draw call
 afterwards using seperate buffers.

Implementing OpenGL texture wasn't very chanllenging, but I sure leanrt a lot from implementing 
multi-threading. Since our multi-threading progress wasn't on schedule, I participated a lot in 
implementing multi-threading as well, and removed many bugs.

Jie Meng:
1. In the swimming part, when the player is moving, in the collision detecting part I detect the type of the block that 
the player will hit, if it is liquid, then just go without collision;
If the player's feet are inside liquid, then slow down the speed and give a upward force to slow down the gravity.
2. The sandbox just means to disable all collision detect.
3. The multi-threading part is the most difficult part of my work. I created a new class inherited from QRunnable,
and pass the necessary arguments to its member function, then in the run function, this thread will generate 2 chunks
using these arguments; After that it will push this new created chunk to the new chunks' list, then in the myGl's 
timerUpdate function, it will take a new chunk from the list and create it. The logical problem is to disable the 
checking for boundary when the new threading is working, I use QThreadpool's activethread count to check if there
is threading working on something; If there is, then disable the checking for boundary. For now it works fine, but 
it still need some refinement.


