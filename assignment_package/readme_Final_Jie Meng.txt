Jie Meng: 
Procedual FBM terrain generation, cube removing and adding;
Swimming features, sanbox mode and Multi-thread terrain generation;
Real-time shadow effect by shadow mapping.





Jie Meng : 
Implementation details:
Procedual FBM terrain generation:
For the terrain generation feature, I got some very useful information from 
"https://thebookofshaders.com/",  there I learned how to use FBM and other noise to 
generate procedural things like terrain.
Then I experimented with the scaling coefficients and OCTAVES to get satisfactory terrain form. 
After merged with the Chunk part, I use the interfaces that provides to finish the Add Terrain, 
Remove and add cube operations.
The hardest part of this feature is the functions of removing and adding a cube, when removing a cube, 
I iterate the 5x5x5 (or 4x4x4, when player stands on the center of a cube) cubes surrounding, compute the ray intersection, 
find the nearest one, which is non-EMPTY, and remove it; When add a cube, I iterate the 4x4x4 cubes surrounding, 
cast a ray on it, for each one in front, check the intesecting face to find the place where a LAVA should be added. 
And when there is not EMPTY, jump to next cube place.
Generally saying, the restrictions on adding a cube are too much considering different situations.
This is the hardest part of my first Milestone work, and I solved it just by patience.


Swimming features:
In the swimming part, when the player is moving, in the collision detecting part I detect
the type of the block that the player will hit, if it is liquid, then just go without collision;
If the player's feet are inside liquid, then slow down the speed and give a upward force to slow down the gravity.
SandBox:
The sandbox just means to disable all collision detect and gravity.
Multithreading feature:
The multi-threading part is the most difficult part of second Milestone work. 
I created a new class inherited from QRunnable,and pass the necessary arguments to its member variables, 
then in the run function, this thread will generate 2 chunks using these arguments; 
After that it will push this new created chunk to the new chunks' list, then in the myGl's 
timerUpdate function, it will take a new chunk from the list and create it. 
The logical problem is to disable the checking for boundary when the new threading is working, 
I use QThreadpool's activethread count to check if thereis threading working on something; 
If there is, then disable the checking for boundary. For now it works fine, but not runnin "seamlessly" as we need to 
update river and caves int the new generated terrain. We think that our chunk's structure 
needs some change to improve the performance again.

Real-time shadow effect by shadow mapping:
I change the original rendering process to a new two-pass render function, in the firse pass,
I created a new Framebuffer and bind to it; Then I render a orthogonal image from the light's
direction, this image will be render into the previous framebuffer, then I generate a 
texture and render this frame into this texture. In the second render pass, I use the texture
containing the shadow map, and render from the camera's view point. During this pass, I generally
change the lambert shader so that It receive the view projection matrix from the light's direction
then transform the vertex coordinates into light space and compare its depth with the depth
value inside the shadow map texture. If its depth is smaller then this fragment is in light,
otherwise in shadow. 
In order to make the light real-time, I change the light direction and origin according to the 
current time of the system; As the light's bounding box is limited, I let the light origin 
moving with the player so that we can always see the shadow effect in the camera view.
The shadow is acctually only the basic shadow mapping, with no fancy effect. But after all, it works!
By the way, it's worth mention that a big part of my work in this week is to fix the bugs in previous
version.
Also, for some reason that, on Windows10, if you run our program with NVIDIA high performance 
GPU, the shadow effect just disappear. We just don't know why. So please run the program 
with like Intel's integrated graphic card and enjoy the shadow effect, thank you!!
 
