#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define INV_SIZE 10
#define ITEMS_COUNT 10
#define ITEM_LEN 33
#define RES_MAX 100
#define MAP_MIN 15
#define MAP_MAX 30
#define OBST_MIN 15
#define OBST_MAX 30
#define VIEW 4
#define INPUT_LEN 128
#define DIARY_FILE "diary.txt"
#define ITEMS_FILE "items.txt"
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

typedef struct GameTime {
    int day;
    int hour;
} GameTime;

int read_int(const char *prompt, int min_value, int max_value, int *value);
int read_line(const char *prompt, char *buffer, size_t size);
int confirm(const char *prompt);
int random_num(int min_value, int max_value);
void load_items(char item_names[ITEMS_COUNT][ITEM_LEN]);
void print_inventory(const int inventory[INV_SIZE],
                     const char item_names[ITEMS_COUNT][ITEM_LEN]);
int get_id(char item_names[ITEMS_COUNT][ITEM_LEN], const char *name);
void print_time(const GameTime *time);
void add_hours(GameTime *time, int hours);
void count_resources(void);

#endif
