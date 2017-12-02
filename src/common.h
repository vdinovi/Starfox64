#pragma once
#ifndef FINAL_471_COMMON_H_INCLUDED
#define FINAL_471_COMMON_H_INCLUDED

// Didn't want to have to include the whole glfw in some files when only a couple defs
// (perhaps this is unecessary -- not 100% on how includes are handled in cpp)
#define KEY_PRESS 0
#define KEY_REPEAT 1
#define KEY_RELEASE 2

#define AIRSPACE_HEIGHT 10.0
#define AIRSPACE_WIDTH 20.0

#define PARTICLE_LIFETIME 10
#define GRAVITY -0.2


#endif // FINAL_471_COMMON_H_INCLUDED