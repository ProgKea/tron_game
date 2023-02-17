#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <assert.h>

#define RAYGUI_IMPLEMENTATION
#include "./raygui.h"

#define FPS 240
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400
#define BG_COLOR           \
    CLITERAL(Color)        \
    {                      \
        232, 220, 184, 255 \
    }

#define PLAYER_WIDTH 25
#define PLAYER_HEIGHT 10
#define PLAYER_MOVEMENT_SPEED 100
#define PLAYER_ROTATION_SPEED 200
#define PLAYER_HEAD_SIZE 10
#define PLAYER_BODY_COLOR BEIGE
#define PLAYER_HEAD_COLOR BROWN

#define TRAIL_CAP 100
#define TRAIL_INIT_MAX 100
#define TRAIL_SIZE 5
#define TRAIL_UPDATE FPS
#define TRAIL_COLOR DARKGRAY

#define SCORE_SIZE 150
#define SCORE_COLOR WHITE

typedef enum {
    Menu = 0,
    Game,
} TronState;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float angle;
} Player;

typedef struct {
    Vector2 pos;
    float angle;
} TrailPart;

typedef struct {
    Player player;

    int score;

    TronState game_state;

    // Trail
    TrailPart trail[TRAIL_CAP];
    int trail_index;
    int trail_max;
    int trail_update;
} Tron;

float degrees_to_radians(float degrees);

void tron_render_score(Tron *t);
void tron_render_player(Tron *t);
void tron_update_player(Tron *t);
void tron_extend_trail(Tron *t);
void tron_render_trail(TrailPart *trail_p);
void tron_check_collision_trail(Tron *t, TrailPart *trail_p);
void tron_init(Tron *t);
void tron_render_menu(Tron *t);
void tron_update_menu(Tron *t);
void tron_update_game(Tron *t);

float degrees_to_radians(float degrees)
{
    return degrees * (PI / 180);
}

int count_digits(int num)
{
    assert(num >= 0);
    if (num == 0) return 1;

    int digit_count = 0;
    while (num != 0) {
        num /= 10;
        digit_count++;
    }

    return digit_count;
}

void tron_render_score(Tron *t)
{
    // This is temporary
    t->score = t->trail_index;

    int digits = count_digits(t->score);
    char score_text[10];
    sprintf(score_text, "%d", t->score);
    DrawText(score_text, WINDOW_WIDTH / 2 - ((SCORE_SIZE / 2) / 2) * digits, WINDOW_HEIGHT / 2 - SCORE_SIZE / 2, SCORE_SIZE, SCORE_COLOR);
}

void tron_render_player(Tron *t)
{
    Player *p = &t->player;
    Rectangle p_rect = {
        p->pos.x,
        p->pos.y,
        PLAYER_WIDTH,
        PLAYER_HEIGHT,
    };

    DrawRectanglePro(p_rect, (Vector2){p_rect.width / 2, p_rect.height / 2}, p->angle, PLAYER_BODY_COLOR);
}

void tron_update_player(Tron *t)
{
    Player *p = &t->player;

    if (IsKeyDown(KEY_LEFT)) p->angle -= PLAYER_ROTATION_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) p->angle += PLAYER_ROTATION_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_R)) tron_init(t);
    if (p->angle > 360) p->angle = 0;
    if (p->angle < 0) p->angle = 360;

    p->pos.x += (PLAYER_MOVEMENT_SPEED * cos(degrees_to_radians(p->angle))) * GetFrameTime();
    p->pos.y += (PLAYER_MOVEMENT_SPEED * sin(degrees_to_radians(p->angle))) * GetFrameTime();

    if (p->pos.x > WINDOW_WIDTH) p->pos.x = 0;
    if (p->pos.y > WINDOW_HEIGHT) p->pos.y = 0;
    if (p->pos.x < 0) p->pos.x = WINDOW_WIDTH;
}

void tron_extend_trail(Tron *t)
{
    if (t->trail_update < TRAIL_UPDATE) {
        t->trail_update += 1;
        return;
    }

    Player *p = &t->player;

    if (t->trail_index >= t->trail_max) {
        t->trail_index = 0;
    }

    float trail_x = p->pos.x + -PLAYER_WIDTH / 2 * cos(degrees_to_radians(p->angle));
    float trail_y = p->pos.y + -PLAYER_WIDTH / 2 * sin(degrees_to_radians(p->angle));

    t->trail[t->trail_index] = (TrailPart){{trail_x, trail_y}, p->angle};
    t->trail_index += 1;
    t->trail_update = 0;
}

void tron_render_trail(TrailPart *trail_p)
{
    DrawCircleV((Vector2){trail_p->pos.x, trail_p->pos.y}, TRAIL_SIZE, TRAIL_COLOR);
}

void tron_check_collision_trail(Tron *t, TrailPart *trail_p)
{
    Player *p = &t->player;
    float player_head_x1 = p->pos.x + PLAYER_WIDTH / 2 * cos(degrees_to_radians(p->angle));
    float player_head_y1 = p->pos.y + PLAYER_WIDTH / 2 * sin(degrees_to_radians(p->angle));
    player_head_x1 += PLAYER_HEIGHT / 2 * cos(degrees_to_radians(p->angle - 90));
    player_head_y1 += PLAYER_HEIGHT / 2 * sin(degrees_to_radians(p->angle - 90));

    float player_head_x2 = p->pos.x + PLAYER_WIDTH / 2 * cos(degrees_to_radians(p->angle));
    float player_head_y2 = p->pos.y + PLAYER_WIDTH / 2 * sin(degrees_to_radians(p->angle));
    player_head_x2 += PLAYER_HEIGHT / 2 * cos(degrees_to_radians(p->angle + 90));
    player_head_y2 += PLAYER_HEIGHT / 2 * sin(degrees_to_radians(p->angle + 90));

    float player_head_x3 = p->pos.x + PLAYER_WIDTH / 2 * cos(degrees_to_radians(p->angle));
    float player_head_y3 = p->pos.y + PLAYER_WIDTH / 2 * sin(degrees_to_radians(p->angle));

    DrawLineEx((Vector2){player_head_x1, player_head_y1}, (Vector2){player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE, PLAYER_HEAD_COLOR);

    if (CheckCollisionPointCircle((Vector2){player_head_x1, player_head_y1}, (Vector2){trail_p->pos.x, trail_p->pos.y}, TRAIL_SIZE) || CheckCollisionPointCircle((Vector2){player_head_x2, player_head_y2}, (Vector2){trail_p->pos.x, trail_p->pos.y}, TRAIL_SIZE) || CheckCollisionPointCircle((Vector2){player_head_x3, player_head_y3}, (Vector2){trail_p->pos.x, trail_p->pos.y}, TRAIL_SIZE)) tron_init(t);
}

void tron_init(Tron *t)
{
    t->player = (Player){
        (Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2},
        (Vector2){0.0, 0.0},
        0,
    };

    t->score = 0;
    t->game_state = Menu;

    t->trail_index = 0;
    t->trail_update = 0;
    t->trail_max = TRAIL_INIT_MAX;
    for (int i = 0; i < TRAIL_CAP; i++) {
        t->trail[i] = (TrailPart){0};
    }
}

void tron_update_menu(Tron *t)
{
    float play_button_width = WINDOW_WIDTH;
    float play_button_height = WINDOW_HEIGHT;
    if (GuiButton((Rectangle) {WINDOW_WIDTH/2 - play_button_width/2, WINDOW_HEIGHT/2 - play_button_height/2, play_button_width, play_button_height}, "Play")) t->game_state = Game;
}

void tron_update_game(Tron *t)
{
    tron_render_score(t);
    tron_extend_trail(t);

    for (int i = 0; i < TRAIL_CAP; i++) {
        TrailPart *trail_p = &t->trail[i];
        tron_render_trail(trail_p);
        tron_check_collision_trail(t, trail_p);
    }

    tron_render_player(t);
    tron_update_player(t);
}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dotron");
    SetTargetFPS(FPS);

    Tron t = {0};
    tron_init(&t);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BG_COLOR);
            DrawFPS(0, 0);

            switch (t.game_state) {
            case Menu:
                tron_update_menu(&t);
                break;
            case Game:
                tron_update_game(&t);
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// TODO: Write a shader so the trail points throw a shadow
// TODO: add a way to increase score (either time spend near a trail or trails placed)
// TODO: implement a shop where you can buy upgrades that change turning speed, decrease the players height and more
// TODO: remove the random circle at the top right corner
