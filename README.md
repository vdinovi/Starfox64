## Update
The course is over and my website-demo is being hosted here: http://users.csc.calpoly.edu/~idunn01/teaching/csc471/finalf17/


## CPE 471 - Final Project Proposal - *Starfox 64*
Vittorio Dinovi - vdinovi@calpoly.edu

> My final project will be a simplified rendition of the original level from *Starfox 64*.

![](https://r.mprd.se/media/images/40230-Star_Fox_64_(USA)-21.jpg)
## Description
The project will consist of an Arial Combat game emulating the original Starfox 64. It will be implemented as a C++ program using OpenGL for rendering.

Instead of investigating one piece of advanced material deeply, I will try to compose the many techniques we've developed over the quarter into one complex piece of work.
I will, however, make use of certain *advanced topics* (they were listed on the slides) such as:
* collision detection - necessary for core game mechanics
* particle effects - explosion animations
* procedural generation - enemies and rings, perhaps some simple terrain generation.


## Features
 I have assembled a list of features which I plan to implement according to the plan I have set.
### Primary features [necessary]
* Full control for the arwing consistent with the original game
* Collision detection with enemies and environment
* Projectile mechanics
* Enemies and bonus Rings
* Usage of accurate models and textures
* Game interface consistent with the original game


### Secondary features [as time allows]
* Level completeness and sophistication
* Use of particle effects in animations
* Procedural terrain generation
* Teammate speech animations
* **Complete implementation of original gameplay features**


## Plan
Implementation will be partitioned into serveral major milestones:
1. Load in the level from a source (online, from VRML exported by an emulator, or by recreating pieces manually)
2. Add a ship model and the ability to fly around the map controlling the ship
3. Implement collision between the ship and the map
4. Statically generate rings and enemies and implement collision detection
5. Implement ship & enemey projectiles mechanics
6. Add *special movements* to the ship controls
7. Add game interface and gameplay mechanics (points, health, stock, missiles)
8. Add teammate speech animations
9. Add complexity to the level (particle effect animations, procedural enemies/rings/terrain, boss mechanics, etc)
10. (**Additional features if time allows**)


## Materials
* **Game Assets**: Mesh files and textures for this classic game are available online. But, if I do encounter trouble finding such materials, I'll try to rip them from an emulator. If all else fails, I can try to find substitutes or recreate them entirely. I don't anticipate this being a problem
* **Help**: There are abundant resources available online the areas of game dev. and OpenGL so I don't anticipate this being an issue.

