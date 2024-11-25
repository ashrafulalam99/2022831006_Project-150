#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 720
#define CELL_SIZE 20
#define NUM_OBSTACLES 3 // Number of line obstacles
#define OBSTACLE_WIDTH 30
#define OBSTACLE_HEIGHT 1

typedef struct 
{
    int x, y;
} Point;

//Initialize Snake
int food_x, food_y, bonus_food_x, bonus_food_y;
Point snake[SCREEN_WIDTH * SCREEN_HEIGHT / CELL_SIZE / CELL_SIZE]; // Creation of Snake
int snake_len = 3;

int snake_direction = 1; // 1: right, 2: left, 3: up, 4: down
int score = 0, bonus_food_counter = 0;
bool bonus_food_active = false;

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void draw_snake(SDL_Renderer *renderer)
{
    // Draw the head as a circle
    SDL_SetRenderDrawColor(renderer, 0, 153, 0, 255);
    int head_x = snake[0].x * CELL_SIZE + CELL_SIZE / 2;
    int head_y = snake[0].y * CELL_SIZE + CELL_SIZE / 2;
    int head_radius = CELL_SIZE / 2 + 2;
    draw_circle(renderer, head_x, head_y, head_radius);

    // Draw the rest of the snake as rectangles
    for (int i = 1; i < snake_len; i++)
    {
        SDL_Rect rect = {snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

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
void generate_bonus_food()
{
    bonus_food_x = rand() % (SCREEN_WIDTH / CELL_SIZE);
    bonus_food_y = rand() % (SCREEN_HEIGHT / CELL_SIZE);

    for (int i = 0; i < snake_len; i++)
    {
        if (bonus_food_x == snake[i].x && bonus_food_y == snake[i].y) {
            generate_bonus_food();
        }
    }

    if (bonus_food_x == food_x && bonus_food_y == food_y)
    {
        generate_bonus_food();
    }
}

void draw_food(SDL_Renderer *renderer, bool isBonusFood, int x, int y)
{
    if (isBonusFood)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } 
    
    else
    {
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
                    if (snake_direction != 4) snake_direction = 3;
                    break;
                case SDLK_DOWN:
                    if (snake_direction != 3) snake_direction = 4;
                    break;
                case SDLK_LEFT:
                    if (snake_direction != 1) snake_direction = 2;
                    break;
                case SDLK_RIGHT:
                    if (snake_direction != 2) snake_direction = 1;
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

    switch (snake_direction) 
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
void consuming_food() 
{
    if (snake[0].x == food_x && snake[0].y == food_y)
    {
        snake_len++;
        score += 10;
        bonus_food_counter++;
        generate_food();
        snake[snake_len - 1] = snake[snake_len - 2];
    }
}

// Check if the snake eats the food
void consuming_bonus_food()
{
    if (bonus_food_counter == 5 && !bonus_food_active)
    {
        generate_bonus_food();
        bonus_food_active = true;
        bonus_food_counter = 0;
    }

    if (bonus_food_active && snake[0].x == bonus_food_x && snake[0].y == bonus_food_y) {
        
        for(int i = 1; i <= 3; i++)
        {   snake_len += 1;
            snake[snake_len - 1] = snake[snake_len - 2];
        }

        score += 50;
        bonus_food_active = false;
    }
}

//Displaying Score
void render_score(SDL_Renderer *renderer, TTF_Font *font, int score)
{
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

#define MAX_LINE_LENGTH 50 // Maximum length of the obstacle line

Point obstacle_line[MAX_LINE_LENGTH];

int obstacle_length = 0;

// Function to generate a single line-shaped obstacle
void generate_obstacle() {
    srand(time(NULL));

    // Randomly decide if the line is horizontal or vertical
    bool is_horizontal = rand() % 2;

    // Starting point of the line
    int start_x = rand() % (SCREEN_WIDTH / CELL_SIZE);
    int start_y = rand() % (SCREEN_HEIGHT / CELL_SIZE);

    // Random length of the line
    obstacle_length = rand() % (MAX_LINE_LENGTH - 5) + 5; // Minimum length of 5

    for (int i = 0; i < obstacle_length; i++) {
        if (is_horizontal) {
            obstacle_line[i].x = (start_x + i) % (SCREEN_WIDTH / CELL_SIZE);
            obstacle_line[i].y = start_y;
        } else {
            obstacle_line[i].x = start_x;
            obstacle_line[i].y = (start_y + i) % (SCREEN_HEIGHT / CELL_SIZE);
        }
    }
}

// Function to draw the obstacle line
void draw_obstacle(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray color for the line
    for (int i = 0; i < obstacle_length; i++) {
        SDL_Rect rect = {
            obstacle_line[i].x * CELL_SIZE,
            obstacle_line[i].y * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}
// Function to check collision with the single obstacle
bool check_obstacle_collision() {
    for (int i = 0; i < obstacle_length; i++) {
        if (snake[0].x == obstacle_line[i].x && snake[0].y == obstacle_line[i].y) {
            return true;
        }
    }
    return false;
}

void restart_game()
{
    snake_len = 3;
    snake_direction = 1;
    score = 0;
    bonus_food_counter = 0;
    bonus_food_active = false;

    snake[0] = (Point){SCREEN_WIDTH / 2 / CELL_SIZE, SCREEN_HEIGHT / 2 / CELL_SIZE};
    generate_food();
}

//Displaying Game Over and final score
void display_game_over(SDL_Renderer *renderer, TTF_Font *font, int score) 
{
    //Coloring
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color reddish_orange = {255, 128, 0, 255};

    TTF_Font *large_font = TTF_OpenFont("arial.ttf", 48); // Adjust font size
    SDL_Surface *game_over_surface = TTF_RenderText_Solid(large_font, "GAME OVER!", red);
    SDL_Texture *game_over_texture = SDL_CreateTextureFromSurface(renderer, game_over_surface);
    SDL_Rect game_over_rect = {SCREEN_WIDTH / 2 - game_over_surface->w / 2, SCREEN_HEIGHT / 2 - game_over_surface->h / 2 - 50, game_over_surface->w, game_over_surface->h};

    char score_text[50];
    sprintf(score_text, "Final Score: %d", score);
    SDL_Surface *score_surface = TTF_RenderText_Solid(font, score_text, white);
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    SDL_Rect score_rect = {SCREEN_WIDTH / 2 - score_surface->w / 2, SCREEN_HEIGHT / 2 - score_surface->h / 2 + 50, score_surface->w, score_surface->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, game_over_texture, NULL, &game_over_rect);
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    SDL_RenderPresent(renderer);

    char restart_text[30] = "Press ENTER to restart";
    SDL_Surface *restart_surface = TTF_RenderText_Solid(font, restart_text, reddish_orange);
    SDL_Texture *restart_texture = SDL_CreateTextureFromSurface(renderer, restart_surface);

    SDL_Rect restart_rect = {SCREEN_WIDTH / 2 - restart_surface->w / 2, SCREEN_HEIGHT / 2 + 100, restart_surface->w, restart_surface->h};

    SDL_RenderCopy(renderer, restart_texture, NULL, &restart_rect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(game_over_surface);
    SDL_FreeSurface(score_surface);
    SDL_FreeSurface(restart_surface);
    SDL_DestroyTexture(game_over_texture);
    SDL_DestroyTexture(score_texture);
    SDL_DestroyTexture(restart_texture);

    SDL_Delay(3000);

    bool restart = false;
    while (!restart)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    restart = true;
                }
            }
        }
    }
}


int main(int argc, char *argv[])
{
    // Initialize SDL and TTF
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        printf("Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize Window, Renderer, font
    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("arial.ttf", 28);

    if (!window || !renderer || !font) {
        printf("Error creating resources: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize game state
    snake[0] = (Point){SCREEN_WIDTH / 2 / CELL_SIZE, SCREEN_HEIGHT / 2 / CELL_SIZE};
    generate_food();
    generate_obstacle(); // Generate obstacle only once

    bool game_over = false;
    while (true)
    {
        while (!game_over)
        {
            handle_events(&game_over);

            move_snake();

            // Check for wall or self-collision
            if (check_collision()) {
                game_over = true;
            }

            // Check for obstacle collision
            if (check_obstacle_collision()) {
                // Pause game and handle input
                SDL_Event event;
                bool resolved = false;

                // Display collision message
                printf("You hit an obstacle! Press Y to continue (lose 10 points) or N to quit.\n");

                while (!resolved) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_KEYDOWN) {
                            switch (event.key.keysym.sym) {
                                case SDLK_y: // Lose points and continue
                                    score -= 10;
                                    resolved = true;
                                    break;
                                case SDLK_n: // Quit game
                                    game_over = true;
                                    resolved = true;
                                    break;
                            }
                        }
                    }
                }
            }

            consuming_food();
            consuming_bonus_food();

            SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
            SDL_RenderClear(renderer);

            draw_snake(renderer);
            draw_food(renderer, false, food_x, food_y);
            draw_obstacle(renderer); // Draw the obstacle

            if (bonus_food_active) {
                draw_food(renderer, true, bonus_food_x, bonus_food_y);
            }

            render_score(renderer, font, score);
            SDL_RenderPresent(renderer);
            SDL_Delay(75); // Game speed
        }

        display_game_over(renderer, font, score);
        restart_game();

        game_over = false;
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}