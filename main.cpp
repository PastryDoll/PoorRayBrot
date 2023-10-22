#include "raylib.h"
#include "math.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>         // Required for: malloc() and free()
#include <time.h>
#include <pthread.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define MAX_ITER 100
#define MAX_NORM 4
#define SUPERSAMPLING_FACTOR 16
#define NUM_THREADS 7

u32 sampling = 1;
Color pixels[SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(Color)] = {0};
pthread_t threads[NUM_THREADS];

inline double getRand() 
{
    int randomInt = rand() % 11;
    double randomDouble = (double)randomInt / 10;
    return randomDouble;
}

typedef struct MandelArgs
{
    Color* pixels;
    Vector2 offSet;
    double zoom;
    int begin;
    int end;
}MandelArgs;


// TODO: Fix smooth color.. add multiple thread.. maybe add more precision
inline void* Mandel(void* arg)
{   
    MandelArgs Args = *(MandelArgs*)arg;
    for (int y = Args.begin; y < Args.end; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {    
            double colorSumR = 0.0;
            double colorSumG = 0.0;
            double colorSumB = 0.0;

            for (int i = 0; i < sampling; i++)
            {
                double Randx = (sampling > 1)? getRand() : 0;
                double Randy = (sampling > 1)? getRand() : 0;
                double real = (x + (Randx) - SCREEN_WIDTH / 2.0) * 4.0 / SCREEN_WIDTH / Args.zoom + Args.offSet.x;
                double imag = (y + (Randy) - SCREEN_HEIGHT / 2.0) * 4.0 / SCREEN_HEIGHT / Args.zoom + Args.offSet.y;
                double c_real = real;
                double c_imag = imag;
                int n = 0;
                double z_real = 0.0;
                double z_imag = 0.0;
                double smoothColor = 0.0;

                while (n < MAX_ITER)
                {
                    double z2_real = z_real * z_real - z_imag * z_imag;
                    double z2_imag = 2.0 * z_real * z_imag;
                    z_real = z2_real + c_real;
                    z_imag = z2_imag + c_imag;
                    if (z_real * z_real + z_imag * z_imag > MAX_NORM)
                    {
                        // smoothColor = n + 1 - log(log2(sqrt(z_real * z_real + z_imag * z_imag))) / log2(2); 
                        break;
                    }
                    n++;
                }

                if (n == MAX_ITER)
                {
                    colorSumR += 0;
                    colorSumG += 0;
                    colorSumB += 0;
                }
                else
                {   
                    // double smoothFactor = smoothColor - n;
                    // colorSumR += smoothFactor * ((n * 10) % 255) + ((n + 1) * 10) % 255;
                    // colorSumG += smoothFactor * ((n * 5) % 255) + ((n + 1) * 5) % 255;
                    // colorSumB += smoothFactor * ((n * 2) % 255) + ((n + 1) * 2) % 255;

                    colorSumR += (n * 10) % 255;
                    colorSumG += (n * 5) % 255;
                    colorSumB += (n * 2) % 255;

                }

            }
            Color color;
            color.a = 255;
            color.r = (char)(colorSumR / ( (sampling>1)? SUPERSAMPLING_FACTOR : 1));
            color.g = (char)(colorSumG / ( (sampling>1)? SUPERSAMPLING_FACTOR : 1));
            color.b = (char)(colorSumB / ( (sampling>1)? SUPERSAMPLING_FACTOR : 1));
            Args.pixels[y * SCREEN_WIDTH + x] = color;
        }
    }
}

int main(void){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Poor Del Brot");
    // Mandel(pixels);
    Image checkedIm = 
    {
        .data = pixels,             // We can assign pixels directly to data
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };
    double zoom = 1;
    float n = 1;
    Vector2 offSet = {0,0};
    Vector2 oldOffSet = {0,0};
    
    while (!WindowShouldClose())            // Detect window close button or ESC key
    {   
        Vector2 mouse = GetMousePosition();  
        Vector2 NormalizeMouse = {0,0};  
        if (IsKeyPressed(KEY_A)) sampling = (sampling>1)? 1 : SUPERSAMPLING_FACTOR;
        if (IsKeyPressed(KEY_UP)) 
        {
            NormalizeMouse.x = (2.0*mouse.x/SCREEN_WIDTH - 1.0)*2.0;
            NormalizeMouse.y = (2.0*mouse.y/SCREEN_WIDTH - 1.0)*2.0;
            offSet.x += NormalizeMouse.x/zoom; 
            offSet.y += NormalizeMouse.y/zoom; 
            zoom += 100*n;
            n*=1.2;

        }
        if (IsKeyPressed(KEY_DOWN)) zoom -= 10;
        MandelArgs Args1 = {pixels, offSet, zoom, 0, 128};
        if (pthread_create(&threads[0], NULL, Mandel, &Args1) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args2 = {pixels, offSet, zoom, 128, 256};
        if (pthread_create(&threads[1], NULL, Mandel, &Args2) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args3 = {pixels, offSet, zoom, 256, 384};
        if (pthread_create(&threads[2], NULL, Mandel, &Args3) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args4 = {pixels, offSet, zoom, 384, 512};
        if (pthread_create(&threads[3], NULL, Mandel, &Args4) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args5 = {pixels, offSet, zoom, 512, 640};
        if (pthread_create(&threads[4], NULL, Mandel, &Args5) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args6 = {pixels, offSet, zoom, 640, 768};
        if (pthread_create(&threads[5], NULL, Mandel, &Args6) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args7 = {pixels, offSet, zoom, 768, 896};
        if (pthread_create(&threads[6], NULL, Mandel, &Args7) != 0)
        {
            perror("pthread_create");
            return 1;
        }
        MandelArgs Args8 = {pixels, offSet, zoom, 896, 1000};
        Mandel(&Args8);
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);
        pthread_join(threads[3], NULL);
        pthread_join(threads[4], NULL);
        pthread_join(threads[5], NULL);
        pthread_join(threads[6], NULL);
        Texture2D checked = LoadTextureFromImage(checkedIm);
        BeginDrawing();

            ClearBackground(BLACK);
            DrawTexture(checked, 0,0,WHITE);
            DrawCircle(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,1,WHITE);
            DrawText(TextFormat("Zoom: %f  Mouse: %f,%f",zoom,mouse.x,mouse.y),10,10,10,WHITE);
            DrawText(TextFormat("Frame Time: %f",GetFrameTime()),10,25,10,WHITE);
            DrawText(TextFormat("Anti-Aliasing: %s",(sampling>1)? "ON" : "OFF"),10,40,10,WHITE);
    
        EndDrawing();
        UnloadTexture(checked);
    }

    CloseWindow();                  // Close window and OpenGL context

    return 0;
}