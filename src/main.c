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

void handlePlayerInput(paddleEntity *self);
void updatePlayer(paddleEntity *self, const float dy);
void drawPaddleEntity(paddleEntity *self);
void updateCpu(paddleEntity *self, const ballEntity *ball, const paddleEntity *player, const float dy);
paddleEntity createPaddleEntity(Rectangle rect, Color color);

enum game_mode {
	VS_CPU,
	VS_BALL,
	VS_PLAYER2
};

const float player_dy = 5;
const float cpu_dy = 5;

const float MAX_SPEED = 15;
float speedup = 1.1f;
float ball_dx = -2;
float ball_dy = 2;

enum end_state {
    YOU_WIN,
    YOU_LOSE
};

int main()
{
    Rectangle p1_rect = (Rectangle){64, SCREEN_HEIGHT/2 - 60, 20, 120}; 
    paddleEntity p1_paddle = createPaddleEntity(p1_rect, WHITE);
    int p1_score = 0;

    Rectangle p2_rect = (Rectangle){SCREEN_WIDTH - 84, SCREEN_HEIGHT/2 - 60, 20, 120};
    paddleEntity p2_paddle = createPaddleEntity(p2_rect, WHITE);

    ballEntity ball = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, {16, WHITE}};
    Vector2 ball_center = (Vector2){ball.x, ball.y};

    enum end_state endState;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    SetTargetFPS(60);

    bool gameOver = false;
    while(!WindowShouldClose()) {
        if (!gameOver) {
            handlePlayerInput(&p1_paddle);
            updateCpu(&p2_paddle, &ball, &p1_paddle, cpu_dy);

            if(CheckCollisionCircleRec(ball_center, ball.sprite.radius, p1_paddle.rect) ||
                CheckCollisionCircleRec(ball_center, ball.sprite.radius, p2_paddle.rect)) {
                if(ball_dx*ball_dx < MAX_SPEED*MAX_SPEED) {
                    ball_dx *= speedup;
                }
                ball_dx = -ball_dx;
            }

            if(CheckCollisionCircleRec(ball_center, ball.sprite.radius, p1_paddle.rect)) {
				p1_score++;
            }

            if (ball.y + ball.sprite.radius > SCREEN_HEIGHT || ball.y - ball.sprite.radius < 0) {
                ball_dy = -ball_dy;
            }

            if (ball.x - ball.sprite.radius <= 0.0) {
                gameOver = true;
                endState = YOU_LOSE;
            }

            if (ball.x + ball.sprite.radius >= SCREEN_WIDTH) {
				ball.x = SCREEN_WIDTH/2;
				ball.y = SCREEN_HEIGHT/2;
            }

            updatePlayer(&p1_paddle, player_dy);

            ball.x += ball_dx;
            ball.y += ball_dy;
            ball_center = (Vector2){ball.x, ball.y};

			char p1_scoreboard[16];
			sprintf(p1_scoreboard, "Score: %03d", p1_score);
            BeginDrawing();
			ClearBackground(BLACK);
			drawPaddleEntity(&p1_paddle);
			drawPaddleEntity(&p2_paddle);
			DrawCircle(ball.x, ball.y, ball.sprite.radius, ball.sprite.color);
			DrawText(p1_scoreboard, 10, 10, 48, WHITE);
            EndDrawing();
        } else {
			char p1_finalScore[16];
			sprintf(p1_finalScore, "Score: %03d", p1_score);
            switch (endState)
            {
            case YOU_WIN:
                BeginDrawing();
				ClearBackground(BLACK);
				DrawText("A WINNER IS YOU!", SCREEN_WIDTH*0.25, (SCREEN_HEIGHT/2) - 16, 32, WHITE);
                EndDrawing();
                break;
            case YOU_LOSE:
                BeginDrawing();
				ClearBackground(BLACK);
				DrawText(p1_finalScore, 10, 10, 48, WHITE);
				DrawText("A LOSER IS YOU!", SCREEN_WIDTH*0.25, (SCREEN_HEIGHT/2) - 16, 32, WHITE);
                EndDrawing();
            }
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

void updateCpu(paddleEntity *self, const ballEntity *ball, const paddleEntity *player, const float dy)
{
    int paddleDistance = self->rect.x - (2 * (player->rect.x + player->rect.width));    // line between sides facing each other
    if(ball->x > paddleDistance * 0.66) {
        if(self->rect.y - dy >= 0)
        {
            if(ball->y < self->rect.y - 10) {
                self->rect.y -= dy;
            }
        }

        if(self->rect.y + self->rect.height + cpu_dy <= SCREEN_HEIGHT)
        {
            if(ball->y > self->rect.y + self->rect.height - 10) {
                self->rect.y += dy;
            }
        }
    }
}
