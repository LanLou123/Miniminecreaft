BowenYang's duty in this Final Project in a nutshell:

Milestone 1:
Efficient Terrain Rendering and Chunking

Milestone 2:
Texturing and Texture Animation, assisted in debugging and optimizing multi-threading.

Milestone 3:
Day and night cycle, Biomes, and further optimizing multi-threading and chunk.

Team code optimizer and team YouTuber

_____________________________________________
What I have for milestone 1:

The implementation of the Chunk class is quite straightforward.(well, at least for this milestone. Lots of optimization was done afterwards, which would be our topic below) Since my team mates preferred traversing these blocks in x-z-y order, the index to the block at x, y, z was set to 4096 * x + 256 * z + y in order to enhance the locality. 

Every instance of Chunk came with an int_64 member indicating its left-lower corner's world coordinate, used as the origin of this Chunk. The x, z values were its upper and lower 32 bits respectively. And of course, the int_64 member xzGlobalPos was inherently hashable, and could be registered using a map.

Chunk instances also came with a pointer to its parenting Terrain. The Terrain class however, maintained an unordered map, with key being global xz coordinates, and value being a pointer to the corresponding chunk. This way, whenever Chunk class was to be created, the Chunk could tell if there were any other chunks in its vicinity using this map.

To feed the client-side buffer to GPU, more members were added to drawable class to maintain more handles to new-added vertex attributes, and corresponding buffers.

In the create function overriden by Chunk, every non-empty block was tested against six directions to see which faces of it should be drawn. If the block lied deep inside the chunk, we only need to perform test with its neighbors in this chunk to see which faces were to be drawn. If the block lied on the boundary, its neighboring blocks in the adjacent chunk were also tested against.

If a face of this block was to be drawn, its position, normal and color attributes would be pushed back to the chunk's member vectors. Element indices would also be pushed back to the vector<GLuint>. After all the tests on this chunk were done, the data in these vectors were fed to the GPU side with glBufferData().

These frequent calls to push_back() proved to be a bottleneck later on. Optimizations were conducted in the next milestones.
_____________________________________________
What I have for milestone 2:

Actually I'd like to talk about multi-threading first although it was not my task. When I reviewed my teammate, Jie Meng's code, I found that he attempted to make myGL::timerUpdate() create all the chunks remained in the queue. I modified this behavior and let the timer create chunks one by one, every now and then (every 3 times it was called, a chunk in the queue was created, to be precise).

There was a potential risk when new chunks were added: everytime timerUpdate() was called, boundry check was performed to see if new chunks should be added. If the function's called again when the dispatched threads were still hard at work, duplicate chunks would be added, significantly reducing performance. At first our code attempted to use a mutex bool flag to indicate whether we should start new threads, but its mutex lock was placed incorrectly. I attempted to fix this using QThreadPool::activeThreadCount(). If no active threads were present, go ahead and start threads. Otherwise, wait till they were done.

As for the part I was responsible for, I utilized the Texture class in our previous homeworks' base code to get the job done. To tell which of these texture slots were which, an enumeration textureSlot in shaderprogram.h was introduced, with SURFACE, NORMAL, GREYSCALE, GLOSSINESS, DUPL being surfacing texture, normal mapping, greyscale mapping (duplicated afterwards), glossiness texture (whose sampled greyscale value was used to calculate the per-fragment specular power) and a duplicate slot just in case new textures were needed.

Shaderprogram class and drawable class were modified greatly as well. I added some new attributes to our vertices: attrUV being uv coordinate; attrFlowVelocity being the flow velocity along u and v used by the animation of lava and water; attrTangent and attrBitangent being T, B vectors for normal mapping.

In the shader programs, I passed the timer as a uniform variable indicating time, so that we could calculate an offset of uv using time * flowVelocity, and add this offset to the fluid's uv value to make it "flow". The offset was designed to make its uv move like a sliding window on the texture, generating a seamless animation.

Since alpha blending was introduced, to make our transparent water look more convincing, I added a duplicate set of buffer handles and draw functions to Drawable class. Every time chunks were drawn, opaque blocks were drawn first, and the buffers in the vertex attribute array were switched, and transparent blocks were rendered on the second pass.

To make our program run even faster, I modified the Chunk class. These membering vectors used in create() was replaced by static ones, and when create() executes, every vector was reserved with more than enough space, avoiding the potential performance loss of vector resizing and copying around. After this optimization our program run significantly faster when new chunks were generated.

_____________________________________________
What I have for milestone 3:

Talk about the day and night cycle: I referred to the procedural sky demo on piazza, commented nearly every line of the shader, and fixed a minor flaw in the demo caused by atan function's singularity (tan(phi) = z / x, when x == 0 phi was undefined, making an embarassing gap on the sky). With this improvement, there won't be a gap on the x = 0 plane anymore.

In the shader program, Our sun ran on the x = 0.0f YZ plane in a circle. Three different colors were blended to give the final result of the sky. The clear sky daytime color, the sunset/dusk/dawn color, and the night color. The daytime/night color was mixed first using the height of the sun. The y component of the sun was normalized to [0, 1] range, and was used to blend day/night. Next, this color was mixed with sunset/dusk/dawn color, using sun's y component. The smaller abs(y) was, the closer the color would be to the sunset.

About biomes: see follows.

To make our program run eeeeeven faster, I used several methods to optimize:

1.
Define an "invalidBlocktype" = 0xFF;
Initialize blocks using fast memset function and make thier initial value all at invaildBlockType;
Maintain a height field information generated by Jie Meng's FBM height field.
When accessing block types, if the block type was "invalidBlocktype" (indicating that the block remained intact since the chunk was generated), we use the heightField information to deduct its type, write to blocks[] and return it. Otherwise, return the value in blocks[] directly.

This way, in the threads' run function, we no longer have to traverse 65536 blocks and assign their types respectively. Only 256 height values were needed to be written to the chunk it was responsible for. Consequently the burden was shifted to updateTimer() where chunks were created.

This height field method was used to efficiently generate snow peaks biome as well. If the height field value was above certain threshold, instead of generating dirt and grass, we add snow blocks to the part that exceeded the snow peak line.

2.
Introduced a bool flag isCompleted to myGL. See myGL line 280 - 308 to see how it works. This way we could directly tell when all these threads were finished and the chunks were valid, so that Lan Lou could generate his caves simply by setting blocks, instead of using expensive map operations.

3.
Managed to move the chunk allocating operations (new Chunk()) from thread's run() to the outside, since it was not expensive, but was causing performance loss in the threads.

4.
Set the threads' expiry time to 20ms, so that those threads that took exceptionally long to execute were hung and were later on run again when QThreadPool thinked it was appropriate.

After all these optimization, our game was close to seamless.