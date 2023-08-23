// Inclusión de las cabeceras necesarias
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <string>

// Definición de la estructura Circle
struct Circle
{
    // Propiedades del círculo
    float x, y;      // Coordenadas (posición) del círculo
    float dx, dy;    // Velocidad y dirección del movimiento del círculo
    int radius;      // Radio del círculo
    SDL_Color color; // Color del círculo

    // Función para mover el círculo
    void move(int canvasWidth, int canvasHeight)
    {
        // Actualiza la posición basada en su velocidad y dirección
        x += dx;
        y += dy;

        // Comprobación de colisión con los bordes para hacer que el círculo rebote
        if (x <= 0 || x >= canvasWidth)
        {
            dx = -dx; // Invierte la dirección horizontal
        }
        if (y <= 0 || y >= canvasHeight)
        {
            dy = -dy; // Invierte la dirección vertical
        }
    }

    // Función para generar un círculo aleatorio
    static Circle randomCircle(int canvasWidth, int canvasHeight)
    {
        Circle c;
        // Define posición aleatoria dentro del lienzo
        c.x = rand() % canvasWidth;
        c.y = rand() % canvasHeight;
        // Define velocidad y dirección aleatorias
        c.dx = (rand() % 10 - 5) / 5.0;
        c.dy = (rand() % 10 - 5) / 5.0;
        // Define un radio aleatorio entre 5 y 25
        c.radius = rand() % 20 + 5;
        // Define un color aleatorio
        c.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};
        return c;
    }
};

int main(int argc, char *argv[])
{
    // Inicializa SDL con soporte para video
    SDL_Init(SDL_INIT_VIDEO);

    // Definición de dimensiones del lienzo
    const int canvasWidth = 640;
    const int canvasHeight = 480;

    // Establece un número predeterminado de círculos
    int N = 100;

    // Si se proporciona un argumento al programa, úsalo para establecer el número de círculos
    if (argc > 1)
    {
        N = std::stoi(argv[1]);
    }

    // Crea una ventana con el nombre "Screensaver" y las dimensiones definidas
    SDL_Window *window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, SDL_WINDOW_SHOWN);

    // Crea un renderizador para dibujar en la ventana
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Crea un array de círculos y los inicializa con círculos aleatorios
    Circle circles[N];
    for (int i = 0; i < N; i++)
    {
        circles[i] = Circle::randomCircle(canvasWidth, canvasHeight);
    }

    // Bucle principal del programa
    bool isRunning = true;
    while (isRunning)
    {
        SDL_Event event;

        // Manejo de eventos (como el cierre de la ventana)
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false; // Termina el bucle si se cierra la ventana
            }
        }

        // Establece el color de fondo y limpia el renderizador
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja y mueve cada círculo
        for (int i = 0; i < N; i++)
        {
            // Establece el color del círculo y dibuja un punto en su posición
            SDL_SetRenderDrawColor(renderer, circles[i].color.r, circles[i].color.g, circles[i].color.b, circles[i].color.a);
            SDL_RenderDrawPoint(renderer, circles[i].x, circles[i].y);

            // Mueve el círculo
            circles[i].move(canvasWidth, canvasHeight);
        }

        // Presenta los cambios en el renderizador a la ventana
        SDL_RenderPresent(renderer);
    }

    // Limpieza: destruye el renderizador y la ventana y cierra SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0; // Fin del programa
}
