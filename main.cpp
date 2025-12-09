#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define PATTERN_LENGTH  16 //16 steps (4 bars of 16th notes)




int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 900;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "simple drum machine");
    InitAudioDevice();

    

    Sound fxKick = LoadSound("sounds/kick.wav");
    Sound fxSnare = LoadSound("sounds/snare.wav");
    Sound fxHat = LoadSound("sounds/hat.wav");

    bool kickPattern[PATTERN_LENGTH] = { 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0 };  //Kick on every quarter note
    bool snarePattern[PATTERN_LENGTH] = { 0,0,0,0, 1,0,0,0, 0,0,0,0, 1,0,0,0 }; //Snare on beats 2 and 4
    bool hihatePattern[PATTERN_LENGTH] = { 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0 }; //Hi-hat on 8th notes

    //Playback control
    int currentStep = 0;
    float bpm = 120.0f;
    float stepTime = 60.0f / bpm / 4.0f; //Time for each 16th note
    float accumulator = 0.0f; //Time acculator
    bool isPlaying = false;

    //time to make the grid dimensions for clicking
    int gridX = 150;
    int gridY = 200;
    int stepSize = 40;
    int rowHeight = stepSize;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();


        //BPM Control with Up/Down arrows
        if (IsKeyPressed(KEY_UP)) bpm += 5.0f;
        if (IsKeyPressed(KEY_DOWN)) bpm -= 5.0f;
        if (bpm < 60.0f) bpm = 60.0f;
        if (bpm > 240.0f) bpm = 240.0f;

        //Update step time based on new BPM
        stepTime = 60.0f / bpm / 4.0f;

        if (IsKeyPressed(KEY_SPACE)) isPlaying = !isPlaying;

        // TODO: Update your variables here
        if (IsKeyPressed(KEY_SEVEN)) PlaySound(fxKick);
        if (IsKeyPressed(KEY_EIGHT)) PlaySound(fxSnare);
        if (IsKeyPressed(KEY_NINE))  PlaySound(fxHat);
        //----------------------------------------------------------------------------------

        //Mouse clikc to toggle steps
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();

            //check if the click is within grid 
            if (mousePos.x >= gridX && mousePos.x <= gridX + PATTERN_LENGTH * stepSize && mousePos.y >= gridY && mousePos.y <= gridY + 3 * rowHeight)
            {
                //Calculate which step was called
                int step = (int)((mousePos.x - gridX) / stepSize);
                int row = (int)((mousePos.y - gridY) / rowHeight);

                if (step >= 0 && step < PATTERN_LENGTH && row >= 0 && row < 3)
                {
                    //Play sound when toggling (for instant feedback)
                    //if (row == 0 && !kickPattern[step]) PlaySound(fxKick);
                    //if (row == 1 && !snarePattern[step]) PlaySound(fxSnare);
                    //if (row == 2 && !hihatePattern[step]) PlaySound(fxHat);

                    switch (row)
                    {
                    case 0: 
                        kickPattern[step] = !kickPattern[step];
                        break;

                    case 1:
                        snarePattern[step] = !snarePattern[step];
                        break;

                    case 2:
                        hihatePattern[step] = !hihatePattern[step];
                        break;
                    }
                }
            }
        }

        if (isPlaying)
        {
            accumulator += deltaTime; //Add time

            if (accumulator >= stepTime)
            {
                accumulator = 0; 

                //Play sounds for this step
                if (kickPattern[currentStep]) PlaySound(fxKick);
                if (snarePattern[currentStep]) PlaySound(fxSnare);
                if (hihatePattern[currentStep]) PlaySound(fxHat);

                //Move to next step (loop back to 0 after 15)
                currentStep++;
                if (currentStep >= PATTERN_LENGTH) currentStep = 0;
            }
        }

        if (IsKeyPressed(KEY_S))
        {
            isPlaying = false;
            currentStep = 0;
            accumulator = 0;
        }

        //Clear pattern with 'C'
        if (IsKeyPressed(KEY_C))
        {
            for (int i = 0; i < PATTERN_LENGTH; i++)
            {
                kickPattern[i] = false;
                snarePattern[i] = false;
                hihatePattern[i] = false;
            }
        }

        //Randomize pattern with 'R'
        if (IsKeyPressed(KEY_R))
        {
            for (int i = 0; i < PATTERN_LENGTH; i++)
            {
                kickPattern[i] = GetRandomValue(0, 3) == 0; //25% chance
                snarePattern[i] = GetRandomValue(0, 3) == 0; //25% chance
                hihatePattern[i] = GetRandomValue(0, 1) == 0; //50% chance
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        //DrawBackground(bg, screenWidth, screenHeight);
        ClearBackground(BLACK);

        //Draw semi-transparent overlay for readability
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));

        

        DrawText("SHITTY DRUM SEQUENCER", 240, 20, 36, MAGENTA);

        //Draw BPM display
        DrawText(TextFormat("BPM: %.0f", bpm), 400, 70, 30, YELLOW);
        DrawText("UP/DOWN arrows to change BPM", 330, 100, 20, LIGHTGRAY);

        if (isPlaying)
        {
            DrawText("PLAYING", 400, 140, 30, GREEN);
            DrawText("SPACE to pause", 360, 175, 20, LIGHTGRAY);
        }
        else
        {
            DrawText("STOPPED", 400, 140, 30, RED);
            DrawText("SPACE to play", 360, 175, 20, LIGHTGRAY);
        }


        //Draw step sequencer grid
        for (int step = 0; step < PATTERN_LENGTH; step++)
        {
            int x = gridX + step * stepSize;

            //Draw kick step (with glow if needed)
            if (kickPattern[step])
            {
                DrawRectangle(x, gridY, stepSize - 4, stepSize - 4, RED);
                DrawRectangleLines(x, gridY, stepSize - 4, stepSize - 4, YELLOW);
            }
            else
            {
                DrawRectangle(x, gridY, stepSize - 4, stepSize - 4, Fade(DARKGRAY, 0.5f));
                DrawRectangleLines(x, gridY, stepSize - 4, stepSize - 4, GRAY);
            }


            //Draw snare step
            if (snarePattern[step])
            {
                DrawRectangle(x, gridY + rowHeight, stepSize - 4, stepSize - 4, BLUE);
                DrawRectangleLines(x, gridY + rowHeight, stepSize - 4, stepSize - 4, YELLOW);
            }
            else
            {
                DrawRectangle(x, gridY + rowHeight, stepSize - 4, stepSize - 4, Fade(DARKGRAY, 0.5f));
                DrawRectangleLines(x, gridY + rowHeight, stepSize - 4, stepSize - 4, GRAY);
            }


            //Draw hi-hat step
            if (hihatePattern[step])
            {
                DrawRectangle(x, gridY + 2 * rowHeight, stepSize - 4, stepSize - 4, GREEN);
                DrawRectangleLines(x, gridY + 2 * rowHeight, stepSize - 4, stepSize - 4, YELLOW);
            }
            else
            {
                DrawRectangle(x, gridY + 2 * rowHeight, stepSize - 4, stepSize - 4, Fade(DARKGRAY, 0.5f));
                DrawRectangleLines(x, gridY + 2 * rowHeight, stepSize - 4, stepSize - 4, GRAY);
            }

            //Draw step numbers (with highlight on current step)
            if (step == currentStep && isPlaying)
            {
                DrawText(TextFormat("%d", step + 1), x + 10, gridY - 30, 20, YELLOW);
            }
            else
            {
                DrawText(TextFormat("%d", step + 1), x + 10, gridY - 30, 15, LIGHTGRAY);
            }
        }
 

        //Draw labels
        DrawText("KICK", gridX - 60, gridY + 10, 20, RED);
        DrawText("SNARE", gridX - 80, gridY + stepSize + 10, 20, BLUE);
        DrawText("HI-HAT", gridX - 86, gridY + 2 * stepSize + 10, 20, GREEN);


        //Draw playhead (shows current step)
        if (isPlaying)
        {
            int playheadX = gridX + currentStep * stepSize;
            DrawRectangle(playheadX, gridY - 10, stepSize - 4, 10, YELLOW);

            //Pulsing effect on playhead
            //float pulse = sinf(bg.time * 10.0f) * 0.5f + 0.5f;
            //DrawRectangleLines(playheadX, gridY - 10, stepSize - 4, 10, DARKBLUE);
        }

        //Draw control instructions
        DrawText("CONTROLS: ", 50, 400, 24, LIGHTGRAY);
        DrawText("CLICK grid to toggle steps", 50, 430, 20, WHITE);
        DrawText("SPACE = Play/Pause", 50, 455, 20, WHITE);
        DrawText("S = Stop & Reset", 50, 480, 20, WHITE);
        DrawText("C = Clear pattern", 50, 505, 20, WHITE);
        DrawText("R = Random pattern", 50, 530, 20, WHITE);

        DrawText("7/8/9 = Manual trigger", 600, 4300, 20, WHITE);
        DrawText("Arrow Keys = Change BPM", 600, 455, 20, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadSound(fxKick);
    UnloadSound(fxSnare);
    UnloadSound(fxHat);

    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}