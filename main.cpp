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
Vector4 calcBallCollision(Ball& ball1, Ball& ball2, float e) { // Returns x,y,z,w (x,y is velocity 1 and z,w is velocity 2)
    Vector4 finalVelocities = { 0, 0, 0, 0 };

    finalVelocities.x = (ball1.mass * ball1.velocity.x + ball2.mass * ball2.velocity.x - ball2.mass * e * (ball1.velocity.x - ball2.velocity.x))
                        / (ball1.mass + ball2.mass);
    finalVelocities.y = (ball1.mass * ball1.velocity.y + ball2.mass * ball2.velocity.y - ball2.mass * e * (ball1.velocity.y - ball2.velocity.y))
                        / (ball1.mass + ball2.mass);
                        
    finalVelocities.z = (ball1.mass * ball1.velocity.x + ball2.mass * ball2.velocity.x + ball1.mass * e * (ball1.velocity.x - ball2.velocity.x))
                        / (ball1.mass + ball2.mass);
    finalVelocities.w = (ball1.mass * ball1.velocity.y + ball2.mass * ball2.velocity.y + ball1.mass * e * (ball1.velocity.y - ball2.velocity.y))
                        / (ball1.mass + ball2.mass);

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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), gravSliderRec))
        {
            balls.emplace_back();
        }

        for(Ball& ball : balls) { // loop through each particle
            
            // Checks for ball collisions (TODO: OPTIMIZE)
            for(Ball& checkBall : balls) {
                if(&checkBall == &ball) continue; // skips same ball

                Vector2 ballNextPos = Vector2Add(ball.position, ball.velocity);
                Vector2 checkBallNextPos = Vector2Add(checkBall.position, checkBall.velocity);             

                if(CheckCollisionCircles(ballNextPos, ball.radius, checkBallNextPos, checkBall.radius)) { // In case of collision
                    
                    // Checks and makes sure no balls overlap
                    Vector2 deltaVec = Vector2Subtract(checkBall.position, ball.position);
                    float distance = Vector2Length(deltaVec);
                    float overlap = checkBall.radius + ball.radius - distance;
                    if(overlap > 0) { // fix overlap
                        ball.position = Vector2Add(ball.position, Vector2Scale(toUnitVector(deltaVec), overlap));
                    }

                    Vector4 resultVec = calcBallCollision(ball, checkBall, restitutionCoeff);
                    
                    ball.velocity.x = resultVec.x;
                    ball.velocity.y = resultVec.y;
                    checkBall.velocity.x = resultVec.z;
                    checkBall.velocity.y = resultVec.w;    
                }
            }

            // Updates position from velocity
            ball.position.x += ball.velocity.x;
            ball.position.y += ball.velocity.y;
            
            // Updates velocity from gravity
            ball.velocity.y += gravity;
            
            if(ball.position.y + ball.radius >= screenHeight) { // ball out of bounds below
                ball.velocity.y *= -restitutionCoeff;
                ball.position.y = screenHeight - ball.radius;
            } else if(ball.position.y - ball.radius <= 0) { // ball out of bounds above
                ball.velocity.y *= -restitutionCoeff;
                ball.position.y = ball.radius;
            }
    
            // Draws each ball
            DrawCircleV(ball.position, ball.radius, ball.color);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}