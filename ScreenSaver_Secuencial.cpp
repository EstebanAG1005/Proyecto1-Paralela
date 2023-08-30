#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <chrono>

struct Circle
{
    float x, y;
    float dx, dy;
    int radius;
    SDL_Color color;

    void move(int canvasWidth, int canvasHeight)
    {
        x += dx;
        y += dy;
        if (x <= 0 || x >= canvasWidth)
        {
            dx = -dx;
        }
        if (y <= 0 || y >= canvasHeight)
        {
            dy = -dy;
        }
    }

    static Circle randomCircle(int canvasWidth, int canvasHeight, int radius)
    {
        Circle c;
        c.x = rand() % canvasWidth;
        c.y = rand() % canvasHeight;

        do
        {
            c.dx = (rand() % 10 - 5) / 5.0;
            c.dy = (rand() % 10 - 5) / 5.0;
        } while (c.dx == 0 && c.dy == 0);

        c.radius = radius;
        c.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};
        return c;
    }
};

void drawCircle(SDL_Renderer *renderer, int x, int y, int radius)
{
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    const int canvasWidth = 640;
    const int canvasHeight = 480;

    int N = 100;    // Default number of circles
    int radius = 5; // Default radius

    if (argc > 1)
        N = std::stoi(argv[1]); // Number of circles from the first argument
    if (argc > 2)
        radius = std::stoi(argv[2]); // Radius from the second argument

    SDL_Window *window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Circle circles[N]; // Dynamically adjust based on N
    for (int i = 0; i < N; i++)
    {
        circles[i] = Circle::randomCircle(canvasWidth, canvasHeight, radius);
    }

    bool isRunning = true;
    while (isRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < N; i++)
        {
            SDL_SetRenderDrawColor(renderer, circles[i].color.r, circles[i].color.g, circles[i].color.b, circles[i].color.a);
            drawCircle(renderer, circles[i].x, circles[i].y, circles[i].radius);
            circles[i].move(canvasWidth, canvasHeight);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
