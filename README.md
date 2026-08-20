# Ball Simulation

A 2D physics sandbox built with C++ and raylib.
Click to spawn balls and drag them around to throw them at each other. They will collide and bounce off the walls with momentum-based physics and adjustable gravity.

## Features

- Elastic/inelastic ball-to-ball collisions using mass-based momentum conservation
- Configurable restitution coefficient (in code) controlling energy loss on impact
- Click to spawn and drag to throw ball interaction
- Adjustable gravity via a live slider (raygui)
- Overlap correction so colliding balls don't visually overlap

## Requirements

- C++ compiler (GCC or equivalent)
- [raylib](https://www.raylib.com/)
- raygui (included as `raygui.h`)

## Installation/Usage

### 1. Clone the repository

```
git clone https://github.com/NoahBKalma/ball-simulation
```

### 2. Build and run

This was built on Linux. Mac and Windows usage may vary.
```
g++ main.cpp -o ball-simulation -lraylib -lGL -lm -lpthread -ldl -lX11
./ball-simulation
```

### Controls

- Left click (empty space): Spawn a new ball
- Left click + drag (on a ball): Grab + move a ball
- Release drag: Throw the ball with your mouse's velocity
- Slider (top of window): Adjust gravity strength/direction
- R: Clear all balls
