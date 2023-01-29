#include <stdio.h>
#include <raylib.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define BG_COLOR           \
    CLITERAL(Color)        \
    {                      \
        232, 220, 184, 255 \
    }

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 25
#define PLAYER_MOVEMENT_SPEED 1
#define PLAYER_ROTATION_SPEED 2
#define PLAYER_HEAD_SIZE 10
#define PLAYER_BODY_COLOR BEIGE
#define PLAYER_HEAD_COLOR BROWN

#define TRAIL_CAP 1000
#define TRAIL_INIT_MAX 100
#define TRAIL_SIZE 5
#define TRAIL_UPDATE 100
#define TRAIL_COLOR BLACK

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
    TrailPart trail[TRAIL_CAP];
    int trail_index;
    int trail_max;
    int trail_update;
} Tron;

float degrees_to_radians(float degrees);

void tron_render_player(Tron *t);
void tron_update_player(Tron *t);
void tron_extend_trail(Tron *t);
void tron_render_trail(TrailPart *trail_p);
void tron_check_collision_trail(Tron *t, TrailPart *trail_p);
void tron_init(Tron *t);
void tron_update_game(Tron *t);

float degrees_to_radians(float degrees)
{
    return degrees * (PI / 180);
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

    if (IsKeyDown(KEY_LEFT)) p->angle -= PLAYER_ROTATION_SPEED;
    if (IsKeyDown(KEY_RIGHT)) p->angle += PLAYER_ROTATION_SPEED;
    if (p->angle > 360) p->angle = 0;
    if (p->angle < 0) p->angle = 360;

    p->pos.x += PLAYER_MOVEMENT_SPEED * cos(degrees_to_radians(p->angle));
    p->pos.y += PLAYER_MOVEMENT_SPEED * sin(degrees_to_radians(p->angle));

    if (p->pos.x > WINDOW_WIDTH) p->pos.x = 0;
    if (p->pos.y > WINDOW_HEIGHT) p->pos.y = 0;
    if (p->pos.x < 0) p->pos.x = WINDOW_WIDTH;
    if (p->pos.y < 0) p->pos.y = WINDOW_HEIGHT;
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
    DrawCircleV((Vector2) {trail_p->pos.x, trail_p->pos.y}, TRAIL_SIZE, TRAIL_COLOR);
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

    t->trail_max = TRAIL_INIT_MAX;
    for (int i = 0; i < TRAIL_CAP; i++) {
        t->trail[i] = (TrailPart){0};
    }
    t->trail_index = 0;
    t->trail_update = 0;
}

void tron_update_game(Tron *t)
{
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
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tron");
    SetTargetFPS(120);

    Tron t = {0};
    tron_init(&t);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BG_COLOR);
            tron_update_game(&t);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
