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

void print_inventory(const int inventory[]) {
    int i;
    for (i = 0; i < INVENTORY_SIZE; ++i) {
        int id = inventory[i];
        printf("Слот %d: [%d] (%s)\n", i, id, get_item_name(id));
    }
}

void invert_inventory(int inventory[]) {
    int left = 0;
    int right = INVENTORY_SIZE - 1;

    while (left < right) {
        int temp = inventory[left];
        inventory[left] = inventory[right];
        inventory[right] = temp;
        ++left;
        --right;
    }
}

int main(void) {

    int current_day = 1;
    int current_hour = 8;

    int inventory[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_STONE, ITEM_EMPTY, ITEM_SEEDS, ITEM_IRON,
        ITEM_COAL, ITEM_AXE, ITEM_APPLE, ITEM_PICKAXE, ITEM_WHEAT
    };

    int choice;

      while (1) {
        printf("\n=== МЕНЮ ===\n");
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Выполнить задание по варианту (Инверсия карманов)\n");

        choice = read_int_safe("Выберите пункт: ");

        switch (choice) {
            case 0:
                printf("Выход из игры.\n");
                return 0;

            case 1:
             printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
                break;

            case 2: {
                int hours_work = read_int_safe("Сколько часов вы хотите поработать? ");
                if (hours_work < 0) {
                    printf("Часы не могут быть отрицательными.\n");
                    break;
                }

                current_hour += hours_work;
                  break;
            }
             case 3:
                print_inventory(inventory);
                break;

            case 4: {
                int slot_index = read_int_safe("Введите индекс слота (0–9): ");
                if (slot_index < 0 || slot_index >= INVENTORY_SIZE) {
                    printf("Недопустимый индекс слота. Допустимые значения: 0–%d.\n", INVENTORY_SIZE - 1);
                    break;
                }

                int item_id = read_int_safe("Введите ID предмета (0–9): ");
                if (item_id < 0 || item_id > 9) {
                    printf("Недопустимый ID предмета. Допустимые значения: 0–9.\n");
                    break;
                }

                inventory[slot_index] = item_id;
                printf("Предмет с ID %d помещён в слот %d.\n", item_id, slot_index);
                break;
            }

            case 5: {
                int slot_index = read_int_safe("Введите индекс слота для удаления предмета (0–9): ");
                if (slot_index < 0 || slot_index >= INVENTORY_SIZE) {
                    printf("Недопустимый индекс слота.\n");
                    break;
                }

                int old_id = inventory[slot_index];
                inventory[slot_index] = ITEM_EMPTY;
                printf("Предмет из слота %d удалён (был ID %d).\n", slot_index, old_id);
                break;
            }

            case 6: {
                printf("Инвентарь ДО инверсии:\n");
                print_inventory(inventory);

                invert_inventory(inventory);

                printf("Инвентарь ПОСЛЕ инверсии:\n");
                print_inventory(inventory);
                break;
            }

            default:
                printf("Неверный пункт меню. Выберите от 0 до 6.\n");
                break;
        }
    }

    return 0;
}
