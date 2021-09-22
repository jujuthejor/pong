/* MIT License
 *
 * Copyright (c) 2021 Jose Gabriel Ramos (@jujuthejor)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// DONE: Score system
// DONE: increasing difficulty for standard 1 player mode
// TODO: 2 player and alternative 1 player modes
// TODO: start screen
#include<stddef.h>
#include<stdio.h>

#include "raylib.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

enum GameMode {
    GAMEMODE_TITLE_SCREEN,
    GAMEMODE_1PLAYER,
    GAMEMODE_2PLAYER,
    GAMEMODE_1P_ALTERNATIVE,
    GAMEMODE_END
};

enum EndState {
    YOU_WIN,
    YOU_LOSE
};

typedef struct circle {
    int radius;
    Color color;
} circle;

typedef struct ballEntity {
    float x;
    float y;
    circle sprite;
} ballEntity;

typedef struct paddleEntity{
    Rectangle rect;
    enum direction {PADDLE_STOP, PADDLE_UP, PADDLE_DOWN} direction;
    Color color;
} paddleEntity;

typedef struct speeds {
    const float PLAYER_DY;
    const float CPU_DY;
    const float BALL_MAX_SPEED;
    float speedup;
    float ball_dx;
    float ball_dy;
} Speeds;

void handlePlayerInput(paddleEntity *self);
void handlePlayer2Input(paddleEntity *self);
void updatePlayer(paddleEntity *self, const float dy);
void drawPaddleEntity(paddleEntity *self);
void updateCpu(paddleEntity *self, const ballEntity *ball,
        const paddleEntity *player, Speeds speeds);
paddleEntity createPaddleEntity(Rectangle rect, Color color);

void run_1P_mode(paddleEntity *p1_paddle, paddleEntity *p2_paddle,
        ballEntity *ball, Vector2 *ball_center, Speeds *speeds,
        int *p1_score, char p1_scoreboard[], bool *gameOver, enum EndState *endState);

int main()
{
    Rectangle p1_rect = (Rectangle){64, (float)SCREEN_HEIGHT/2 - 60, 20, 120};
    paddleEntity p1_paddle = createPaddleEntity(p1_rect, WHITE);
    int p1_score = 0;

    Rectangle p2_rect = (Rectangle){SCREEN_WIDTH - 84,
        (float)SCREEN_HEIGHT/2 - 60, 20, 120};
    paddleEntity p2_paddle = createPaddleEntity(p2_rect, WHITE);
    int p2_score = 0;

    ballEntity ball = {(float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2, {16, WHITE}};
    Vector2 ball_center = (Vector2){ball.x, ball.y};

    Speeds speeds = {
        .PLAYER_DY = 5,
        .CPU_DY = 5,
        .BALL_MAX_SPEED = 15,
        .speedup = 1.1f,
        .ball_dx = -2,
        .ball_dy = 2,
    };

    enum GameMode gameMode = GAMEMODE_TITLE_SCREEN;
    enum EndState endState = YOU_LOSE;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    SetTargetFPS(60);

    char p1_scoreboard[16], p2_scoreboard[16];
    bool gameOver = false;
    while(!WindowShouldClose()) {
        switch (gameMode) {
        case GAMEMODE_TITLE_SCREEN:
            if(IsKeyPressed(KEY_ONE)) {
                gameMode = GAMEMODE_1PLAYER;
            }

            if(IsKeyPressed(KEY_TWO)) {
                gameMode = GAMEMODE_2PLAYER;
            }

            if(IsKeyPressed(KEY_THREE)) {
                gameMode= GAMEMODE_1P_ALTERNATIVE;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PONG", 10, 10, 48, WHITE);
            DrawText("1 - 1P\n2 - 2P\n3 - 1P Alternative", 10, 60, 32, WHITE);
            EndDrawing();
            break;
        case GAMEMODE_1PLAYER:
            if (!gameOver) {
                run_1P_mode(&p1_paddle, &p2_paddle, &ball, &ball_center, &speeds,
                        &p1_score, p1_scoreboard, &gameOver, &endState);
            } else {
                char p1_finalScore[16];
                sprintf(p1_finalScore, "Score: %03d", p1_score);
                switch (endState)
                {
                case YOU_WIN:
                    BeginDrawing();
                    ClearBackground(BLACK);
                    DrawText("A WINNER IS YOU!", SCREEN_WIDTH*0.25,
                            (SCREEN_HEIGHT/2) - 16, 32, WHITE);
                    EndDrawing();
                    break;
                case YOU_LOSE:
                    BeginDrawing();
                    ClearBackground(BLACK);
                    DrawText(p1_finalScore, 10, 10, 48, WHITE);
                    DrawText("A LOSER IS YOU!", SCREEN_WIDTH*0.25,
                            (SCREEN_HEIGHT/2) - 16, 32, WHITE);
                    EndDrawing();
                }
            }
            break;
        case GAMEMODE_2PLAYER:
            handlePlayerInput(&p1_paddle);
            handlePlayer2Input(&p2_paddle);
            if(CheckCollisionCircleRec(ball_center, ball.sprite.radius,
                        p1_paddle.rect) || CheckCollisionCircleRec(ball_center,
                            ball.sprite.radius, p2_paddle.rect)) {
                if(speeds.ball_dx * speeds.ball_dx < speeds.BALL_MAX_SPEED *
                        speeds.BALL_MAX_SPEED) {
                    speeds.ball_dx *= speeds.speedup;
                }
                speeds.ball_dx = -speeds.ball_dx;
            }

            if (ball.y + ball.sprite.radius > SCREEN_HEIGHT ||
                    ball.y - ball.sprite.radius < 0) {
                speeds.ball_dy = -speeds.ball_dy;
            }

            if (ball.x - ball.sprite.radius <= 0.0) {
                ball.x = (float)SCREEN_WIDTH/2;
                ball.y = (float)SCREEN_HEIGHT/2;
                p2_score++;
            }

            if (ball.x + ball.sprite.radius >= SCREEN_WIDTH) {
                ball.x = (float)SCREEN_WIDTH/2;
                ball.y = (float)SCREEN_HEIGHT/2;
                p1_score++;
            }

            updatePlayer(&p1_paddle,speeds.PLAYER_DY);
            updatePlayer(&p2_paddle, speeds.PLAYER_DY);

            ball.x += speeds.ball_dx;
            ball.y += speeds.ball_dy;
            ball_center = (Vector2){ball.x, ball.y};

            sprintf(p1_scoreboard, "Score: %03d", p1_score);
            sprintf(p2_scoreboard, "Score: %03d", p2_score);
            BeginDrawing();
            ClearBackground(BLACK);
            drawPaddleEntity(&p1_paddle);
            drawPaddleEntity(&p2_paddle);
            DrawCircle(ball.x, ball.y, ball.sprite.radius, ball.sprite.color);
            DrawText(p1_scoreboard, 10, 10, 48, WHITE);
            DrawText(p2_scoreboard, SCREEN_WIDTH/2 + 55, 10, 48, WHITE);
            EndDrawing();
            break;
        case GAMEMODE_1P_ALTERNATIVE:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("AY LMAO", SCREEN_WIDTH*0.25,
                    (SCREEN_HEIGHT/2) - 16, 32, WHITE);
            EndDrawing();
            break;
        case GAMEMODE_END:
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("AY LMAO", SCREEN_WIDTH*0.25,
                    (SCREEN_HEIGHT/2) - 16, 32, WHITE);
            EndDrawing();
            break;
        }

    }
    CloseWindow();
    return 0;
}

paddleEntity createPaddleEntity(Rectangle rect, Color color)
{
    paddleEntity returnEntity;

    returnEntity.rect = rect;
    returnEntity.color = color;

    return returnEntity;
}

void handlePlayerInput(paddleEntity *self)
{
    if(IsKeyDown(KEY_W)) {
        self->direction = PADDLE_UP;
    } else if(IsKeyDown(KEY_S)) {
        self->direction = PADDLE_DOWN;
    } else {
        self->direction = PADDLE_STOP;
    }
}

void handlePlayer2Input(paddleEntity *self)
{
    if(IsKeyDown(KEY_UP)) {
        self->direction = PADDLE_UP;
    } else if(IsKeyDown(KEY_DOWN)) {
        self->direction = PADDLE_DOWN;
    } else {
        self->direction = PADDLE_STOP;
    }
}

void updatePlayer(paddleEntity *self, const float dy)
{
    switch (self->direction)
    {
    case PADDLE_UP:
        if(self->rect.y - dy >= 0) {
            self->rect.y -= dy;
        } else {
            self->rect.y = self->rect.y;
        }
        break;
    case PADDLE_DOWN:
        if(self->rect.y + self->rect.height + dy <= SCREEN_HEIGHT) {
            self->rect.y += dy;
        } else {
            self->rect.y = self->rect.y;
        }
        break;
    default:
        self->rect.y = self->rect.y;
        break;
    }
}

void drawPaddleEntity(paddleEntity *self)
{
    DrawRectangleRec(self->rect, self->color);
}

void updateCpu(paddleEntity *self, const ballEntity *ball,
        const paddleEntity *player, Speeds speeds)
{
    int paddleDistance = self->rect.x - (2 * (player->rect.x + player->rect.width));    // line between sides facing each other
    if(ball->x > paddleDistance * 0.66) {
        if(self->rect.y - speeds.CPU_DY >= 0)
        {
            if(ball->y < self->rect.y - 10) {
                self->rect.y -= speeds.CPU_DY;
            }
        }

        if(self->rect.y + self->rect.height + speeds.CPU_DY <= SCREEN_HEIGHT)
        {
            if(ball->y > self->rect.y + self->rect.height - 10) {
                self->rect.y += speeds.CPU_DY;
            }
        }
    }
}

void run_1P_mode(paddleEntity *p1_paddle, paddleEntity *p2_paddle,
        ballEntity *ball, Vector2 *ball_center, Speeds *speeds,
        int *p1_score, char p1_scoreboard[], bool *gameOver,
        enum EndState *endState)
{
        handlePlayerInput(p1_paddle);
        updateCpu(p2_paddle, ball, p1_paddle, *speeds);

        if(CheckCollisionCircleRec(*ball_center, ball->sprite.radius,
                    p1_paddle->rect) || CheckCollisionCircleRec(*ball_center,
                        ball->sprite.radius, p2_paddle->rect)) {
            if(speeds->ball_dx * speeds->ball_dx < speeds->BALL_MAX_SPEED *
                    speeds->BALL_MAX_SPEED) {
                speeds->ball_dx *= speeds->speedup;
            }
            speeds->ball_dx = -speeds->ball_dx;
        }

        if(CheckCollisionCircleRec(*ball_center, ball->sprite.radius,
                    p1_paddle->rect)) {
            (*p1_score)++;
        }

        if (ball->y + ball->sprite.radius > SCREEN_HEIGHT ||
                ball->y - ball->sprite.radius < 0) {
            speeds->ball_dy = -speeds->ball_dy;
        }

        if (ball->x - ball->sprite.radius <= 0.0) {
            *gameOver = true;
            *endState = YOU_LOSE;
        }

        if (ball->x + ball->sprite.radius >= SCREEN_WIDTH) {
            ball->x = (float)SCREEN_WIDTH/2;
            ball->y = (float)SCREEN_HEIGHT/2;
        }

        updatePlayer(p1_paddle, speeds->PLAYER_DY);

        ball->x += speeds->ball_dx;
        ball->y += speeds->ball_dy;
        *ball_center = (Vector2){ball->x, ball->y};

        sprintf(p1_scoreboard, "Score: %03d", *p1_score);
        BeginDrawing();
        ClearBackground(BLACK);
        drawPaddleEntity(p1_paddle);
        drawPaddleEntity(p2_paddle);
        DrawCircle(ball->x, ball->y, ball->sprite.radius, ball->sprite.color);
        DrawText(p1_scoreboard, 10, 10, 48, WHITE);
        EndDrawing();
}
