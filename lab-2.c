#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define INVENTORY_SIZE 10
#define HOURS_PER_DAY 24

#define ITEM_EMPTY      0
#define ITEM_WOOD       1
#define ITEM_STONE      2
#define ITEM_SEEDS      3
#define ITEM_IRON       4
#define ITEM_COAL       5
#define ITEM_AXE        6
#define ITEM_APPLE      7
#define ITEM_PICKAXE    8
#define ITEM_WHEAT      9

const char* get_item_name(int id) {
    switch (id) {
        case ITEM_WOOD:   return "Дерево";
        case ITEM_STONE:  return "Камень";
        case ITEM_SEEDS:  return "Семена";
        case ITEM_IRON:   return "Железо";
        case ITEM_COAL:   return "Уголь";
        case ITEM_AXE:  return "Топор";
        case ITEM_APPLE:   return "Яблоко";
        case ITEM_PICKAXE:   return "Кирка";
        case ITEM_WHEAT:   return "Пшеница";
        default:          return "Пусто";
    }
}

int read_int_safe(const char* prompt) {
    int value;
    char buffer[256];

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            /* EOF или ошибка потока */
            printf("Ошибка ввода. Попробуйте снова.\n");
            continue;
        }

         size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        char* endptr;
        long val_long = strtol(buffer, &endptr, 10);

         if (*endptr != '\0' || buffer[0] == '\0') {
            printf("Некорректный ввод. Введите целое число.\n");
            continue;
        }

        if (val_long < INT_MIN || val_long > INT_MAX) {
            printf("Число слишком большое. Попробуйте снова.\n");
            continue;
        }

        value = (int)val_long;
        break;
    }
    return value;
}
