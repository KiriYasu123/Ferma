#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
	int current_day = 1;
	int current_hour = 8;
	int choice;
	int work_hours;
	printf("МЕНЮ\n");
	printf(" [0] - Выход\n");
	printf(" [1] - Посмотреть на часы\n");
	printf(" [2] - Промотать время\n");
	printf(" [3] - Просмотреть инвентарь\n");
	printf(" [4] - Положить предмет в слот\n");
	printf(" [5] - Выбросить предмет\n");
	printf(" [6] - Сжатие инвентаря\n");
	printf("Введите номер -> ");
	scanf("%d", &choice);
	switch (choice) {
	case 1:
	{
		printf("Текущее время: День %d, %d:00\n", current_day, current_hour);
		break;
	}
	case 2:

		printf("Сколько часов работать?: ");
		if (scanf("%d", &work_hours) != 1)
		{
			printf("Э, я число просил\n");
			while (getchar() != '\n');
		}

		current_hour += work_hours;

		while (current_hour >= 24)
		{
			current_day++;
			current_hour -= 24;
		}
		break;
	}
}

