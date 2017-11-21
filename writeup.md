# Write up
Place to record development activity/thoughts/resources

### 11/19 - 7:34 pm
* Mesh files used were obtained from [models-resource](https://www.models-resource.com/nintendo_64/starfox64/)
* Had to spend more time than I'd like to admit figuring how mtl files work and extending *Shape* to load in material files as well.
* Not sure how to render environment. I can't seem to find any meshes online. I'd imagine its **not** a skybox. I might start off creating some elongated surface in maya and just tile them as the game moves forward. I could also just create a water floor and clouds. It looks like the water environment has reflection which might be tricky (or not, just rendering the scene reflected over the horizon into the fbo for the water)
* Ended up chaning A LOT of the base code. Going pretty well so far.

### 11/20 - 12:12am
* Finished fixing up Shape
* Added simple ground texture which scrolls over time (texCoord + offset.y). Just uses a generic water texture on a quad that wraps
* Added camera controls (mostly pulling from asgn4). I had intended for wasd to be ship controls but I moved those to arrow keys. U key unlocks the camera (ship remains fixed in XY plane) but the camera can move around the scene. I figured this would help with debugging.
* Right now alot of the game state is in the Application class which is pretty disorganized. Should probably start factoring code into distinct entity classes.
* This ship pitch/yaw/planar-movement is pretty clunky but it works properly; eventually that will need to be cleaned up.
* Feeling pretty good about the project so far


### 11/20 2:49pm
* Tried to get phong working with textures (material/texture info loaded from mtl files). The best I could do was get some strange coloration -- not sure why :(. Gave up on it for now and reverted to using just textures with no lighting.
* Frustrated with ^^^, skipping for now. Next thing is to structure the existing code and implement some basic collision detection with the ground.

### 11/20 10:44pm
* Factored out arwing into its own class. Surprisingly went without a hitch.
* Factored out environment code into its own class. 
* Reworked ship movement and now it looks GREAT! Smooth as it could be.
