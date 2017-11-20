# Write up
Place to record development activity/thoughts/resources

### 11/19 - 7:34 pm
* Mesh files used were obtained from [models-resource](https://www.models-resource.com/nintendo_64/starfox64/)
* Had to spend more time than I'd like to admit figuring how mtl files work and extending *Shape* to load in material files as well.
* Not sure how to render environment. I can't seem to find any meshes online. I'd imagine its **not** a skybox. I might start off creating some elongated surface in maya and just tile them as the game moves forward. I could also just create a water floor and clouds. It looks like the water environment has reflection which might be tricky (or not, just rendering the scene reflected over the horizon into the fbo for the water)
* Ended up chaning A LOT of the base code. Going pretty well so far.
