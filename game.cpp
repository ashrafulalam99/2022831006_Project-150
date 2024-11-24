#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20

typedef struct {
    int x, y;
} Point;

int food_x, food_y, bonus_food_x, bonus_food_y;
Point snake[SCREEN_WIDTH * SCREEN_HEIGHT / CELL_SIZE / CELL_SIZE];
int snake_len = 3;
int snake_dir = 1; // 1: right, 2: left, 3: up, 4: down
int score = 0, bonus_food_counter = 0;
bool bonus_food_active = false;

//Generating food
void generate_food() 
{
    srand(time(NULL));
    food_x = rand() % (SCREEN_WIDTH / CELL_SIZE);
    food_y = rand() % (SCREEN_HEIGHT / CELL_SIZE);

    for (int i = 0; i < snake_len; i++) {
        if (food_x == snake[i].x && food_y == snake[i].y) {
            generate_food();
        }
    }
}

//Generating Bonus Food
void generate_bonus_food() {
    bonus_food_x = rand() % (SCREEN_WIDTH / CELL_SIZE);
    bonus_food_y = rand() % (SCREEN_HEIGHT / CELL_SIZE);

    for (int i = 0; i < snake_len; i++) {
        if (bonus_food_x == snake[i].x && bonus_food_y == snake[i].y) {
            generate_bonus_food();
        }
    }
    if (bonus_food_x == food_x && bonus_food_y == food_y) {
        generate_bonus_food();
    }
}

//Shaping the food as circle
void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void draw_food(SDL_Renderer *renderer, bool isBonusFood, int x, int y) {
    if (isBonusFood) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 204, 204, 255);
    }

    int center_x = x * CELL_SIZE + CELL_SIZE / 2;
    int center_y = y * CELL_SIZE + CELL_SIZE / 2;
    int radius = (isBonusFood ? CELL_SIZE : CELL_SIZE / 2) - 2; //Bonus food has double radius

    draw_circle(renderer, center_x, center_y, radius);
}

//Key Input
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
        score += 10;
        bonus_food_counter++;
        generate_food();
        return true;
    }

    return false;
}

// Check if the snake eats the food
void consuming_bonus_food() {
    if (bonus_food_counter == 5 && !bonus_food_active) {
        generate_bonus_food();
        bonus_food_active = true;
        bonus_food_counter = 0;
    }

    if (bonus_food_active && snake[0].x == bonus_food_x && snake[0].y == bonus_food_y) {
        snake_len += 2;
        score += 50;
        bonus_food_active = false;
    }
}

void draw_snake(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 153, 0, 255);
    for (int i = 0; i < snake_len; i++) {
        SDL_Rect rect = {snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

//Displaying Score
void render_score(SDL_Renderer *renderer, TTF_Font *font, int score) {
    SDL_Color white = {255, 255, 255, 255};
    char score_text[20];
    sprintf(score_text, "Score: %d", score);

    SDL_Surface *surface = TTF_RenderText_Solid(font, score_text, white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect score_rect = {SCREEN_WIDTH - surface->w - 10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &score_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

//Displaying Game Over
void display_game_over(SDL_Renderer *renderer, TTF_Font *font, int score) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};

    SDL_Surface *game_over_surface = TTF_RenderText_Solid(font, "Game Over", red);
    SDL_Texture *game_over_texture = SDL_CreateTextureFromSurface(renderer, game_over_surface);

    SDL_Rect game_over_rect = {SCREEN_WIDTH / 2 - game_over_surface->w / 2, SCREEN_HEIGHT / 2 - game_over_surface->h / 2 - 50, game_over_surface->w, game_over_surface->h};

    char score_text[50];
    sprintf(score_text, "Your Score: %d", score);
    SDL_Surface *score_surface = TTF_RenderText_Solid(font, score_text, white);
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);

    SDL_Rect score_rect = {SCREEN_WIDTH / 2 - score_surface->w / 2, SCREEN_HEIGHT / 2 - score_surface->h / 2 + 50, score_surface->w, score_surface->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, game_over_texture, NULL, &game_over_rect);
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(game_over_surface);
    SDL_FreeSurface(score_surface);
    SDL_DestroyTexture(game_over_texture);
    SDL_DestroyTexture(score_texture);

    SDL_Delay(5000);
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        printf("Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("arial.ttf", 28);

    if (!window || !renderer || !font) {
        printf("Error creating resources: %s\n", SDL_GetError());
        return 1;
    }

    snake[0] = (Point){SCREEN_WIDTH / 2 / CELL_SIZE, SCREEN_HEIGHT / 2 / CELL_SIZE};
    generate_food();

    bool game_over = false;
    while (!game_over) {
        handle_events(&game_over);

        move_snake();

        if (check_collision()) {
            game_over = true;
        }
        consuming_food();
        consuming_bonus_food();

        SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
        SDL_RenderClear(renderer);

        draw_snake(renderer);
        draw_food(renderer, false, food_x, food_y);
        if (bonus_food_active) {
            draw_food(renderer, true, bonus_food_x, bonus_food_y);
        }
        render_score(renderer, font, score);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    display_game_over(renderer, font, score);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
