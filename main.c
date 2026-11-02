#define _CRT_SECURE_NO_WARNINGS // от ошибок
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 33
#define FARMER_NAME_LEN 33
#define MAX_RESOURCES 100
// для карты
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define MIN_OBSTACLE_PERCENT 15
#define MAX_OBSTACLE_PERCENT 30
#define VIEW_RADIUS 4

int check(int scanf_result) { // проверка на дурака
	int c;
	if (scanf_result != 1) {
		while ((c = getchar()) != '\n' && c != EOF);
		printf("Не целое число.\n");
		return 0;
	}
	return 1;
}

void set_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN]) { // заполняем каталог названиями
	strcpy(item_names[0], "Пусто");
	strcpy(item_names[1], "Дерево");
	strcpy(item_names[2], "Камень");
	strcpy(item_names[3], "Семена");
	strcpy(item_names[4], "Солома");
	strcpy(item_names[5], "Цветок");
	strcpy(item_names[6], "Железо");
	strcpy(item_names[7], "Шерсть");
	strcpy(item_names[8], "Мясо");
	strcpy(item_names[9], "Золото");
}

void print_inventory(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) { // вывод инвентаря
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		printf("Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);
	}
}

int find_item_id(char item_names[MAX_ITEMS][MAX_NAME_LEN], char name[]) { // ищем предмет по айди
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (strcmp(item_names[i], name) == 0) { // strcmp возвращает 0, если строки полностью совпадают
			return i;
		}
	}
	return -1;
}

void load_items(char item_names[MAX_ITEMS][MAX_NAME_LEN]) { // Читаем названия предметов из items.txt в массив item_names.
	char name[MAX_NAME_LEN];
	int i;
	set_item_names(item_names);
	FILE* file = fopen("items.txt", "r");
	if (file == NULL) {
		printf("Предупреждение: items.txt не найден, названия по умолчанию.\n");
		return;
	}
	for (i = 0; i < MAX_ITEMS; i++) {
		if (fscanf(file, "%31s", name) != 1) {
			break;
		}
		if (name[0] >= '0' && name[0] <= '9') { // Если первый символ слова цифра, то это ID
			if (fscanf(file, "%31s", name) != 1) {
				break;
			}
		}
		strcpy(item_names[i], name);
	}
	if (i < MAX_ITEMS) {
		printf("Предупреждение: в items.txt только %d названий, остальные по умолчанию.\n", i);
	}
	fclose(file);
}

int random_range(int min, int max) { // случайное число от min до max
	return min + rand() % (max - min + 1);
}

void print_map(char** map, int height, int width, int hero_x, int hero_y) { // вывод карты с туманом войны
	printf("\n");
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x == hero_x && y == hero_y) {
				printf("@");
			}
			else if (abs(x - hero_x) <= VIEW_RADIUS && abs(y - hero_y) <= VIEW_RADIUS) {
				printf("%c", map[y][x]); // клетка рядом с героем - показываем
			}
			else {
				printf("?"); // далеко от героя - неизвестность
			}
		}
		printf("\n");
	}
}

void play_adventure() {
	int depth = 1;
	int in_dungeon = 1; // 1 - пока герой в подземелье
	int on_floor;       // 1 - пока герой на текущем этаже
	int height, width, percent, hero_x, hero_y, ux, uy, dx, dy, new_x, new_y;
	char key;
	char** map;

	while (in_dungeon) {
		// каждый этаж - новая карта случайного размера в динамической памяти
		height = random_range(MIN_MAP_SIZE, MAX_MAP_SIZE);
		width = random_range(MIN_MAP_SIZE, MAX_MAP_SIZE);
		percent = random_range(MIN_OBSTACLE_PERCENT, MAX_OBSTACLE_PERCENT);
		map = (char**)malloc(height * sizeof(char*));
		for (int y = 0; y < height; y++) {
			map[y] = (char*)malloc(width * sizeof(char));
		}

		// заполняем карту
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
					map[y][x] = '#'; // внешние границы - стены
				}
				else if (rand() % 100 < percent) {
					if (rand() % 2 == 0) {
						map[y][x] = 'T'; // дерево
					}
					else {
						map[y][x] = 'R'; // камень
					}
				}
				else {
					map[y][x] = '.'; // свободная клетка
				}
			}
		}

		// герой появляется на свободной клетке
		do {
			hero_x = random_range(1, width - 2);
			hero_y = random_range(1, height - 2);
		} while (map[hero_y][hero_x] != '.');

		// лестница вверх U рядом с героем (первая соседняя клетка, которая не стена)
		ux = hero_x;
		uy = hero_y;
		if (map[uy - 1][ux] != '#') {
			uy--;
		}
		else if (map[uy + 1][ux] != '#') {
			uy++;
		}
		else if (map[uy][ux - 1] != '#') {
			ux--;
		}
		else {
			ux++;
		}
		map[uy][ux] = 'U';

		// лестница вниз D в случайном месте (не на герое и не на U)
		do {
			dx = random_range(1, width - 2);
			dy = random_range(1, height - 2);
		} while ((dx == hero_x && dy == hero_y) || (dx == ux && dy == uy));
		map[dy][dx] = 'D';

		on_floor = 1;
		while (on_floor) {
			print_map(map, height, width, hero_x, hero_y);
			printf("Глубина: %d\n", depth);
			printf("W - вверх, A - влево, S - вниз, D - вправо, Q - сбежать домой (только с лестницы U)\n");
			printf("Введите команду -> ");
			if (scanf(" %c", &key) != 1) { // если ввод закончился
				on_floor = 0;
				in_dungeon = 0;
				break;
			}
			key = toupper(key);

			if (key == 'Q') {
				if (map[hero_y][hero_x] == 'U') {
					printf("Вы вернулись домой.\n");
					on_floor = 0;
					in_dungeon = 0;
				}
				else {
					printf("Домой можно уйти только с лестницы U!\n");
				}
				continue;
			}

			// считаем, куда герой хочет шагнуть
			new_x = hero_x;
			new_y = hero_y;
			if (key == 'W') {
				new_y--;
			}
			else if (key == 'S') {
				new_y++;
			}
			else if (key == 'A') {
				new_x--;
			}
			else if (key == 'D') {
				new_x++;
			}
			else {
				printf("Неизвестная команда.\n");
				continue;
			}

			// ходить можно только по свободной клетке или лестнице
			if (map[new_y][new_x] == '.' || map[new_y][new_x] == 'U' || map[new_y][new_x] == 'D') {
				hero_x = new_x;
				hero_y = new_y;
			}
			else {
				printf("Туда не пройти!\n");
			}

			// герой встал на D - уходим на следующий этаж
			if (map[hero_y][hero_x] == 'D') {
				printf("Вы спустились на следующий этаж!\n");
				depth++;
				on_floor = 0;
			}
		}

		// удаляем старую карту
		for (int y = 0; y < height; y++) {
			free(map[y]);
		}
		free(map);
	}
}

int main() {
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	srand((unsigned int)time(NULL));
	int current_day = 1;
	int current_hour = 8;
	int choice;
	int work_hours;
	int max_work_hours = 10000;
	int inventory[INVENTORY_SIZE] = { 1, 0, 2, 0, 0, 3, 0, 4, 0, 5 };
	int slot;
	int item_id;
	char farmer_name[FARMER_NAME_LEN];
	char item_names[MAX_ITEMS][MAX_NAME_LEN];
	char search_name[MAX_NAME_LEN];

	do {
		printf("Введите имя фермера: ");
	} while (!check(scanf("%32s", farmer_name)));
	load_items(item_names);

	while (1)
	{
		printf("\nПривет, %s!\n"
			"МЕНЮ\n"
			" [0] - Выход\n"
			" [1] - Посмотреть на часы\n"
			" [2] - Промотать время (Поработать)\n"
			" [3] - Посмотреть инвентарь\n"
			" [4] - Положить предмет в слот\n"
			" [5] - Выбросить предмет\n"
			" [6] - Сжатие инвентаря\n"
			" [7] - Поиск предмета в рюкзаке\n"
			" [8] - Записать в дневник\n"
			" [9] - Расшифровать старые записи\n"
			" [10] - Отправиться в приключение\n"
			"Введите номер -> ", farmer_name);

		if (!check(scanf("%d", &choice)))
		{
			continue; // если вместо числа ввели что-то другое — показываем меню снова
		}

		switch (choice) {
		case 0:
			printf("До свидания, %s!\n", farmer_name);
			return 0; // закрытие
		case 1:
			printf("Текущее время: День %d, %d:00\n", current_day, current_hour);
			break;
		case 2:
			printf("Сколько часов работать?: ");
			if (!check(scanf("%d", &work_hours)))
			{
				break;
			}
			if (work_hours < 0 || work_hours > max_work_hours)
			{
				printf("Допустимо от 0 до %d часов.\n", max_work_hours);
				break;
			}
			current_hour += work_hours;
			current_day += current_hour / 24;
			current_hour %= 24;
			printf("Сейчас: День %d, %02d:00\n", current_day, current_hour);
			break;
		case 3:
			print_inventory(inventory, item_names);
			break;
		case 4:
			printf("Введите индекс слота (0-%d): ", INVENTORY_SIZE - 1);
			if (!check(scanf("%d", &slot)))
			{
				break;
			}
			if (slot < 0 || slot >= INVENTORY_SIZE)
			{
				printf("Такого слота нет! Допустимо 0-%d.\n", INVENTORY_SIZE - 1);
				break;
			}
			printf("Введите ID предмета (0-%d): ", MAX_ITEMS - 1);
			if (!check(scanf("%d", &item_id)))
			{
				break;
			}
			if (item_id < 0 || item_id >= MAX_ITEMS)
			{
				printf("Такого ID нет! Допустимо 0-%d.\n", MAX_ITEMS - 1);
				break;
			}
			inventory[slot] = item_id;
			printf("В слот %d помещен предмет: [%d] - %s\n", slot, item_id, item_names[item_id]);
			break;
		case 5:
			printf("Введите индекс слота для выброса (0-%d): ", INVENTORY_SIZE - 1);
			if (!check(scanf("%d", &slot)))
			{
				break;
			}
			if (slot < 0 || slot >= INVENTORY_SIZE)
			{
				printf("Такого слота нет! Допустимо 0-%d.\n", INVENTORY_SIZE - 1);
				break;
			}
			if (inventory[slot] == 0)
			{
				printf("Слот %d уже пуст.\n", slot);
				break;
			}
			printf("Выброшено: [%d] - %s\n", inventory[slot], item_names[inventory[slot]]);
			inventory[slot] = 0;
			break;
		case 6: {
			printf("До сжатия:\n");
			print_inventory(inventory, item_names);
			int next = 0;
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				if (inventory[i] != 0)
				{
					inventory[next] = inventory[i];
					next++;
				}
			}
			for (int i = next; i < INVENTORY_SIZE; i++)
			{
				inventory[i] = 0;
			}
			printf("После сжатия:\n");
			print_inventory(inventory, item_names);
			break;
		}
		case 7: {
			printf("Введите название предмета: ");
			if (!check(scanf("%31s", search_name)))
			{
				break;
			}
			item_id = find_item_id(item_names, search_name);
			if (item_id == -1)
			{
				printf("Предмета \"%s\" нет в каталоге.\n", search_name);
				break;
			}
			int found = 0;
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				if (inventory[i] == item_id)
				{
					if (!found) // заголовок печатаем один раз, при первом совпадении
					{
						printf("Предмет \"%s\" лежит в слотах:", search_name);
					}
					printf(" %d", i);
					found = 1;
				}
			}
			if (found)
			{
				printf("\n");
			}
			else
			{
				printf("Предмета \"%s\" нет в рюкзаке.\n", search_name);
			}
			break;
		}
		case 8: {
			FILE* diary = fopen("diary.txt", "a"); // "a" дописываем в конец, старые записи не трогаем
			if (diary == NULL)
			{
				printf("Не удалось открыть diary.txt.\n");
				break;
			}
			fprintf(diary, "Фермер: %s\n", farmer_name);
			fprintf(diary, "День %d, %02d:00\n", current_day, current_hour);
			fprintf(diary, "Инвентарь:\n");
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				fprintf(diary, "Слот %d: %s\n", i, item_names[inventory[i]]);
			}
			fprintf(diary, "\n");
			fclose(diary);
			printf("Запись добавлена в diary.txt.\n");
			break;
		}
		case 9:
		{
			char names[MAX_RESOURCES][MAX_NAME_LEN];
			int counts[MAX_RESOURCES];
			char res_name[MAX_NAME_LEN]; // название очередного ресурса из файла
			int count;// количество очередного ресурса из файла
			int n = 0; // сколько разных ресурсов уже накопили
			int pos; // позиция найденного ресурса в names
			FILE* in = fopen("input.txt", "r"); // открываем на чтение
			if (in == NULL)
			{
				printf("Файл input.txt не найден.\n");
				break;
			}
			while (fscanf(in, "%31s %d", res_name, &count) == 2) // Читаем пару слово - число fscanf возвращает 2, если прочитаны оба значения
			{
				fscanf(in, " ,");
				pos = -1;
				for (int i = 0; i < n; i++)
				{
					if (strcmp(names[i], res_name) == 0)
					{
						pos = i;// запоминаем позицию
						break;
					}
				}
				if (pos >= 0) // ресурс уже был - прибавляем количество
				{
					counts[pos] += count;
				}
				else if (n < MAX_RESOURCES) // новый ресурс и место ещё есть
				{
					strcpy(names[n], res_name);
					counts[n] = count;
					n++;
				}
				else
				{
					printf("Слишком много разных ресурсов, %s пропущен.\n", res_name);
				}
			}
			fclose(in);
			FILE* out = fopen("output.txt", "w");
			if (out == NULL)
			{
				printf("Не удалось открыть output.txt.\n");
				break;
			}
			for (int i = 0; i < n; i++)
			{
				printf("%s: %d шт.\n", names[i], counts[i]);
				fprintf(out, "%s: %d шт.\n", names[i], counts[i]);
			}
			fclose(out);
			printf("Результат записан в output.txt.\n");
			break;
		}
		case 10:
			play_adventure();
			break;
		default:
			printf("Нет такого пункта, выберите 0-10.\n");
			break;
		}
	}
	return 0;
}
