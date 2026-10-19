#define _CRT_SECURE_NO_WARNINGS // от ошибок
#include <stdio.h>
#include <string.h>
#include <windows.h> 

#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 33
#define FARMER_NAME_LEN 33
#define MAX_RESOURCES 100

int check (int scanf_result) { //проверка на дурака
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

void print_inventory(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {  // вывод инвентаря 
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		printf("Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);
	}
}

int find_item_id(char item_names[MAX_ITEMS][MAX_NAME_LEN], char name[]) { //ищем предмет по айди
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
		if (name[0] >= '0' && name[0] <= '9') {  // Если первый символ слова цифра , то это ID
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

int main() {
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
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
		printf("\nМЕНЮ\n"
			" [0] - Выход\n"
			" [1] - Посмотреть на часы\n"
			" [2] - Промотать время (Поработать)\n"
			" [3] - Посмотреть инвентарь\n"
			" [4] - Положить предмет в слот\n"
			" [5] - Выбросить предмет\n"
			" [6] - Сжатие инвентаря\n"
			"Введите номер -> ");

		if (!check(scanf("%d", &choice)))
		{
			continue; // если вместо числа ввели что-то другое — показываем меню снова 
		}

		switch (choice) {
		case 0:
			printf("До свидания!\n"); 
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
			for (int i = 0; i < INVENTORY_SIZE; i++) //отображение инвентаря по порядку
			{
				printf("Слот %d: [%d] (", i, inventory[i]);
				print_item(inventory[i]);
				printf(")\n");
			}
			break;
		case 4:
		{
			printf("Введите индекс слота (0-9): ");
			if (!check(scanf("%d", &slot)))
			{
				break;
			}
			if (slot < 0 || slot >= INVENTORY_SIZE) // 10-го слота нет
			{
				printf("Такого слота нет! Допустимо 0-%d.\n", INVENTORY_SIZE - 1); 
				break;
			}
			printf("Введите ID предмета (0-9): "); 
			if (!check(scanf("%d", &item_id)))
			{
				break;
			}
			if (item_id < 0 || item_id >= INVENTORY_SIZE) 
			{
				printf("Такого ID нет! Допустимо 0-%d.\n", INVENTORY_SIZE - 1);
				break;
			}
			inventory[slot] = item_id;
			printf("В слот %d помещен предмет: [%d] (", slot, item_id);
			print_item(item_id);
			printf(")\n");
			break;
		case 5:
			printf("Введите индекс слота для выброса (0-9): ");
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
			printf("Выброшено: [%d] (", inventory[slot]);
			inventory[slot] = 0;
			break;
		case 6:
			printf("До сжатия:\n");
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				printf("Слот %d: [%d] (", i, inventory[i]);
				print_item(inventory[i]);
				printf(")\n");
			}
			
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
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				printf("Слот %d: [%d] (", i, inventory[i]);
				print_item(inventory[i]);
				printf(")\n");
			}
			break;
		default:
			printf("Нет такого пункта, выберите 0-6.\n");
			break;
		}
	}
	return 0;
}
