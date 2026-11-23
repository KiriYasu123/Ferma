
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void fill_map(Map* map, int obstacle_percent) {
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (x == 0 || y == 0 ||
                x == map->width - 1 || y == map->height - 1) {
                map->cells[y][x] = '#';
            }
            else if (rand() % 100 < obstacle_percent) {
                map->cells[y][x] = (rand() % 2 == 0) ? 'T' : 'R';
            }
            else {
                map->cells[y][x] = '.';
            }
        }
    }
}

static void place_hero(Map* map, Player* player) {
    do {
        player->x = random_range(1, map->width - 2);
        player->y = random_range(1, map->height - 2);
    } while (map->cells[player->y][player->x] != '.');
}

static void place_stairs_up(Map* map, int x, int y, int* ux, int* uy) {
    *ux = x;
    *uy = y;

    if (map->cells[y - 1][x] == '.') --(*uy);
    else if (map->cells[y + 1][x] == '.') ++(*uy);
    else if (map->cells[y][x - 1] == '.') --(*ux);
    else if (map->cells[y][x + 1] == '.') ++(*ux);

    map->cells[*uy][*ux] = 'U';
}

static void place_stairs_down(Map* map, int x, int y, int ux, int uy) {
    int dx, dy;

    do {
        dx = random_range(1, map->width - 2);
        dy = random_range(1, map->height - 2);
    } while ((dx == x && dy == y) ||
        (dx == ux && dy == uy) ||
        map->cells[dy][dx] != '.');

    map->cells[dy][dx] = 'D';
}

Map* map_create(int width, int height, int floor) {
    Map* map = malloc(sizeof(Map));
    if (map == NULL) return NULL;

    map->width = width;
    map->height = height;
    map->floor = floor;

    map->cells = malloc(height * sizeof(char*));
    if (map->cells == NULL) {
        free(map);
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        map->cells[y] = malloc(width * sizeof(char));

        if (map->cells[y] == NULL) {
            for (int i = 0; i < y; i++) free(map->cells[i]);
            free(map->cells);
            free(map);
            return NULL;
        }
    }

    return map;
}

void map_destroy(Map** map) {
    if (map == NULL || *map == NULL) return;

    for (int y = 0; y < (*map)->height; y++) {
        free((*map)->cells[y]);
    }

    free((*map)->cells);

    printf("Память структуры по адресу %p освобождена\n", (void*)*map);

    free(*map);
    *map = NULL;
}

void map_print(const Map* map, int hero_x, int hero_y) {
    if (map == NULL) return;

    printf("\n");

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (x == hero_x && y == hero_y) {
                putchar('@');
            }
            else {
                int dx = x - hero_x;
                int dy = y - hero_y;

                /* Круглая область видимости */
                if (dx * dx + 4 * dy * dy <= VIEW_RADIUS * VIEW_RADIUS) {
                    putchar(map->cells[y][x]);
                }
                else {
                    putchar('?');
                }
            }
        }

        putchar('\n');
    }
}

static int is_walkable(char cell) {
    return cell == '.' || cell == 'U' || cell == 'D';
}

static void rescue_player(Player* player) {
    int lost = player->gold * RESCUE_GOLD_PERCENT / 100;
    player->gold -= lost;

    printf("\nГерой выбился из сил! Спасательная служба доставила его домой.\n");
    printf("За услуги удержано %d золота.\n", lost);
}

void play_adventure(Player* player, GameTime* time) {
    int adventure_active = 1;
    int rescued = 0;

    if (player == NULL) {
        printf("Сначала создайте персонажа.\n");
        return;
    }

    if (player->stamina <= 0) {
        printf("Недостаточно выносливости для похода.\n");
        return;
    }

    if (time->hour >= 2 && time->hour < 8) {
        printf("Ночью отправляться в приключение нельзя. Сначала отдохните.\n");
        return;
    }

    int floor = 1;

    while (adventure_active) {
        int size = random_range(MIN_MAP_SIZE, MAX_MAP_SIZE);
        int obstacle_percent = random_range(MIN_OBSTACLE_PERCENT, MAX_OBSTACLE_PERCENT);
        int ux, uy;

        Map* map = map_create(size, size, floor);

        if (map == NULL) {
            printf("Не удалось создать карту.\n");
            break;
        }

        fill_map(map, obstacle_percent);
        place_hero(map, player);
        place_stairs_up(map, player->x, player->y, &ux, &uy);
        place_stairs_down(map, player->x, player->y, ux, uy);

        int on_floor = 1;

        while (on_floor && adventure_active) {
            char command_line[16];
            char command;
            int new_x = player->x;
            int new_y = player->y;

            printf("\nHP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n",
                player->hp, player->max_hp, player->stamina,
                player->max_stamina, player->gold, floor, time->hour);

            map_print(map, player->x, player->y);

            printf("W — вверх, A — влево, S — вниз, D — вправо, Q — домой с U\n");

            if (!read_line("Введите команду -> ", command_line, sizeof(command_line))) {
                adventure_active = 0;
                break;
            }

            command = (char)toupper((unsigned char)command_line[0]);

            if (command == 'Q') {
                if (map->cells[player->y][player->x] == 'U') {
                    printf("Вы вернулись домой. Золото сохранено.\n");
                    adventure_active = 0;
                    on_floor = 0;
                }
                else {
                    printf("Домой можно уйти только с лестницы U!\n");
                }
                continue;
            }

            switch (command) {
            case 'W': new_y--; break;
            case 'S': new_y++; break;
            case 'A': new_x--; break;
            case 'D': new_x++; break;
            default:
                printf("Неизвестная команда.\n");
                continue;
            }

            if (new_x < 0 || new_x >= map->width ||
                new_y < 0 || new_y >= map->height ||
                !is_walkable(map->cells[new_y][new_x])) {
                printf("Туда не пройти!\n");
                continue;
            }

            player->x = new_x;
            player->y = new_y;
            player->stamina -= NORMAL_STEP_STAMINA_COST;

            if (player->stamina <= 0) {
                player->stamina = 0;
                rescue_player(player);
                rescued = 1;
                adventure_active = 0;
                on_floor = 0;
                break;
            }

            if (map->cells[player->y][player->x] == 'D') {
                player->stamina -= STAIR_STEP_STAMINA_COST;
                advance_hours(time, 1);

                floor++;
                player->gold += BASE_MAP_REWARD * floor;

                printf("Вы перешли на этаж %d и получили %d золота!\n",
                    floor, BASE_MAP_REWARD * floor);

                if (player->stamina <= 0 || time->hour == 2) {
                    if (player->stamina <= 0) player->stamina = 0;
                    rescue_player(player);
                    rescued = 1;
                    adventure_active = 0;
                }

                on_floor = 0;
            }
        }

        map_destroy(&map);
    }

    if (rescued) {
        printf("Приключение завершено экстренной эвакуацией.\n");
    }
}
