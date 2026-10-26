#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <locale.h>

/* --- КОНСТАНТЫ --- */
#define INVENTORY_SIZE 10
#define HOURS_PER_DAY 24
#define MAX_ITEMS 10
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 128
#define MAX_FARMER_NAME 32

/* ID предметов */
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

/* --- ПРОТОТИПЫ ФУНКЦИЙ --- */
const char *get_default_item_name(int id);
int load_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
int read_int_safe(const char *prompt);
void print_inventory(const int *inventory, const char (*item_names)[MAX_NAME_LEN]);
void invert_inventory(int *inventory);
void get_farmer_name(char *name);
int find_item_by_name(const char (*item_names)[MAX_NAME_LEN], const char *target_name);
void write_diary(const char *farmer_name, int day, int hour, const int *inventory, const char (*item_names)[MAX_NAME_LEN]);
void process_npc_dialogue(const char *farmer_name, int day, const int *inventory, const char (*item_names)[MAX_NAME_LEN]);
int replace_tag_in_line(char *line, const char *tag, const char *replacement);


int main(void)
{
    /* Настройка локали */
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    /* --- ВСЕ ПЕРЕМЕННЫЕ ОБЪЯВЛЕНЫ ЗДЕСЬ (вверху main) --- */
    int current_day = 1;
    int current_hour = 8;
    char farmer_name[MAX_FARMER_NAME] = "Фермер";

    int inventory[INVENTORY_SIZE] = {
        ITEM_WOOD, ITEM_STONE, ITEM_EMPTY, ITEM_SEEDS, ITEM_IRON,
        ITEM_COAL, ITEM_AXE, ITEM_APPLE, ITEM_PICKAXE, ITEM_WHEAT};

    char item_names[MAX_ITEMS][MAX_NAME_LEN];

    int taskNum = -1;
    int work_hours = 0;
    int slot = 0;
    int item_id = 0;
    char search_name[MAX_NAME_LEN] = {0};

    /* Переменные для циклов (объявлены здесь для совместимости со старым C) */
    int i = 0;
    int found_in_inventory = 0;
    int found_id = -1;

    load_item_names(item_names);
    get_farmer_name(farmer_name);

    do
    {
        printf("\n=== Ферма [%s] ===\n", farmer_name);
        printf("Время: День %d, %02d:00\n", current_day, current_hour);
        printf("────────────────\n");
        printf("0 - Выход\n");
        printf("1 - Посмотреть на часы\n");
        printf("2 - Поработать\n");
        printf("3 - Посмотреть инвентарь\n");
        printf("4 - Положить предмет в слот\n");
        printf("5 - Выбросить предмет\n");
        printf("6 - Инверсия карманов\n");
        printf("7 - Поиск предмета по названию\n");
        printf("8 - Записать состояние в дневник (diary.txt)\n");
        printf("9 - Обработать письмо от старосты (шаблонизатор NPC)\n");


        printf("Введи № действия (0-9): ");

        if (scanf("%d", &taskNum) != 1)
        {
            printf("Ошибка: введите число!\n");
            while (getchar() != '\n')
                ;
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
            work_hours = read_int_safe("Сколько часов работать?: ");
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
            slot = read_int_safe("Введи номер слота (0-9): ");
            if (slot < 0 || slot >= INVENTORY_SIZE)
            {
                printf("Такого слота нет (0-%d)\n", INVENTORY_SIZE - 1);
                break;
            }
            item_id = read_int_safe("Введите ID предмета (0-9): ");
            if (item_id < 0 || item_id >= MAX_ITEMS)
            {
                printf("Недопустимый ID предмета.\n");
                break;
            }
            inventory[slot] = item_id;
            printf("В слот %d помещён: [%s]\n", slot, item_names[item_id]);
            break;

        case 5:
            slot = read_int_safe("Введи слот для удаления (0-9): ");
            if (slot < 0 || slot >= INVENTORY_SIZE)
            {
                printf("Такого слота нет.\n");
                break;
            }
            item_id = inventory[slot];
            inventory[slot] = ITEM_EMPTY;

            if (item_id >= 0 && item_id < MAX_ITEMS)
                printf("Вы выкинули [%s] из слота %d\n", item_names[item_id], slot);
            else
                printf("Вы очистили слот %d (ранее там был некорректный ID)\n", slot);
            break;

        case 6:
            printf("Инвентарь ДО инверсии:\n");
            print_inventory(inventory, item_names);
            invert_inventory(inventory);
            printf("Инвентарь ПОСЛЕ инверсии:\n");
            print_inventory(inventory, item_names);
            break;

        case 7:
            printf("Введите название предмета для поиска (например, Дерево): ");

            if (fgets(search_name, sizeof(search_name), stdin) == NULL)
                break;

            size_t len = strlen(search_name);
            if (len > 0 && search_name[len - 1] == '\n')
            {
                search_name[len - 1] = '\0';
            }

            found_id = find_item_by_name(item_names, search_name);

            if (found_id == -1)
            {
                printf("Предмет \"%s\" не найден в каталоге.\n", search_name);
            }
            else
            {
                printf("ID предмета \"%s\" равен %d.\n", search_name, found_id);

                found_in_inventory = 0;
                for (i = 0; i < INVENTORY_SIZE; ++i)
                {
                    if (inventory[i] == found_id)
                    {
                        printf("  -> Найден в слоте %d\n", i);
                        found_in_inventory = 1;
                    }
                }
                if (!found_in_inventory)
                {
                    printf("  -> Предмет есть в каталоге, но нет в инвентаре.\n");
                }
            }
            break;

        case 8:
            write_diary(farmer_name, current_day, current_hour, inventory, item_names);
            printf("Дневник успешно сохранен в diary.txt\n");
            break;
            case 9:
    process_npc_dialogue(farmer_name, current_day, inventory, item_names);
    break;


        case 0:
            printf("До встречи, %s!\n", farmer_name);
            break;

        default:
            printf("Неверная команда. Выберите 0-8.\n");
            break;
        }
    } while (taskNum != 0);

    return 0;
}

/* --- РЕАЛИЗАЦИЯ ФУНКЦИЙ (ВСЕ переменные i объявлены в начале!) --- */

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

int load_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    FILE *file = fopen("items.txt", "r");
    int i = 0; /* Объявление i в начале функции */
    int count = 0;
    char line[MAX_LINE_LEN];

    if (file == NULL)
    {
        for (i = 0; i < MAX_ITEMS; ++i)
        {
            const char *def = get_default_item_name(i);
            strncpy(item_names[i], def, MAX_NAME_LEN - 1);
            item_names[i][MAX_NAME_LEN - 1] = '\0';
        }
        return 0;
    }

    while (count < MAX_ITEMS && fgets(line, sizeof(line), file) != NULL)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
        strncpy(item_names[count], line, MAX_NAME_LEN - 1);
        item_names[count][MAX_NAME_LEN - 1] = '\0';
        count++;
    }
    fclose(file);

    for (i = count; i < MAX_ITEMS; ++i)
    {
        const char *def = get_default_item_name(i);
        strncpy(item_names[i], def, MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
    }
    return 0;
}

int read_int_safe(const char *prompt)
{
    long val_long;
    char buffer[MAX_LINE_LEN];

    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            return 0;

        char *endptr;
        val_long = strtol(buffer, &endptr, 10);

        if (endptr == buffer || (*endptr != '\n' && *endptr != '\0'))
        {
            printf("Некорректный ввод. Введите целое число.\n");
            continue;
        }
        if (val_long < INT_MIN || val_long > INT_MAX)
        {
            printf("Число слишком большое.\n");
            continue;
        }
        break;
    }
    return (int)val_long;
}

void print_inventory(const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    int i = 0;  /* Объявление i в начале функции */
    int id = 0; /* Объявление id в начале функции */

    for (i = 0; i < INVENTORY_SIZE; ++i)
    {
        id = inventory[i];
        if (id < 0 || id >= MAX_ITEMS)
            id = ITEM_EMPTY;
        printf("Слот %d: [%d] (%s)\n", i, id, item_names[id]);
    }
}

void invert_inventory(int *inventory)
{
    int left = 0;
    int right = INVENTORY_SIZE - 1;
    int temp = 0;

    while (left < right)
    {
        temp = inventory[left];
        inventory[left] = inventory[right];
        inventory[right] = temp;
        ++left;
        --right;
    }
}

void get_farmer_name(char *name)
{
    printf("Как зовут вашего фермера? (макс %d символов): ", MAX_FARMER_NAME - 1);
    if (fgets(name, MAX_FARMER_NAME, stdin) != NULL)
    {
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n')
            name[len - 1] = '\0';
        if (strlen(name) == 0)
            strcpy(name, "Фермер");
    }
    else
    {
        strcpy(name, "Фермер");
    }
}

int find_item_by_name(const char (*item_names)[MAX_NAME_LEN], const char *target_name)
{
    int i = 0; /* Объявление i в начале функции */

    for (i = 0; i < MAX_ITEMS; ++i)
    {
        if (strcmp(item_names[i], target_name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void write_diary(const char *farmer_name, int day, int hour, const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    FILE *file = fopen("diary.txt", "a");
    int i = 0;  /* Объявление i в начале функции */
    int id = 0; /* Объявление id в начале функции */

    if (file == NULL)
    {
        printf("Ошибка: не удалось открыть файл diary.txt!\n");
        return;
    }

    fprintf(file, "--- Запись в дневник ---\n");
    fprintf(file, "Фермер: %s\n", farmer_name);
    fprintf(file, "Дата: День %d, Время %02d:00\n", day, hour);
    fprintf(file, "Содержимое рюкзака:\n");

    for (i = 0; i < INVENTORY_SIZE; ++i)
    {
        id = inventory[i];
        if (id < 0 || id >= MAX_ITEMS)
        {
            id = ITEM_EMPTY;
        }
        fprintf(file, "Слот %d: %s\n", i, item_names[id]);
    }
    fprintf(file, "------------------------\n\n");

    fclose(file);
}

int replace_tag_in_line(char *line, const char *tag, const char *replacement)
{
    int count = 0;
    char *pos = line;

    size_t tag_len = strlen(tag);
    if (tag_len == 0) return 0;

    while ((pos = strstr(pos, tag)) != NULL)
    {
        size_t remaining = strlen(pos);
        // Сдвигаем хвост строки, чтобы освободить место под замену
        size_t replace_len = strlen(replacement);
        if (replace_len > tag_len)
        {
            // Если замена длиннее тега, нужно сдвинуть хвост вправо
            memmove(pos + replace_len, pos + tag_len, remaining - tag_len + 1);
        }
        else if (replace_len < tag_len)
        {
            // Если короче — сдвигаем влево, чтобы убрать лишнее
            memmove(pos + replace_len, pos + tag_len, remaining - tag_len + 1);
        }

        // Вставляем замену
        memcpy(pos, replacement, replace_len);

        pos += replace_len;      // двигаемся дальше после вставки
        count++;
    }

    return count;
}

void process_npc_dialogue(const char *farmer_name, int day, const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    if (!in)
    {
        printf("Ошибка: не удалось открыть input.txt\n");
        return;
    }
    if (!out)
    {
        printf("Ошибка: не удалось создать output.txt\n");
        fclose(in);
        return;
    }

    char line[MAX_LINE_LEN];
    const char *item0_name = item_names[ITEM_EMPTY]; // по умолчанию "Пусто"

    // Получаем название предмета в нулевом слоте (если ID корректен)
    int id0 = inventory[0];
    if (id0 >= 0 && id0 < MAX_ITEMS)
    {
        item0_name = item_names[id0];
    }

    printf("\n=== Обработка письма от старосты (Вариант 4) ===\n");

    int line_num = 0;
    while (fgets(line, sizeof(line), in) != NULL)
    {
        line_num++;

        // Убираем \n для удобства, но сохраняем возможность вывода
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            len--;
        }

        // Заменяем теги
        replace_tag_in_line(line, "<NAME>", farmer_name);
        replace_tag_in_line(line, "<DAY>", line); // сначала <DAY> нельзя, он числовой
        // Чтобы заменить <DAY>, нужно подготовить строку-число
        char day_str[16];
        sprintf(day_str, "%d", day);
        replace_tag_in_line(line, "<DAY>", day_str);

        replace_tag_in_line(line, "<ITEM>", item0_name);

        // Выводим на экран
        printf("%s\n", line);

        // Пишем в файл (с новой строкой)
        fprintf(out, "%s\n", line);
    }

    fclose(in);
    fclose(out);

    printf("=== Готово. Результат сохранён в output.txt ===\n");
}
