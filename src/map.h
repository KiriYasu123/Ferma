#ifndef MAP_H
#define MAP_H

#include "player.h"

#define BASE_MAP_REWARD 10
#define RESCUE_GOLD_PERCENT 50
#define STAIR_STEP_STAMINA_COST 5
#define NORMAL_STEP_STAMINA_COST 1


typedef struct Map {
    char **cells;
    int width;
    int height;
    int floor;
} Map;

Map *map_create(int width, int height, int floor);
void map_destroy(Map **map);
void map_print(const Map *map, int hero_x, int hero_y);
void play_adventure(Player *player, GameTime *time);

#endif
