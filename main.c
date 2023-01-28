#include <stdio.h>
#include <raylib.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 25
#define PLAYER_MOVEMENT_SPEED 2
#define PLAYER_ROTATION_SPEED 3
#define PLAYER_HEAD_SIZE 10
#define PLAYER_HEAD_COLOR PURPLE

#define TRAIL_CAP 10000
#define TRAIL_INIT_MAX 1000
#define TRAIL_SIZE 10

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
    return degrees*(PI/180);
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

    DrawRectanglePro(p_rect, (Vector2) {p_rect.width/2, p_rect.height/2}, p->angle, RED);
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
}

void tron_extend_trail(Tron *t)
{
    Player *p = &t->player;

    if (t->trail_index >= t->trail_max) {
        t->trail_index = 0;
    }

    float trail_x = p->pos.x + -PLAYER_WIDTH/2 * cos(degrees_to_radians(p->angle));
    float trail_y = p->pos.y + -PLAYER_WIDTH/2 * sin(degrees_to_radians(p->angle));

    t->trail[t->trail_index] = (TrailPart) {{trail_x, trail_y}, p->angle};
    t->trail_index += 1;
}

void tron_render_trail(TrailPart *trail_p)
{
    Rectangle trail_rect = {
        trail_p->pos.x,
        trail_p->pos.y,
        TRAIL_SIZE,
        TRAIL_SIZE,
    };

    DrawRectanglePro(trail_rect, (Vector2) {trail_rect.width/2, trail_rect.height/2}, trail_p->angle, BLUE);
}

void tron_check_collision_trail(Tron *t, TrailPart *trail_p)
{
    Rectangle trail_rect = {
        trail_p->pos.x,
        trail_p->pos.y,
        TRAIL_SIZE,
        TRAIL_SIZE,
    };

    // TODO: find a cleaner way to write this (maybe put moving point in angle in seperate functions)
    float trail_x1 = trail_rect.x + TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle));
    float trail_y1 = trail_rect.y + TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle));
    trail_x1 += TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle-90));
    trail_y1 += TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle-90));

    float trail_x2 = trail_rect.x + TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle));
    float trail_y2 = trail_rect.y + TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle));
    trail_x2 += TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle+90));
    trail_y2 += TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle+90));

    float trail_x3 = trail_rect.x - TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle));
    float trail_y3 = trail_rect.y - TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle));
    trail_x3 += TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle-90));
    trail_y3 += TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle-90));

    float trail_x4 = trail_rect.x - TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle));
    float trail_y4 = trail_rect.y - TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle));
    trail_x4 += TRAIL_SIZE/2 * cos(degrees_to_radians(trail_p->angle+90));
    trail_y4 += TRAIL_SIZE/2 * sin(degrees_to_radians(trail_p->angle+90));

    Vector2 trail_points[4] = { {trail_x1, trail_y1}, {trail_x2, trail_y2}, {trail_x3, trail_y3}, {trail_x4, trail_y4} };

    Player *p = &t->player;
    float player_head_x1 = p->pos.x + PLAYER_WIDTH/2 * cos(degrees_to_radians(p->angle));
    float player_head_y1 = p->pos.y + PLAYER_WIDTH/2 * sin(degrees_to_radians(p->angle));
    player_head_x1 += PLAYER_HEIGHT/2 * cos(degrees_to_radians(p->angle-90));
    player_head_y1 += PLAYER_HEIGHT/2 * sin(degrees_to_radians(p->angle-90));

    float player_head_x2 = p->pos.x + PLAYER_WIDTH/2 * cos(degrees_to_radians(p->angle));
    float player_head_y2 = p->pos.y + PLAYER_WIDTH/2 * sin(degrees_to_radians(p->angle));
    player_head_x2 += PLAYER_HEIGHT/2 * cos(degrees_to_radians(p->angle+90));
    player_head_y2 += PLAYER_HEIGHT/2 * sin(degrees_to_radians(p->angle+90));

    DrawLineEx((Vector2) {player_head_x1, player_head_y1}, (Vector2) {player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE, PLAYER_HEAD_COLOR);

    if (CheckCollisionPointLine(trail_points[0], (Vector2) {player_head_x1, player_head_y1}, (Vector2) {player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE) || CheckCollisionPointLine(trail_points[1], (Vector2) {player_head_x1, player_head_y1}, (Vector2) {player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE) || CheckCollisionPointLine(trail_points[2], (Vector2) {player_head_x1, player_head_y1}, (Vector2) {player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE) || CheckCollisionPointLine(trail_points[3], (Vector2) {player_head_x1, player_head_y1}, (Vector2) {player_head_x2, player_head_y2}, PLAYER_HEAD_SIZE)) tron_init(t);
}

void tron_init(Tron *t)
{
    t->player = (Player) {
        (Vector2) {WINDOW_WIDTH/2, WINDOW_HEIGHT/2},
        (Vector2) {0.0, 0.0},
        0,
    };

    t->trail_max = TRAIL_INIT_MAX;
    for (int i = 0; i < TRAIL_CAP; i++) {
        t->trail[i] = (TrailPart) {0};
    }
    t->trail_index = 0;
}

void tron_update_game(Tron *t)
{
    // Drawing
    tron_render_player(t);
    tron_extend_trail(t);
    for (int i = 0; i < TRAIL_CAP; i++) {
        TrailPart *trail_p = &t->trail[i];
        tron_render_trail(trail_p);
        tron_check_collision_trail(t, trail_p);
    }

    // Updating
    tron_update_player(t);
}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tron");
    SetTargetFPS(60);

    Tron t = {0};
    tron_init(&t);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(WHITE);
            tron_update_game(&t);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
