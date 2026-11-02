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
#define PERCENT_MAX 100
#define TREE_SHARE 50
#define VIEW_RADIUS 4
#define MIN_FREE_CELLS 3
#define DIR_COUNT 4
#define CELL_WALL '#'
#define CELL_FLOOR '.'
#define CELL_TREE 'T'
#define CELL_ROCK 'R'
#define CELL_HERO '@'
#define CELL_STAIRS_DOWN 'D'
#define CELL_STAIRS_UP 'U'
#define CELL_FOG '?'
//для передвижения
#define KEY_UP 'W'
#define KEY_LEFT 'A'
#define KEY_DOWN 'S'
#define KEY_RIGHT 'D'
#define KEY_HOME 'Q'
#define KEY_NONE '\0'
#define RESULT_HOME 0
#define RESULT_STAIRS 1

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

int random_range(int min, int max) { //генерируем рандомный диапазон для капты
	return min + rand() % (max - min + 1);
}

void clear_input() { //очищаем буфер
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

char read_key() { //считываем клавишу и приводим ее с верхнему регистру
	char key;
	if (scanf(" %c", &key) != 1) {
		return KEY_NONE;
	}
	clear_input();
	return (char)toupper((unsigned char)key);
}

void free_grid(char** grid, int height) { //освобождаем память массива
	if (grid == NULL) { 
		return;
	}
	for (int i = 0; i < height; i++) {	
		free(grid[i]);
	}
	free(grid);
}

char** create_grid(int height, int width, char fill) { // Создаём двумерный массив height на width в динамической памяти и заполняем его символом fill
	char** grid = (char**)calloc(height, sizeof(char*));
	if (grid == NULL) { // Если памяти не хватило, calloc вернёт NULL
		return NULL;
	}
	for (int i = 0; i < height; i++) { 
		grid[i] = (char*)malloc(width * sizeof(char));	// Выделяем память под одну строку из width символов
		if (grid[i] == NULL) {
			free_grid(grid, height);
			return NULL;
		}
		for (int j = 0; j < width; j++) { // Заполняем строку одинаковым символом
			grid[i][j] = fill;	// Кладём символ fill в клетку
		}
	}
	return grid;
}

void fill_map(char** map, int height, int width, int obstacle_percent) { //нполение карты - препятсвия и стены
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
				map[y][x] = CELL_WALL;
			}
			else if (rand() % PERCENT_MAX < obstacle_percent) {
				if (rand() % PERCENT_MAX < TREE_SHARE) {
					map[y][x] = CELL_TREE;
				}
				else {
					map[y][x] = CELL_ROCK;
				}
			}
			else {
				map[y][x] = CELL_FLOOR; //свободное место 
			}
		}
	}
}

int count_floor(char** map, int height, int width) { //подсчитываем кол-во свободных клеток,чтобы герой не застрял на спавне
	int count = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (map[y][x] == CELL_FLOOR) {
				count++;
			}
		}
	}
	return count;
}

void find_free_cell(char** map, int height, int width, int* x, int* y) {  //поиск случайной свободной клетки
	do {
		*x = random_range(1, width - 2);
		*y = random_range(1, height - 2);
	} while (map[*y][*x] != CELL_FLOOR);
}

void place_stairs_up(char** map, int hero_x, int hero_y) { //лестница U спвнится рядом с героем
	int dir_x[DIR_COUNT] = { 0, 0, -1, 1 }; //лево право
	int dir_y[DIR_COUNT] = { -1, 1, 0, 0 };//верх низ
	int start = rand() % DIR_COUNT; // Выбираем случайное направление, с которого начнём проверку
	for (int i = 0; i < DIR_COUNT; i++) {
		int k = (start + i) % DIR_COUNT;
		int nx = hero_x + dir_x[k];
		int ny = hero_y + dir_y[k];
		if (map[ny][nx] != CELL_WALL) {
			map[ny][nx] = CELL_STAIRS_UP;
			return;
		}
	}
}

void clear_cell(char** map, int x, int y) { //замена дерева или камня на лестницу
	if (map[y][x] == CELL_TREE || map[y][x] == CELL_ROCK) {
		map[y][x] = CELL_FLOOR;
	}
}

void clear_path(char** map, int from_x, int from_y, int to_x, int to_y) { //очищение пути до лестницы,чтобы герой всегда мог пройти
	int x = from_x;
	int y = from_y;
	while (x != to_x) {
		if (x < to_x) {
			x++;
		}
		else {
			x--;
		}
		clear_cell(map, x, y);
	}
	while (y != to_y) {
		if (y < to_y) {
			y++;
		}
		else {
			y--;
		}
		clear_cell(map, x, y);
	}
}

void build_level(char** map, int height, int width, int* hero_x, int* hero_y) { //строим этаж
	int obstacle_percent = random_range(MIN_OBSTACLE_PERCENT, MAX_OBSTACLE_PERCENT); // Случайный процент препятствий для этого этажа (15-30)
	int stairs_x;
	int stairs_y;
	do { // Генерируем карту заново, пока на ней не окажется достаточно свободных клеток
		fill_map(map, height, width, obstacle_percent);
	} while (count_floor(map, height, width) < MIN_FREE_CELLS); // Если свободных клеток слишком мало - повторяем
	find_free_cell(map, height, width, hero_x, hero_y); //находим свободную клетку для героя
	place_stairs_up(map, *hero_x, *hero_y); //ставим лестницу на выход
	do {
		find_free_cell(map, height, width, &stairs_x, &stairs_y); //лестнца вниз
	} while (stairs_x == *hero_x && stairs_y == *hero_y);
	map[stairs_y][stairs_x] = CELL_STAIRS_DOWN;
	clear_path(map, *hero_x, *hero_y, stairs_x, stairs_y);
}

int is_passable(char cell) { //проверка на препятствия
	return cell == CELL_FLOOR || cell == CELL_STAIRS_DOWN || cell == CELL_STAIRS_UP;
}

int is_visible(int x, int y, int hero_x, int hero_y) { //видимость героя 
	int dx = x - hero_x;
	int dy = y - hero_y;
	return dx * dx + dy * dy <= VIEW_RADIUS * VIEW_RADIUS; //находим расстояние по теореме пифагора
}

void print_map(char** map, int height, int width, int hero_x, int hero_y) { //вывод карты с учетом тумана войны
	printf("\n");
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x == hero_x && y == hero_y) {
				printf("%c", CELL_HERO);
			}
			else if (is_visible(x, y, hero_x, hero_y)) { //рисуем карту если в радиусе
				printf("%c", map[y][x]);
			}
			else {
				printf("%c", CELL_FOG); //заполняем туманом
			}
		}
		printf("\n");
	}
}

int get_direction(char key, int* dx, int* dy) { //для направлений
	*dx = 0;
	*dy = 0;
	if (key == KEY_UP) {
		*dy = -1;
		return 1;
	}
	if (key == KEY_DOWN) {
		*dy = 1;
		return 1;
	}
	if (key == KEY_LEFT) {
		*dx = -1;
		return 1;
	}
	if (key == KEY_RIGHT) {
		*dx = 1;
		return 1;
	}
	return 0;
}

int explore_level(char** map, int height, int width, int hero_x, int hero_y, int depth) { //смотрим дейтсвия игрока
	char key;
	int dx;
	int dy;
	int new_x;
	int new_y;
	while (1) {
		print_map(map, height, width, hero_x, hero_y);
		printf("Глубина: %d\n", depth);
		printf("W/A/S/D - идти, Q - сбежать домой (только на лестнице U)\n");
		printf("Введите команду -> ");
		key = read_key();
		if (key == KEY_NONE) {
			printf("Ввод закончился, возвращаемся домой.\n");
			return RESULT_HOME;
		}
		if (key == KEY_HOME) {
			if (map[hero_y][hero_x] == CELL_STAIRS_UP) {
				printf("Вы вернулись домой.\n");
				return RESULT_HOME; //возвращаем домой если игрок нажал Q 
			}
			printf("Домой можно уйти только с лестницы U!\n");
			continue;
		}
		if (!get_direction(key, &dx, &dy)) { //если не wasd
			printf("Неизвестная команда.\n");
			continue;
		}
		new_x = hero_x + dx;
		new_y = hero_y + dy;
		if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height || !is_passable(map[new_y][new_x])) {
			printf("Туда не пройти!\n"); //если препятсвие на пути
			continue;
		}
		hero_x = new_x;
		hero_y = new_y;
		if (map[hero_y][hero_x] == CELL_STAIRS_DOWN) {
			printf("Вы спустились на следующий этаж!\n");
			return RESULT_STAIRS;
		}
	}
}

void play_adventure() { //основная функция приключений
	char** map;
	int height;
	int width;
	int hero_x;
	int hero_y;
	int depth = 1;
	int result;
	int in_dungeon = 1;
	while (in_dungeon) {
		height = random_range(MIN_MAP_SIZE, MAX_MAP_SIZE);
		width = random_range(MIN_MAP_SIZE, MAX_MAP_SIZE);
		map = create_grid(height, width, CELL_WALL);
		if (map == NULL) {
			printf("Не хватило памяти для карты.\n");
			return;
		}
		build_level(map, height, width, &hero_x, &hero_y);
		result = explore_level(map, height, width, hero_x, hero_y, depth);
		free_grid(map, height);
		if (result == RESULT_HOME) {
			in_dungeon = 0;
		}
		else {
			depth++;
		}
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
