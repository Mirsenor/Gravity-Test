#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_NAME "Gravity Test"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define TAU 6.2831854f
#define G_CONST 6.6743e-11f
//#define CIRCLE_ANGLE 0.3926991f
#define CIRCLE_VERTICES 16
#define METRES_IN_PIXEL 1000.0f

constexpr float circle_angle = TAU / CIRCLE_VERTICES;
constexpr float pixels_in_metre = 1.0f / METRES_IN_PIXEL;
constexpr float half_window_width = WINDOW_WIDTH / 2.0f;
constexpr float half_window_height = WINDOW_HEIGHT / 2.0f;
const int circle_vertices_indicies[42] = {
    0, 15, 1,
    1, 15, 2,  15, 14, 2,  2, 14, 3,  14, 13, 3,
    3, 13, 4,  13, 12, 4,  4, 12, 5,  12, 11, 5,
    5, 11, 6,  11, 10, 6,  6, 10, 7,  10, 9, 7,
    7, 9, 8
};

typedef struct position {
    float x;
    float y;
} position;

class planet {
    private:
        float mass;
        float radius;
        position pos;
        position velocity;
        SDL_Renderer *renderer;
        float *dt;
        SDL_Vertex vertices[16];
    public:
        planet(float _mass, float _radius, position _pos, position _velocity, SDL_Renderer* _renderer, float* _dt) {
            mass = _mass;
            radius = _radius;
            pos = _pos;
            velocity = _velocity;
            renderer = _renderer;
            dt = _dt;

            for (size_t i = 0; i < CIRCLE_VERTICES; i++) {
                const float current_angle = circle_angle * i;
                vertices[i].position.x = std::cos(current_angle) * radius * pixels_in_metre;
                vertices[i].position.y = std::sin(current_angle) * radius * pixels_in_metre;
                vertices[i].color.r = 1.0f;
                vertices[i].color.g = 1.0f;
                vertices[i].color.b = 1.0f;
                vertices[i].color.a = 1.0f;
            }
        }

        void update(planet _planet) {
            const position proj = {_planet.pos.x - pos.x, _planet.pos.y - pos.y};
            const float dist_sqr = proj.x * proj.x + proj.y * proj.y;
            const float acc_dt = (dist_sqr != 0) ? (_planet.mass / dist_sqr * G_CONST * *dt) : 0.0f;
            const float dist = std::sqrt(dist_sqr);
            velocity.x += proj.x / dist * acc_dt;
            velocity.y += proj.y / dist * acc_dt;
            pos.x += velocity.x * *dt;
            pos.y += velocity.y * *dt;
            SDL_Vertex pos_vertices[CIRCLE_VERTICES];

            for (size_t i = 0; i < CIRCLE_VERTICES; i++) {
                pos_vertices[i].position.x = pos.x * pixels_in_metre + half_window_width + vertices[i].position.x;
                pos_vertices[i].position.y = -pos.y * pixels_in_metre + half_window_height + vertices[i].position.y;
                pos_vertices[i].color = vertices[i].color;
            }

            SDL_RenderGeometry(renderer, NULL, pos_vertices, CIRCLE_VERTICES, circle_vertices_indicies, 42);
        }
};

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();

        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return 1;
    }

    float dt;
    planet fp(1000000.0f, 50000.0f, {200000.0f, 0.0f}, {0.0f, 60000.0f}, renderer, &dt);
    planet sp(1.0E25f, 100000.0f, {0.0f, 0.0f}, {0.0f, 0.0f}, renderer, &dt);
    bool window_loop = true;
    Uint64 last_time = 0;

    while (window_loop) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                window_loop = false;
            }
        }

        SDL_RenderClear(renderer);
        const planet _fp = fp;
        fp.update(sp);
        sp.update(_fp);
        SDL_RenderPresent(renderer);
        const Uint64 current_time = SDL_GetTicks();
        dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}