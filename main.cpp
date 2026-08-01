#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#include <vector>
#include <iostream>

struct Ball
{
    Vector2 velocity;
    Vector2 position;
    int radius;
    Color color;
    float mass;
    static constexpr float density = 5;

    Ball() {
        position = GetMousePosition();
        color = { (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255),
                    255 };
        radius = GetRandomValue(10, 20);
        velocity = { 0, 0 };
        mass = PI * radius * radius * density;
    };

};

Vector2 toUnitVector(Vector2 vector) {
    float vectorLength = Vector2Length(vector);
    vector.x /= vectorLength;
    vector.y /= vectorLength;
    return vector;
}

// Calculates a partially inelastic collision between two balls
Vector4 calcBallsCollision(Ball& ball1, Ball& ball2, float e) { // Returns x,y,z,w (x,y is velocity 1 and z,w is velocity 2)
    Vector4 finalVelocities = { 0, 0, 0, 0 };

    // Calculates the normals
    Vector2 n1 = Vector2Subtract(ball2.position, ball1.position);
    Vector2 n2 = Vector2Subtract(ball2.position, ball1.position);

    // Calculates ball momentums
    Vector2 p1 = Vector2Scale(ball1.velocity, ball1.mass);
    Vector2 p2 = Vector2Scale(ball2.velocity, ball2.mass);
    // Calculates magnitude of collision impulse
    float j = -(1 + e);

    return finalVelocities;
}

int main() {
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
    constexpr int gravSliderWidth = 200;
    constexpr int gravSliderHeight = 20;
    constexpr float restitutionCoeff = .5f; // How much energy is conserved in collision
    constexpr Vector2 gravSliderPos = { 25, 10 };
    Rectangle gravSliderRec = { gravSliderPos.x, gravSliderPos.y, gravSliderWidth, gravSliderHeight };
    Ball* grabbedBall = nullptr;

    float gravity = 0.1;

    std::vector<Ball> balls {};

    InitWindow(screenWidth, screenHeight, "Particle Simulation");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {   // Detect window close with button or ESC key
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // CONTROLS
        GuiSlider(gravSliderRec, "0%", "100%", &(gravity), -0.5f, 0.5f); // Slider to control gravity

        // Places ball if mouse not on gravity slider
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(CheckCollisionPointRec(GetMousePosition(), gravSliderRec)) break;

            for(Ball& ball : balls) {
                if(CheckCollisionPointCircle(GetMousePosition(), ball.position, ball.radius)) {
                    grabbedBall = &ball;
                    break;
                }
            }

            if(grabbedBall == nullptr) { balls.emplace_back(); }
        }
        

        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && grabbedBall != nullptr) {
            grabbedBall->velocity = GetMouseDelta();
            grabbedBall = nullptr;            
        }

        for(int i = 0; i < balls.size(); i++) { // loop through each ball
            
            // Checks for ball collisions (TODO: OPTIMIZE)
            for(int j = i + 1; j < balls.size(); j++) {
                if(j >= balls.size()) break; // check out of bounds

                Vector2 ballNextPos = Vector2Add(balls[i].position, balls[i].velocity);
                Vector2 checkBallNextPos = Vector2Add(balls[j].position, balls[j].velocity);             

                if(CheckCollisionCircles(ballNextPos, balls[i].radius, checkBallNextPos, balls[j].radius)) { // In case of collision
                    
                    // Checks and makes sure no balls overlap
                    Vector2 deltaVec = Vector2Subtract(balls[j].position, balls[i].position);
                    float distance = Vector2Length(deltaVec);
                    float overlap = balls[j].radius + balls[i].radius - distance;
                    if(overlap > 0) { // fix overlap
                        balls[i].position = Vector2Add(balls[i].position, Vector2Scale(toUnitVector(deltaVec), overlap));
                    }

                    Vector4 resultVec = calcBallsCollision(balls[i], balls[j], restitutionCoeff);
                    
                    balls[i].velocity.x = resultVec.x;
                    balls[i].velocity.y = resultVec.y;
                    balls[j].velocity.x = resultVec.z;
                    balls[j].velocity.y = resultVec.w;    
                }
            }

            if(&balls[i] == grabbedBall) { // Updates grabbed ball
                balls[i].position = GetMousePosition();
                
                if(balls[i].position.y + balls[i].radius >= screenHeight) { // ball out of bounds below
                    balls[i].position.y = screenHeight - balls[i].radius;
                } else if(balls[i].position.y - balls[i].radius <= 0) { // ball out of bounds above
                    balls[i].position.y = balls[i].radius;
                } else if(balls[i].position.x + balls[i].radius >= screenWidth) { // ball out of bounds right
                    balls[i].position.x = screenWidth - balls[i].radius;
                } else if(balls[i].position.x - balls[i].radius <= 0) { // ball out of bounds left
                    balls[i].position.x = balls[i].radius;
                }

            } else { // Updates everything else
                // Updates position from velocity
                balls[i].position = Vector2Add(balls[i].position, balls[i].velocity);
                
                // Updates velocity from gravity
                balls[i].velocity.y += gravity;
                
                if(balls[i].position.y + balls[i].radius >= screenHeight) { // ball out of bounds below
                    balls[i].velocity.y *= -restitutionCoeff;
                    balls[i].position.y = screenHeight - balls[i].radius;
                } else if(balls[i].position.y - balls[i].radius <= 0) { // ball out of bounds above
                    balls[i].velocity.y *= -restitutionCoeff;
                    balls[i].position.y = balls[i].radius;
                } else if(balls[i].position.x + balls[i].radius >= screenWidth) { // ball out of bounds right
                    balls[i].velocity.x *= -restitutionCoeff;
                    balls[i].position.x = screenWidth - balls[i].radius;
                } else if(balls[i].position.x - balls[i].radius <= 0) { // ball out of bounds left
                    balls[i].velocity.x *= -restitutionCoeff;
                    balls[i].position.x = balls[i].radius;
                }
            }
    
            // Draws each ball
            DrawCircleV(balls[i].position, balls[i].radius, balls[i].color);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}