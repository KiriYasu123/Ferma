#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

#define NAME_SIZE 64
#define START_HP 100
#define START_STAMINA 50
#define HOUSE_PRICE 50

typedef enum HouseLevel {
    TENT = 0,
    CABIN,
    HOUSE,
    MANSION
} HouseLevel;

typedef struct Farmhouse {
    HouseLevel level;
    int upgrade_cost;
    int comfort;
} Farmhouse;

typedef struct Player {
    char name[NAME_SIZE];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x;
    int y;
    int inventory[INV_SIZE];
    Farmhouse farmhouse;
} Player;

Player *new_player(void);
void free_player(Player **player);
void show_card(const Player *player,
                       const char item_names[ITEMS_COUNT][ITEM_LEN],
                       const GameTime *time);
void rest(Player *player);
int upgrade_house(Player *player);
void show_inventory(const Player *player,
                            const char item_names[ITEMS_COUNT][ITEM_LEN]);
void put_item(Player *player,
                     const char item_names[ITEMS_COUNT][ITEM_LEN]);
void drop_item(Player *player,
                      const char item_names[ITEMS_COUNT][ITEM_LEN]);
void pack_inventory(Player *player,
                               const char item_names[ITEMS_COUNT][ITEM_LEN]);
void search_item(const Player *player,
                        char item_names[ITEMS_COUNT][ITEM_LEN]);

#endif
