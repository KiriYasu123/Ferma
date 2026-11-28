#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void default_names(char names[ITEMS_COUNT][ITEM_LEN]) {
    const char *defaults[ITEMS_COUNT] = {
        "Пусто", "Дерево", "Камень", "Семена", "Солома",
        "Цветок", "Железо", "Шерсть", "Мясо", "Золото"
    };
    for (int i = 0; i < ITEMS_COUNT; ++i) {
        snprintf(names[i], ITEM_LEN, "%s", defaults[i]);
    }
}

int read_line(const char *prompt, char *buffer, size_t size) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
    return 1;
}

int read_int(const char *prompt, int min_value, int max_value, int *value) {
    char line[INPUT_LEN];
    char *end = NULL;
    long parsed;

    for (;;) {
        if (!read_line(prompt, line, sizeof(line))) {
            return 0;
        }
        parsed = strtol(line, &end, 10);
        if (end == line || *end != '\0' || parsed < min_value || parsed > max_value) {
            printf("Ошибка ввода. Введите целое число от %d до %d.\n",
                   min_value, max_value);
            continue;
        }
        *value = (int)parsed;
        return 1;
    }
}

int confirm(const char *prompt) {
    char answer[INPUT_LEN];
    for (;;) {
        if (!read_line(prompt, answer, sizeof(answer))) {
            return 0;
        }
        if (answer[0] == 'y' || answer[0] == 'Y' ||
            answer[0] == '1') {
            return 1;
        }
        if (answer[0] == 'n' || answer[0] == 'N' ||
            answer[0] == '0') {
            return 0;
        }

        if (strcmp(answer, "да") == 0 || strcmp(answer, "Да") == 0 ||
            strcmp(answer, "д") == 0 || strcmp(answer, "Д") == 0) {
            return 1;
        }
        if (strcmp(answer, "нет") == 0 || strcmp(answer, "Нет") == 0 ||
            strcmp(answer, "н") == 0 || strcmp(answer, "Н") == 0) {
            return 0;
        }
        printf("Ответьте да или нет.\n");
    }
}

int random_num(int min_value, int max_value) {
    return min_value + rand() % (max_value - min_value + 1);
}

void load_items(char item_names[ITEMS_COUNT][ITEM_LEN]) {
    char name[ITEM_LEN];
    FILE *file;

    default_names(item_names);
    file = fopen(ITEMS_FILE, "r");
    if (file == NULL) {
        printf("Предупреждение: %s не найден, используются названия по умолчанию.\n",
               ITEMS_FILE);
        return;
    }

    for (int i = 0; i < ITEMS_COUNT; ++i) {
        if (fscanf(file, "%32s", name) != 1) {
            break;
        }
        if (name[0] >= '0' && name[0] <= '9') {
            if (fscanf(file, "%32s", name) != 1) {
                break;
            }
        }
        snprintf(item_names[i], ITEM_LEN, "%s", name);
    }
    fclose(file);
}

void print_inventory(const int inventory[INV_SIZE],
                     const char item_names[ITEMS_COUNT][ITEM_LEN]) {
    for (int i = 0; i < INV_SIZE; ++i) {
        int id = inventory[i];
        if (id < 0 || id >= ITEMS_COUNT) {
            id = 0;
        }
        printf("Слот %d: [%d] - %s\n", i, id, item_names[id]);
    }
}

int get_id(char item_names[ITEMS_COUNT][ITEM_LEN], const char *name) {
    for (int i = 0; i < ITEMS_COUNT; ++i) {
        if (strcmp(item_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

void print_time(const GameTime *time) {
    printf("День %d, %02d:00\n", time->day, time->hour);
}

void add_hours(GameTime *time, int hours) {
    time->hour += hours;
    while (time->hour >= 24) {
        time->hour -= 24;
        ++time->day;
    }
}

void count_resources(void) {
    char names[RES_MAX][ITEM_LEN];
    int counts[RES_MAX];
    char resource_name[ITEM_LEN];
    int count;
    int n = 0;
    int pos = -1;
    FILE *in = fopen(INPUT_FILE, "r");

    if (in == NULL) {
        printf("Файл %s не найден.\n", INPUT_FILE);
        return;
    }

    while (fscanf(in, "%32s %d", resource_name, &count) == 2) {
        int ch;
        while ((ch = fgetc(in)) == ',' || ch == ' ' || ch == '\t') {
            /* пропускаем разделители из старого формата файла */
        }
        if (ch != EOF) {
            ungetc(ch, in);
        }

        pos = -1;
        for (int i = 0; i < n; ++i) {
            if (strcmp(names[i], resource_name) == 0) {
                pos = i;
                break;
            }
        }
        if (pos >= 0) {
            counts[pos] += count;
        } else if (n < RES_MAX) {
            snprintf(names[n], ITEM_LEN, "%s", resource_name);
            counts[n] = count;
            ++n;
        } else {
            printf("Слишком много разных ресурсов, %s пропущен.\n", resource_name);
        }
    }
    fclose(in);

    FILE *out = fopen(OUTPUT_FILE, "w");
    if (out == NULL) {
        printf("Не удалось открыть %s для записи.\n", OUTPUT_FILE);
        return;
    }
    for (int i = 0; i < n; ++i) {
        printf("%s: %d шт.\n", names[i], counts[i]);
        fprintf(out, "%s: %d шт.\n", names[i], counts[i]);
    }
    fclose(out);
    printf("Результат записан в %s.\n", OUTPUT_FILE);
}
