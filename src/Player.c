#define _CRT_SECURE_NO_WARNINGS
#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *house_name(HouseLevel level) {
    switch (level) {
        case TENT: return "Палатка";
        case CABIN: return "Хижина";
        case HOUSE: return "Дом";
        case MANSION: return "Особняк";
        default: return "Неизвестно";
    }
}

static void set_cost(Farmhouse *house) {
    if (house->level >= MANSION) {
        house->upgrade_cost = 0;
        return;
    }
    house->upgrade_cost = HOUSE_PRICE * ((int)house->level + 1);
}

Player *new_player(void) {
    Player *player = (Player *)malloc(sizeof(Player));
    char name[NAME_SIZE];

    if (player == NULL) {
        printf("Не удалось выделить память под персонажа.\n");
        return NULL;
    }

    if (!read_line("Введите имя фермера: ", name, sizeof(name)) || name[0] == '\0') {
        printf("Имя не может быть пустым. Создание отменено.\n");
        free(player);
        return NULL;
    }
    snprintf(player->name, sizeof(player->name), "%s", name);

    player->max_hp = START_HP;
    player->hp = START_HP;
    player->max_stamina = START_STAMINA;
    player->stamina = START_STAMINA;
    player->gold = 100;
    player->x = 1;
    player->y = 1;

    for (int i = 0; i < INV_SIZE; ++i) {
        player->inventory[i] = 0;
    }
    player->inventory[0] = 1;
    player->inventory[2] = 2;
    player->inventory[5] = 3;
    player->inventory[7] = 4;
    player->inventory[9] = 5;

    player->farmhouse.level = TENT;
    player->farmhouse.comfort = 1;
    set_cost(&player->farmhouse);

    printf("Персонаж \"%s\" создан.\n", player->name);
    return player;
}

void free_player(Player **player) {
    if (player != NULL && *player != NULL) {
        printf("Память структуры по адресу %p освобождена\n", (void *)*player);
        free(*player);
        *player = NULL;
    }
}

void show_card(const Player *player,
                       const char item_names[ITEMS_COUNT][ITEM_LEN],
                       const GameTime *time) {
    if (player == NULL) return;

    printf("\n========== КАРТОЧКА ПЕРСОНАЖА ==========\n");
    printf("Имя: %s\n", player->name);
    printf("Здоровье: %d/%d\n", player->hp, player->max_hp);
    printf("Выносливость: %d/%d\n", player->stamina, player->max_stamina);
    printf("Золото: %d\n", player->gold);
    printf("Дом: %s (комфорт: %d)\n", house_name(player->farmhouse.level),
           player->farmhouse.comfort);
    printf("Время: ");
    print_time(time);
    printf("Инвентарь:\n");
    print_inventory(player->inventory, item_names);
    printf("========================================\n");
}

void rest(Player *player) {
    int stamina_target;
    if (player == NULL) return;

    player->hp = player->max_hp;
    switch (player->farmhouse.level) {
        case TENT:
            stamina_target = player->max_stamina * 70 / 100;
            break;
        case CABIN:
            stamina_target = player->max_stamina;
            break;
        case HOUSE:
            stamina_target = player->max_stamina + 15;
            break;
        case MANSION:
            stamina_target = player->max_stamina + 30;
            break;
        default:
            stamina_target = player->max_stamina;
            break;
    }
    player->stamina = stamina_target;
    printf("Вы хорошо отдохнули. Здоровье восстановлено, выносливость: %d.\n",
           player->stamina);
}

int upgrade_house(Player *player) {
    if (player == NULL) return 0;
    if (player->farmhouse.level >= MANSION) {
        printf("У вас уже особняк. Улучшать больше нечего.\n");
        return 0;
    }
    if (player->gold < player->farmhouse.upgrade_cost) {
        printf("Недостаточно золота. Нужно: %d, у вас: %d.\n",
               player->farmhouse.upgrade_cost, player->gold);
        return 0;
    }

    player->gold -= player->farmhouse.upgrade_cost;
    player->farmhouse.level = (HouseLevel)(player->farmhouse.level + 1);
    player->farmhouse.comfort = (int)player->farmhouse.level + 1;
    set_cost(&player->farmhouse);
    printf("Жильё улучшено до уровня: %s.\n", house_name(player->farmhouse.level));
    if (player->farmhouse.upgrade_cost > 0) {
        printf("Следующее улучшение будет стоить %d золота.\n",
               player->farmhouse.upgrade_cost);
    }
    return 1;
}

void show_inventory(const Player *player,
                            const char item_names[ITEMS_COUNT][ITEM_LEN]) {
    if (player != NULL) print_inventory(player->inventory, item_names);
}

void put_item(Player *player,
                     const char item_names[ITEMS_COUNT][ITEM_LEN]) {
    int slot, item_id;
    if (player == NULL) return;
    if (!read_int("Введите индекс слота (0-9): ", 0, INV_SIZE - 1, &slot)) return;
    if (!read_int("Введите ID предмета (0-9): ", 0, ITEMS_COUNT - 1, &item_id)) return;
    player->inventory[slot] = item_id;
    printf("В слот %d помещён предмет: [%d] - %s\n", slot, item_id,
           item_names[item_id]);
}

void drop_item(Player *player,
                      const char item_names[ITEMS_COUNT][ITEM_LEN]) {
    int slot;
    if (player == NULL) return;
    if (!read_int("Введите индекс слота для выброса (0-9): ",
                  0, INV_SIZE - 1, &slot)) return;
    if (player->inventory[slot] == 0) {
        printf("Слот %d уже пуст.\n", slot);
        return;
    }
    printf("Выброшено: [%d] - %s\n", player->inventory[slot],
           item_names[player->inventory[slot]]);
    player->inventory[slot] = 0;
}

void pack_inventory(Player *player,
                               const char item_names[ITEMS_COUNT][ITEM_LEN]) {
    int next = 0;
    if (player == NULL) return;
    printf("До сжатия:\n");
    show_inventory(player, item_names);
    for (int i = 0; i < INV_SIZE; ++i) {
        if (player->inventory[i] != 0) {
            player->inventory[next++] = player->inventory[i];
        }
    }
    for (int i = next; i < INV_SIZE; ++i) {
        player->inventory[i] = 0;
    }
    printf("После сжатия:\n");
    show_inventory(player, item_names);
}

void search_item(const Player *player,
                        char item_names[ITEMS_COUNT][ITEM_LEN]) {
    char search_name[ITEM_LEN];
    int item_id, found = 0;
    if (player == NULL) return;
    if (!read_line("Введите название предмета: ", search_name, sizeof(search_name))) return;
    item_id = get_id(item_names, search_name);
    if (item_id == -1) {
        printf("Предмета \"%s\" нет в каталоге.\n", search_name);
        return;
    }
    for (int i = 0; i < INV_SIZE; ++i) {
        if (player->inventory[i] == item_id) {
            if (!found) printf("Предмет \"%s\" лежит в слотах:", search_name);
            printf(" %d", i);
            found = 1;
        }
    }
    if (found) printf("\n");
    else printf("Предмета \"%s\" нет в рюкзаке.\n", search_name);
}
