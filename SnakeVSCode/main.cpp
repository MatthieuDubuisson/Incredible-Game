#include <raylib.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define SNAKE_LENGTH   256
#define SQUARE_SIZE     38

// commands : cd build, mingw32-make

//structures
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
} Food;



// Global Variables Declaration

static const int screenWidth = 1080;
static const int screenHeight = 720;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

static Food fruit = { 0 };
static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;

static void InitGame(void);
static void UpdateGame(void); 
static void DrawGame(void);
static void UnloadGame(void);
static void UpdateDrawFrame(void);


int main(void)
{

    InitWindow(screenWidth, screenHeight, "Snake game with raylib (by Matthieu Dubuisson)");

    InitGame();
   

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update and Draw
        UpdateDrawFrame();
    }
#endif
    // De-Initialization
    UnloadGame();
 
    
    CloseWindow();     

    return 0;
}


// Initialize game variables
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;
    
    counterTail = 1;
    allowMove = false;

    offset.x = screenWidth%SQUARE_SIZE;
    offset.y = screenHeight%SQUARE_SIZE;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ offset.x/2, offset.y/2 };
        snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

        if (i == 0) snake[i].color = DARKGREEN;
        else snake[i].color = GREEN;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruit.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
    fruit.color = ORANGE;
    fruit.active = false;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;
        
        if (!pause)
        {
            // Player controller
            if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
            }

            // Snake move
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter%5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i-1];
                }
            }

            // Wall limits
            if (((snake[0].position.x) > (screenWidth - offset.x)) || 
                ((snake[0].position.y) > (screenHeight - offset.y)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                gameOver = true;
            }

            // snake itself collision
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
            }

            // Fruit position
            if (!fruit.active)
            {
                fruit.active = true;
                fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/SQUARE_SIZE) - 1)*SQUARE_SIZE + offset.x/2, GetRandomValue(0, (screenHeight/SQUARE_SIZE) - 1)*SQUARE_SIZE + offset.y/2 };

                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
                    {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/SQUARE_SIZE) - 1)*SQUARE_SIZE + offset.x/2, GetRandomValue(0, (screenHeight/SQUARE_SIZE) - 1)*SQUARE_SIZE + offset.y/2 };
                        i = 0;
                    }
                }
            }

            // Collision
            if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
                (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
            }

            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game

void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(LIGHTGRAY);

        if (!gameOver)
        {
            Texture2D background = LoadTexture("D:/ARTFX/GAME_2/BandeDemo/Snake/SnakeVSCode/resources/background.png");
            DrawTexture(background, 0, 0, RAYWHITE);
            // background
            //DrawTexture(background, screenWidth/2, screenHeight/2, WHITE);
            // Draw snake
            for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

            // Draw fruit to pick
            DrawRectangleV(fruit.position, fruit.size, fruit.color);

            if (pause) DrawText("PAUSED", screenWidth/2 - MeasureText("PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER]", GetScreenWidth()/2 - MeasureText("PRESS [ENTER]", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    //UnloadTexture(background);     
}

// Update and Draw
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
