#pragma once

#include <string>
#include "glm/glm.hpp"

namespace engineSpace {
    typedef double real;
    typedef std::string string;

    class Engine;

    const struct {
        // position
        float x, y;
        // texcoords
        float u, v;
    } vertices[] = {
        // position       // texcoords
      { -1.0f,  -1.0f,    0.0f,  1.0f },
      {  1.0f,  -1.0f,    1.0f,  1.0f },
      { -1.0f,   1.0f,    0.0f,  0.0f },
      {  1.0f,   1.0f,    1.0f,  0.0f },
    };

    //ECS!
    struct Position { real x, y; };
    struct Velocity { real x, y; };
    struct Gravity { real meters_per_second; };
    struct Image { string image; real size; };
    struct Health { real percent; };
    struct Script { string name; };
}