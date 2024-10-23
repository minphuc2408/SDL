#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SNAKE_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
public:
    Snake() {
        direction = RIGHT;
        body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    }

    void handleEvent(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (direction != DOWN) direction = UP;
                    break;
                case SDLK_DOWN:
                    if (direction != UP) direction = DOWN;
                    break;
                case SDLK_LEFT:
                    if (direction != RIGHT) direction = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (direction != LEFT) direction = RIGHT;
                    break;
            }
        }
    }

    void move() {
        Point newHead = body.front();
        switch (direction) {
            case UP: newHead.y -= SNAKE_SIZE; break;
            case DOWN: newHead.y += SNAKE_SIZE; break;
            case LEFT: newHead.x -= SNAKE_SIZE; break;
            case RIGHT: newHead.x += SNAKE_SIZE; break;
        }
        body.insert(body.begin(), newHead);
        body.pop_back();
    }

    void grow() {
        body.push_back(body.back());
    }

    bool checkCollision() {
        Point head = body.front();
        if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
            return true;
        }
        for (size_t i = 1; i < body.size(); ++i) {
            if (body[i].x == head.x && body[i].y == head.y) {
                return true;
            }
        }
        return false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        for (const Point& p : body) {
            SDL_Rect fillRect = { p.x, p.y, SNAKE_SIZE, SNAKE_SIZE };
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }

    Point getPosition() const {
        return body.front();
    }

private:
    std::vector<Point> body;
    Direction direction;
};

class Food {
public:
    Food() {
        srand(static_cast<unsigned int>(time(0)));
        respawn();
    }

    void respawn() {
        position.x = (rand() % (SCREEN_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
        position.y = (rand() % (SCREEN_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_Rect fillRect = { position.x, position.y, SNAKE_SIZE, SNAKE_SIZE };
        SDL_RenderFillRect(renderer, &fillRect);
    }

    Point getPosition() const {
        return position;
    }

private:
    Point position;
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }



    Snake snake;
    Food food;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            snake.handleEvent(e);
        }

        snake.move();

        if (snake.checkCollision()) {
            snake = Snake(); // Reset snake
            food = Food();   // Reset food
        }

        if (snake.getPosition() == food.getPosition()) {
            snake.grow();
            food.respawn();
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        snake.render(renderer);
        food.render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}