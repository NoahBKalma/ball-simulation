#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

#include <vector>
#include <iostream>

struct Particle
{
    Vector2 velocity;
    Vector2 position;
    int radius;
    Color color;

    Particle() {
        position = GetMousePosition();
        color = { (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255),
                    255 };
        radius = GetRandomValue(10, 20);
        velocity = { 0, 0 };
    };

};


int main() {
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
    constexpr int gravSliderWidth = 200;
    constexpr int gravSliderHeight = 20;
    constexpr Vector2 gravSliderPos = { 25, 10 };
    Rectangle gravSliderRec = { gravSliderPos.x, gravSliderPos.y, gravSliderWidth, gravSliderHeight };

    float gravity = 0.1;

    std::vector<Particle> particles {};

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
            particles.emplace_back();
        }


        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        for(Particle& particle : particles) { // loop through each particle

            // Updates position from velocity
            particle.position.x += particle.velocity.x;
            particle.position.y += particle.velocity.y;

            particle.velocity.y += gravity;
            
            if(particle.position.y + particle.radius >= screenHeight) { // particle out of bounds below
                particle.velocity.y = 0;
                particle.position.y = screenHeight - particle.radius;
            } else if(particle.position.y - particle.radius <= 0) { // particle out of bounds above
                particle.velocity.y = 0;
                particle.position.y = particle.radius;
            }
    
            // Draws each particle
            DrawCircleV(particle.position, particle.radius, particle.color);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}