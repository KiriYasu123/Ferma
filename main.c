#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define INVENTORY_SIZE 10

int check (int scanf_result) { //проверка на дурака
	int c;
	if (scanf_result != 1) {
		while ((c = getchar()) != '\n' && c != EOF);  
			printf("Не целое число.\n");
		return 0;
	}
	return 1;
}

void print_item(int id) { //присвоение айдишников
	switch (id) {
	default: printf("???"); break;
	case 0: printf("Пусто");   break;
	case 1: printf("Дерево");  break;
	case 2: printf("Камень");  break;
	case 3: printf("Семена");  break;
	case 4: printf("Солома");  break;
	case 5: printf("Цветок");  break;
	case 6: printf("Железо");  break;
	case 7: printf("Шерсть");  break;
	case 8: printf("Мясо");    break;
	case 9: printf("Золото");  break;
	}
}

int main() {
	int current_day = 1;
	int current_hour = 8;
	int choice;
	int work_hours;
	int max_work_hours = 10000;
	int inventory[10] = {1, 0, 2, 0, 0, 3, 0, 4, 0, 5}; //заранее создаю несортированный инвентарь
	int slot;
	int item_id;
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
			return 0;
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
			for (int i = 0; i < INVENTORY_SIZE; i++)
			{
				printf("Слот %d: [%d] (", i, inventory[i]);
				print_item(inventory[i]);
				printf(")\n");
			}
			break;
		case 4:
			printf("Введите индекс слота (0-9): ");
			if (!check(scanf("%d", &slot)))
			{
				break;
			}
			if (slot < 0 || slot >= INVENTORY_SIZE)
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
		default:
			printf("Нет такого пункта, выберите 0-6.\n");
			break;
		}
	}
	return 0;
}

