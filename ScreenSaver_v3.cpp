#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <omp.h>

// Definición de clase Timer para medir el tiempo
class Timer
{
public:
    Timer(const std::string &name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
    ~Timer()
    {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Tiempo tomado por " << name << ": " << duration.count() << " microsegundos" << std::endl;
    }

private:
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

// Definición de la estructura Particle
struct Particle
{
    float x, y;
    float dx, dy;
    int lifetime;
    SDL_Color color;

    // Método para mover la partícula
    void move()
    {
        x += dx;
        y += dy;
        lifetime--;
    }
};

// Definición de la estructura Circle
struct Circle
{
    float x, y;
    float dx, dy;
    int radius;
    SDL_Color color;

    // Método para mover el círculo y gestionar colisiones
    void move(int canvasWidth, int canvasHeight, std::vector<Circle> &circles, std::vector<Particle> &particles)
    {
        x += dx;
        y += dy;

        if (x - radius <= 0 || x + radius >= canvasWidth)
        {
            dx = -dx;
        }
        if (y - radius <= 0 || y + radius >= canvasHeight)
        {
            dy = -dy;
        }
        for (auto &other : circles)
        {
            float distance = sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
            if (&other != this && distance <= (radius + other.radius))
            {
                dx = -dx;
                dy = -dy;

                // Mover los círculos fuera del área de colisión
                float overlap = radius + other.radius - distance;
                float angle = atan2(y - other.y, x - other.x);
                x += overlap * cos(angle) / 2;
                y += overlap * sin(angle) / 2;
                other.x -= overlap * cos(angle) / 2;
                other.y -= overlap * sin(angle) / 2;

                const int numParticles = 30;
                for (int i = 0; i < numParticles; i++)
                {
                    Particle p;
                    p.x = x;
                    p.y = y;
                    float angle = (2 * M_PI / numParticles) * i;
                    p.dx = 0.5 * cos(angle);
                    p.dy = 0.5 * sin(angle);
                    p.lifetime = 30 + (rand() % 20);
                    p.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};
                    particles.push_back(p);
                }

                break;
            }
        }
    }
    // Método estático para generar un círculo aleatorio
    static Circle randomCircle(int canvasWidth, int canvasHeight)
    {
        Circle c;
        c.x = rand() % canvasWidth;
        c.y = rand() % canvasHeight;
        c.dx = (rand() % 10 - 5) / 5.0;
        c.dy = (rand() % 10 - 5) / 5.0;
        c.radius = rand() % 20 + 5;
        c.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};
        return c;
    }
};

int main(int argc, char *argv[])
{
    // Inicializar SDL para vídeo
    SDL_Init(SDL_INIT_VIDEO);

    const int canvasWidth = 640;
    const int canvasHeight = 480;
    int N = 100;
    int specifiedRadius = -1;


    // Procesar argumentos de línea de comando
    if (argc > 1)
    {
        try
        {
            N = std::stoi(argv[1]);
            if (N <= 0)
            {
                std::cerr << "Error: N debe ser un número positivo." << std::endl;
                return 1;
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Error: El primer argumento no es un número válido." << std::endl;
            return 1;
        }
    }
    if (argc > 2)
    {
        try
        {
            specifiedRadius = std::stoi(argv[2]);
            if (specifiedRadius <= 0)
            {
                std::cerr << "Error: El segundo argumento debe ser un número positivo." << std::endl;
                return 1;
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Error: El segundo argumento no es un número válido." << std::endl;
            return 1;
        }
    }
    // Crear ventana y renderer SDL
    SDL_Window *window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Crear vectores para círculos y partículas
    std::vector<Circle> circles(N);
    std::vector<Particle> particles;

// Inicializar círculos con OpenMP
#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        Circle circulo = Circle::randomCircle(canvasWidth, canvasHeight);

        if (specifiedRadius != -1)
        {
            circulo.radius = specifiedRadius;
        }

#pragma omp critical
        {
            circles[i] = circulo;
        }
    }
    // Variables para el control del bucle principal
    bool isRunning = true;
    Uint32 startTime = SDL_GetTicks();
    Uint32 frameCount = 0;

    // Variables para estadísticas de tiempo
    double totalTimeCircles = 0;
    int iterationsCircles = 0;

    double totalTimeParticles = 0;
    int iterationsParticles = 0;

    while (isRunning)
    {
        // Manejar eventos SDL
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

        {

            Timer timer("Bloque de Círculos");
            auto startCircles = std::chrono::high_resolution_clock::now();

            // #pragma omp parallel for
            for (auto &circle : circles)
            {
                SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
                // #pragma omp parallel for
                for (int w = -circle.radius; w < circle.radius; w++)
                {
                    // #pragma omp parallel for
                    for (int h = -circle.radius; h < circle.radius; h++)
                    {
                        if (w * w + h * h <= circle.radius * circle.radius)
                        {
                            // #pragma omp critical
                            SDL_RenderDrawPoint(renderer, circle.x + w, circle.y + h);
                        }
                    }
                }
                circle.move(canvasWidth, canvasHeight, circles, particles);
            }
            auto stopCircles = std::chrono::high_resolution_clock::now();
            auto durationCircles = std::chrono::duration_cast<std::chrono::microseconds>(stopCircles - startCircles);
            totalTimeCircles += durationCircles.count();
            iterationsCircles++;
        }

        {
            Timer timer("Bloque de Partículas");
            auto startParticles = std::chrono::high_resolution_clock::now();

#pragma omp parallale for
            for (auto &particle : particles)
            {

                SDL_SetRenderDrawColor(renderer, particle.color.r, particle.color.g, particle.color.b, 255);
                SDL_RenderDrawPoint(renderer, particle.x, particle.y);
#pragma omp critical
                particle.move();
            }

#pragma omp critical
            particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p)
                                           { return p.lifetime <= 0; }),
                            particles.end());
            auto stopParticles = std::chrono::high_resolution_clock::now();
            auto durationParticles = std::chrono::duration_cast<std::chrono::microseconds>(stopParticles - startParticles);
            totalTimeParticles += durationParticles.count();
            iterationsParticles++;
        }

        SDL_RenderPresent(renderer);

        frameCount++;

        if (SDL_GetTicks() - startTime >= 1000)
        {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            startTime += 1000;
        }
    }

    if (iterationsCircles > 0)
    {
        std::cout << "Tiempo promedio para círculos: " << totalTimeCircles / iterationsCircles << " microsegundos" << std::endl;
    }
    if (iterationsParticles > 0)
    {
        std::cout << "Tiempo promedio para partículas: " << totalTimeParticles / iterationsParticles << " microsegundos" << std::endl;
    }
    // Limpieza
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}