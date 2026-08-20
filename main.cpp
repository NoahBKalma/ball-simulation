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

    // Calculates the normal
    Vector2 n = Vector2Subtract(ball2.position, ball1.position);
    Vector2 nUnit = toUnitVector(n);

    // Calculates velocities along normal
    float v1n = Vector2DotProduct(ball1.velocity, nUnit);
    float v2n = Vector2DotProduct(ball2.velocity, nUnit);

    // Calculates new normal velocities (scalar, no direction)
    float v1nNew  = v1n - (1 + e) * (ball2.mass / (ball1.mass + ball2.mass)) * (v1n - v2n);
    float v2nNew  = v2n + (1 + e) * (ball1.mass / (ball1.mass + ball2.mass)) * (v1n - v2n);

    // Calculates final velocities with directions
    Vector2 v1New = Vector2Add(ball1.velocity, Vector2Scale(nUnit, v1nNew - v1n));
    Vector2 v2New = Vector2Add(ball2.velocity, Vector2Scale(nUnit, v2nNew - v2n));    
    return Vector4{ v1New.x, v1New.y, v2New.x, v2New.y };
}

int main() {
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
    constexpr int gravSliderWidth = 200;
    constexpr int gravSliderHeight = 20;
    constexpr float restitutionCoeff = 0.8f; // How much energy is conserved in collision
    constexpr Vector2 gravSliderPos = { 35, 10 };
    Rectangle gravSliderRec = { gravSliderPos.x, gravSliderPos.y, gravSliderWidth, gravSliderHeight };
    int grabbedBallIndex = -1;

    float gravity = 0.6;

    std::vector<Ball> balls {};

    InitWindow(screenWidth, screenHeight, "Ball Simulation");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {   // Detect window close with button or ESC key
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Creates a slider for gravity
        GuiSlider(gravSliderRec, "-100%", "100%", &(gravity), -0.8f, 0.8f); // Slider to control gravity

        // Places ball if mouse not on gravity slider
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(!CheckCollisionPointRec(GetMousePosition(), gravSliderRec)) {

                for(int i = 0; i < balls.size(); i++) {
                    if(CheckCollisionPointCircle(GetMousePosition(), balls[i].position, balls[i].radius)) {
                        grabbedBallIndex = i;
                        break;
                    }
                }

                // If clicked on an empty area, place ball
                if(grabbedBallIndex == -1) {
                    balls.emplace_back();
                    grabbedBallIndex = balls.size() - 1;
                }
            }
        }
        
        // Lets ball go on mouse release
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && grabbedBallIndex != -1) {
            balls[grabbedBallIndex].velocity = GetMouseDelta();
            grabbedBallIndex = -1;            
        }

        if(IsKeyPressed('R')) {
            balls.clear();
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
                        balls[i].position = Vector2Subtract(balls[i].position, Vector2Scale(toUnitVector(deltaVec), overlap));
                    }

                    Vector4 resultVec = calcBallsCollision(balls[i], balls[j], restitutionCoeff);
                    
                    balls[i].velocity.x = resultVec.x;
                    balls[i].velocity.y = resultVec.y;
                    balls[j].velocity.x = resultVec.z;
                    balls[j].velocity.y = resultVec.w;    
                }
            }

            if(i == grabbedBallIndex) { // Updates grabbed ball
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