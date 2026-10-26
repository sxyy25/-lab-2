#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <locale.h> 

#define INVENTORY_SIZE 10
#define HOURS_PER_DAY 24

#define MAX_ITEMS 10
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 128

#define ITEM_EMPTY 0
#define ITEM_WOOD 1
#define ITEM_STONE 2
#define ITEM_SEEDS 3
#define ITEM_IRON 4
#define ITEM_COAL 5
#define ITEM_AXE 6
#define ITEM_APPLE 7
#define ITEM_PICKAXE 8
#define ITEM_WHEAT 9

const char *get_default_item_name(int id)
{
    switch (id)
    {
    case ITEM_WOOD:
        return "Дерево";
    case ITEM_STONE:
        return "Камень";
    case ITEM_SEEDS:
        return "Семена";
    case ITEM_IRON:
        return "Железо";
    case ITEM_COAL:
        return "Уголь";
    case ITEM_AXE:
        return "Топор";
    case ITEM_APPLE:
        return "Яблоко";
    case ITEM_PICKAXE:
        return "Кирка";
    case ITEM_WHEAT:
        return "Пшеница";
    default:
        return "Пусто";
    }
}

const char *get_item_name(int id)
{
    return get_default_item_name(id);
}

int load_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    FILE *file = fopen("items.txt", "r");
    
    if (file == NULL) {
        printf("Предупреждение: items.txt не найден. Используем стандартные названия.\n");
        
        int i;  // <-- Объявляем i ДО цикла
        for (i = 0; i < MAX_ITEMS; ++i) {
            const char *def = get_default_item_name(i);
            strncpy(item_names[i], def, MAX_NAME_LEN - 1);
            item_names[i][MAX_NAME_LEN - 1] = '\0';
        }
        return 0;  // <-- return вынесен ЗА пределы цикла
    }

    char line[MAX_LINE_LEN];
    int count = 0;

    while (count < MAX_ITEMS && fgets(line, sizeof(line), file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        strncpy(item_names[count], line, MAX_NAME_LEN - 1);
        item_names[count][MAX_NAME_LEN - 1] = '\0';
        count++;
    }

    fclose(file);

    int i;  // <-- Объявляем i ДО второго цикла
    for (i = count; i < MAX_ITEMS; ++i) {
        const char *def = get_default_item_name(i);
        strncpy(item_names[i], def, MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
    }

    return 0;
}


int read_int_safe(const char *prompt)
{
    int value;
    char buffer;

    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Ошибка ввода. Попробуйте снова.\n");
            continue;
        }

        char *endptr;
        long val_long = strtol(buffer, &endptr, 10);

        if (endptr == buffer || *endptr != '\0')
        {
            printf("Некорректный ввод. Введите целое число.\n");
            continue;
        }

        if (val_long < INT_MIN || val_long > INT_MAX)
        {
            printf("Число слишком большое. Попробуйте снова.\n");
            continue;
        }

        value = (int)val_long;
        break;
    }
    return value;
}

void print_inventory(const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    int i;         
    int id;         

    for (i = 0; i < INVENTORY_SIZE; ++i)
    {
        id = inventory[i];
        
    
        if (id < 0 || id >= MAX_ITEMS) {
            id = ITEM_EMPTY;
        }

    
        const char *name = (id >= 0 && id < MAX_ITEMS) ? item_names[id] : "Неизвестный";
        
        printf("Слот %d: [%d] (%s)\n", i, id, name);
    }
}

void invert_inventory(int *inventory)
{
    int left = 0;
    int right = INVENTORY_SIZE - 1;
    int temp;      

    while (left < right)
    {
        temp = inventory[left];
        inventory[left] = inventory[right];
        inventory[right] = temp;
        
        ++left;
        --right;
    }
}




int main(void)
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    int current_day = 1;
    int current_hour = 8;

    int inventory[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_STONE, ITEM_EMPTY, ITEM_SEEDS, ITEM_IRON,
        ITEM_COAL, ITEM_AXE, ITEM_APPLE, ITEM_PICKAXE, ITEM_WHEAT
    };

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    load_item_names(item_names);

    int taskNum;
    int work_hours;
    int slot, item_id;

    do
    {
        printf("────────────────\n"
               "0 - Выход\n"
               "1 - Посмотреть на часы\n"
               "2 - Поработать\n"
               "3 - Посмотреть инвентарь\n"
               "4 - Положить предмет в слот\n"
               "5 - Выбросить предмет\n"
               "6 - Инверсия карманов (задание по варианту)\n"
               "Введи № действия (0-6): ");

        if (scanf("%d", &taskNum) != 1)
        {
            printf("Э, я число просил\n");
            while (getchar() != '\n')
                ;
            taskNum = -1;
            continue;
        }
        while (getchar() != '\n')
            ;

        switch (taskNum)
        {
        case 1:
            printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
            break;

        case 2:
            printf("Сколько часов работать?: ");
            if (scanf("%d", &work_hours) != 1)
            {
                printf("Э, я число просил\n");
                while (getchar() != '\n')
                    ;
                break;
            }
            while (getchar() != '\n')
                ;

            if (work_hours < 0)
            {
                printf("Часы не могут быть отрицательными.\n");
                break;
            }

            current_hour += work_hours;
            while (current_hour >= HOURS_PER_DAY)
            {
                current_day++;
                current_hour -= HOURS_PER_DAY;
            }
            break;

        case 3:
            print_inventory(inventory, item_names);
            break;

        case 4:
            printf("Введи номер слота (0-9): ");
            if (scanf("%d", &slot) != 1)
            {
                printf("Не то\n");
                while (getchar() != '\n')
                    ;
                continue;
            }
            while (getchar() != '\n')
                ;

            if (slot < 0 || slot >= INVENTORY_SIZE)
            {
                printf("Такого слота нет родной (0-%d)\n", INVENTORY_SIZE - 1);
                continue;
            }

            item_id = read_int_safe("Введите ID предмета (0-9): ");

            if (item_id < 0 || item_id > 9)
            {
                printf("Недопустимый ID предмета. Допустимые значения: 0–9.\n");
                continue;
            }

            inventory[slot] = item_id;
            printf("В слот %d помещён предмет: [%d] (%s)\n", slot, item_id, item_names[item_id]);
            break;

        case 5:
            printf("Введи слот (0-9): ");
            if (scanf("%d", &slot) != 1)
            {
                printf("Э, я число просил\n");
                while (getchar() != '\n')
                    ;
                continue;
            }
            while (getchar() != '\n')
                ;

            if (slot < 0 || slot >= INVENTORY_SIZE)
            {
                printf("Такого слота нет родной (0-%d)\n", INVENTORY_SIZE - 1);
                continue;
            }

            item_id = inventory[slot];
            inventory[slot] = ITEM_EMPTY;
            printf("Ты выкинул [%s] из слота %d\n", item_names[item_id], slot);
            break;

        case 6:
            printf("Инвентарь ДО инверсии:\n");
            print_inventory(inventory, item_names);

            invert_inventory(inventory);

            printf("Инвентарь ПОСЛЕ инверсии:\n");
            print_inventory(inventory, item_names);
            break;

        case 0:
            printf("Ну пока\n");
            break;

        default:
            printf("Ты че? только 0-6\n");
            break;
        }
    } while (taskNum != 0);

    return 0;
}
