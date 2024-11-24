#include <bits/stdc++.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20

typedef struct {
    int x, y;
} Point;

int food_x, food_y;
Point snake[SCREEN_WIDTH * SCREEN_HEIGHT / CELL_SIZE / CELL_SIZE];
int snake_len = 3;
int snake_dir = 1; // 1: right, 2: left, 3: up, 4: down

// Creating Snake
void init_snake()
{
    // Initialize the snake's starting position
    snake[0].x = SCREEN_WIDTH / 2 / CELL_SIZE;
    snake[0].y = SCREEN_HEIGHT / 2 / CELL_SIZE;
}

// Generating food
void generate_food()
{
    srand(time(NULL));
    food_x = rand() % (SCREEN_WIDTH / CELL_SIZE);
    food_y = rand() % (SCREEN_HEIGHT / CELL_SIZE);
}

// Key Input
void handle_events(bool *game_over)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) 
        {
            *game_over = true;
        } 
        
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    if (snake_dir != 4) snake_dir = 3;
                    break;
                case SDLK_DOWN:
                    if (snake_dir != 3) snake_dir = 4;
                    break;
                case SDLK_LEFT:
                    if (snake_dir != 1) snake_dir = 2;
                    break;
                case SDLK_RIGHT:
                    if (snake_dir != 2) snake_dir = 1;
                    break;
            }
        }
    }
}

// Movement of Snake
void move_snake()
{
    for (int i = snake_len - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    switch (snake_dir) 
    {
        case 1: snake[0].x++; break;
        case 2: snake[0].x--; break;
        case 3: snake[0].y--; break;
        case 4: snake[0].y++; break;
    }
}

// Collison
bool check_collision() 
{
    // Collision with walls
    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH / CELL_SIZE || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT / CELL_SIZE)
    {
        return true;
    }

    // Collision with itself
    for (int i = 1; i < snake_len; i++)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) 
        {
            return true;
        }
    }

    return false;
}

// Check if the snake eats the food
bool consuming_food() 
{
    if (snake[0].x == food_x && snake[0].y == food_y)
    {
        snake_len++;
        generate_food();
        return true;
    }

    return false;
}

void render_game(SDL_Renderer *renderer) 
{
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the snake
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < snake_len; i++)
    {
        SDL_Rect rect = {snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw the food
    SDL_Rect food_rect = {food_x * CELL_SIZE, food_y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &food_rect);

    // Present the rendered objects
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize game
    init_snake();
    generate_food();

    bool game_over = false;
    while (!game_over) 
    {
        handle_events(&game_over);

        move_snake();

        if (check_collision()) 
        {
            game_over = true;
        }

        consuming_food();

        render_game(renderer);

        SDL_Delay(100); // Adjust the delay to control the game speed
    }

    // Erase the setup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
