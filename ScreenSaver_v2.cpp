// Importar librerias
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>

// Clase Timer para medir el tiempo
class Timer
{
public:
    // Constructor que inicializa el nombre y el tiempo de inicio
    Timer(const std::string &name) : name(name), start(std::chrono::high_resolution_clock::now()) {}

    // Destructor que imprime el tiempo transcurrido
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

// Estructura para representar una partícula
struct Particle
{
    float x, y;
    float dx, dy;
    int lifetime;
    SDL_Color color;

    // Mover la partícula
    void move()
    {
        x += dx;
        y += dy;
        lifetime--;
    }
};

// Estructura para representar un círculo
struct Circle
{
    float x, y;
    float dx, dy;
    int radius;
    SDL_Color color;

    // Método para mover un círculo dentro de un área definida (canvas)
    void move(int canvasWidth, int canvasHeight, std::vector<Circle> &circles, std::vector<Particle> &particles)
    {
        // Actualizar la posición del círculo según su velocidad (dx, dy)
        x += dx;
        y += dy;

        // Comprobar si el círculo ha golpeado los bordes horizontales del canvas y revertir su velocidad en x si es necesario
        if (x - radius <= 0 || x + radius >= canvasWidth)
        {
            dx = -dx;
        }

        // Comprobar si el círculo ha golpeado los bordes verticales del canvas y revertir su velocidad en y si es necesario
        if (y - radius <= 0 || y + radius >= canvasHeight)
        {
            dy = -dy;
        }

        // Verificar si este círculo ha colisionado con otros círculos
        for (auto &other : circles)
        {
            // Calcular la distancia entre los centros de los dos círculos
            float distance = sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));

            // Verificar si hay colisión y que no sea el mismo círculo
            if (&other != this && distance <= (radius + other.radius))
            {
                // Revertir la dirección de movimiento de este círculo
                dx = -dx;
                dy = -dy;

                // Calcular la cantidad de superposición entre los dos círculos
                float overlap = radius + other.radius - distance;

                // Calcular el ángulo entre los dos círculos
                float angle = atan2(y - other.y, x - other.x);

                // Corregir la posición de ambos círculos para resolver la colisión
                x += overlap * cos(angle) / 2;
                y += overlap * sin(angle) / 2;
                other.x -= overlap * cos(angle) / 2;
                other.y -= overlap * sin(angle) / 2;

                // Crear partículas cuando los círculos colisionan
                const int numParticles = 30;
                for (int i = 0; i < numParticles; i++)
                {
                    Particle p;
                    p.x = x;
                    p.y = y;
                    float angle = (2 * M_PI / numParticles) * i;
                    p.dx = 0.5 * cos(angle);
                    p.dy = 0.5 * sin(angle);
                    p.lifetime = 30 + (rand() % 20); // Vida aleatoria entre 30 y 49
                    p.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};
                    particles.push_back(p);
                }

                // Romper el bucle; no es necesario verificar más colisiones para este círculo en este fotograma
                break;
            }
        }
    }

    // Método estático para generar un círculo con valores aleatorios
    static Circle randomCircle(int canvasWidth, int canvasHeight)
    {
        // Crear una instancia de la clase Circle
        Circle c;

        // Asignar una posición x aleatoria en el rango [0, canvasWidth)
        c.x = rand() % canvasWidth;

        // Asignar una posición y aleatoria en el rango [0, canvasHeight)
        c.y = rand() % canvasHeight;

        // Asignar una velocidad x aleatoria en el rango [-1.0, 1.0)
        // `rand() % 10 - 5` genera un entero entre -5 y 4, luego se divide por 5.0 para obtener el rango deseado
        c.dx = (rand() % 10 - 5) / 5.0;

        // Asignar una velocidad y aleatoria en el rango [-1.0, 1.0)
        c.dy = (rand() % 10 - 5) / 5.0;

        // Asignar un radio aleatorio en el rango [5, 24] (ambos incluidos)
        // `rand() % 20` genera un entero entre 0 y 19, luego se suma 5
        c.radius = rand() % 20 + 5;

        // Asignar un color aleatorio al círculo
        // Cada componente de color (R, G, B, A) es un entero aleatorio entre 0 y 255
        c.color = {Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255};

        // Devolver el círculo con los valores aleatorios generados
        return c;
    }
};

int main(int argc, char *argv[])
{
    // Inicializar SDL para el vídeo
    SDL_Init(SDL_INIT_VIDEO);

    // Dimensiones del lienzo
    const int canvasWidth = 640;
    const int canvasHeight = 480;

    // Número de círculos y radio específico (si se especifica)
    int N = 100;
    int specifiedRadius = -1;

    // Leer argumentos de línea de comandos, si los hay
    if (argc > 1)
    {
        N = std::stoi(argv[1]);
    }
    if (argc > 2)
    {
        specifiedRadius = std::stoi(argv[2]);
    }

    // Crear una ventana y un renderizador
    SDL_Window *window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Inicializar vectores para círculos y partículas
    std::vector<Circle> circles(N);
    std::vector<Particle> particles;

    // Llenar el vector de círculos con círculos aleatorios
    for (int i = 0; i < N; i++)
    {
        circles[i] = Circle::randomCircle(canvasWidth, canvasHeight);
        if (specifiedRadius != -1)
        {
            circles[i].radius = specifiedRadius;
        }
    }

    // Variables para el ciclo principal
    bool isRunning = true;
    Uint32 startTime = SDL_GetTicks();
    Uint32 frameCount = 0;

    // Variables para medir el tiempo
    double totalTimeCircles = 0;
    int iterationsCircles = 0;

    double totalTimeParticles = 0;
    int iterationsParticles = 0;

    while (isRunning)
    {
        // Manejo de eventos
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        // Configurar color de fondo y limpiar el renderizador
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Bloque para dibujar y mover círculos
        {
            Timer timer("Bloque de Círculos");
            auto startCircles = std::chrono::high_resolution_clock::now();

            for (auto &circle : circles)
            {
                // Configurar el color de dibujo en el renderizador para este círculo.
                SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);

                // Dibujar el círculo píxel por píxel.
                for (int w = -circle.radius; w < circle.radius; w++)
                {

                    for (int h = -circle.radius; h < circle.radius; h++)
                    {
                        // Comprobar si el píxel está dentro del círculo.
                        if (w * w + h * h <= circle.radius * circle.radius)
                        {
                            // Dibujar el píxel en la posición correcta.
                            SDL_RenderDrawPoint(renderer, circle.x + w, circle.y + h);
                        }
                    }
                }
                // Mover el círculo y manejar cualquier colisión.
                circle.move(canvasWidth, canvasHeight, circles, particles);
            }

            // Tomar el tiempo actual nuevamente para calcular la duración del proceso.
            auto stopCircles = std::chrono::high_resolution_clock::now();
            auto durationCircles = std::chrono::duration_cast<std::chrono::microseconds>(stopCircles - startCircles);
            totalTimeCircles += durationCircles.count();
            iterationsCircles++;
        }

        {
            Timer timer("Bloque de Partículas");
            auto startParticles = std::chrono::high_resolution_clock::now();

            // Iterar a través de cada partícula en el vector de partículas.
            for (auto &particle : particles)
            {
                // Configura el color de dibujo en el renderizador para esta partícula.
                SDL_SetRenderDrawColor(renderer, particle.color.r, particle.color.g, particle.color.b, 255);

                // Dibuja un punto en la posición de la partícula.
                SDL_RenderDrawPoint(renderer, particle.x, particle.y);
                // Mueve la partícula.
                particle.move();
            }
            // Elimina las partículas que han alcanzado el final de su vida útil.
            particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p)
                                           { return p.lifetime <= 0; }),
                            particles.end());

            // Captura el tiempo de finalización para calcular la duración del procesamiento de partículas.
            auto stopParticles = std::chrono::high_resolution_clock::now();
            auto durationParticles = std::chrono::duration_cast<std::chrono::microseconds>(stopParticles - startParticles);
            totalTimeParticles += durationParticles.count();
            iterationsParticles++;
        }

        // mostrar todo en pantalla
        SDL_RenderPresent(renderer);

        // Calcular y mostrar la tasa de cuadros (FPS)
        frameCount++;
        if (frameCount % 100 == 0)
        {
            Uint32 currentTicks = SDL_GetTicks();
            std::cout << "FPS: " << 1000.0 * frameCount / (currentTicks - startTime) << std::endl;
        }
    }

    // Mostrar tiempos promedio
    if (iterationsCircles > 0)
    {
        std::cout << "Tiempo promedio para círculos: " << totalTimeCircles / iterationsCircles << " microsegundos" << std::endl;
    }
    if (iterationsParticles > 0)
    {
        std::cout << "Tiempo promedio para partículas: " << totalTimeParticles / iterationsParticles << " microsegundos" << std::endl;
    }

    // Limpiar recursos de SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
