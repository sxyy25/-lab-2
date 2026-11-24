#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

/* --- КОНСТАНТЫ (лаб. 2-3) --- */
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

/* --- КОНСТАНТЫ (лаб. 4: Подземелье + туман войны) --- */
#define MIN_MAP_SIZE 5          /* минимальный размер стороны этажа */
#define MAX_MAP_SIZE 30         /* максимальный размер стороны этажа */
#define CELL_EMPTY '.'
#define CELL_WALL '#'
#define CELL_TREE 'T'
#define CELL_ROCK 'R'
#define CELL_HERO '@'
#define CELL_STAIR_DOWN 'D'     /* лестница вниз — на новый этаж */
#define CELL_STAIR_UP 'U'       /* лестница вверх — выход домой */
#define CELL_UNKNOWN '?'        /* клетка, скрытая туманом войны */
#define OBST_MIN_PCT 15         /* минимальная плотность препятствий, % */
#define OBST_MAX_PCT 30         /* максимальная плотность препятствий, % */
#define VIS_MIN_RADIUS 3        /* минимальный радиус тумана войны */
#define VIS_MAX_RADIUS 5        /* максимальный радиус тумана войны */
#define CMD_UP 'w'
#define CMD_DOWN 's'
#define CMD_LEFT 'a'
#define CMD_RIGHT 'd'
#define CMD_HOME 'q'
#define MAX_MAP_DIM (MAX_MAP_SIZE * MAX_MAP_SIZE) /* предел поисков клетки */

/* --- ПРОТОТИПЫ ФУНКЦИЙ (лаб. 2-3) --- */
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

/* --- ПРОТОТИПЫ ФУНКЦИЙ (лаб. 4: подземелье) --- */
char **create_map(int height, int width);
void free_map(char **map, int height);
void fill_dungeon(char **map, int height, int width);
int find_free_cell(const char **map, int height, int width, int *x, int *y);
int place_stair_up(char **map, int height, int width, int near_x, int near_y, int *ux, int *uy);
int is_walkable(char cell);
void render_dungeon(const char **map, int height, int width,
                    int hero_x, int hero_y, int vis_radius);
char read_move_command(void);
int generate_floor(char **map, int *height, int *width,
                   int *hero_x, int *hero_y, int *down_x, int *down_y, int *up_x, int *up_y);
void run_dungeon(void);

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

    srand((unsigned)time(NULL)); /* одно зерно на всю игру */

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
        printf("10 - Спуститься в подземелье (Лаб. 4: Подземелье + туман войны)\n");

        printf("Введи № действия (0-10): ");

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

            {
                size_t len = strlen(search_name);
                if (len > 0 && search_name[len - 1] == '\n')
                {
                    search_name[len - 1] = '\0';
                }
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

        case 10:
            run_dungeon();
            break;

        case 0:
            printf("До встречи, %s!\n", farmer_name);
            break;

        default:
            printf("Неверная команда. Выберите 0-10.\n");
            break;
        }
    } while (taskNum != 0);

    return 0;
}

/* --- РЕАЛИЗАЦИЯ ФУНКЦИЙ ЛАБ. 4 (все переменные объявлены в начале функции) --- */

/* Создаёт двумерный динамический массив карты.
   malloc — под массив указателей на строки, calloc — под каждую строку.
   При провале в середине освобождает всё, что успели выделить. */
char **create_map(int height, int width)
{
    char **map = NULL;
    int i = 0;

    map = (char **)malloc((size_t)height * sizeof(char *));
    if (map == NULL)
    {
        printf("Ошибка: не удалось выделить память под карту!\n");
        return NULL;
    }

    for (i = 0; i < height; i++)
    {
        map[i] = (char *)calloc((size_t)width, sizeof(char));
        if (map[i] == NULL)
        {
            /* откатываем всё, что успели выделить */
            while (--i >= 0)
            {
                free(map[i]);
            }
            free(map);
            printf("Ошибка: не удалось выделить память под строку карты!\n");
            return NULL;
        }
    }
    return map;
}

/* Освобождение памяти в обратном порядке: сначала строки, потом массив указателей */
void free_map(char **map, int height)
{
    int i = 0;

    if (map == NULL)
        return;

    for (i = 0; i < height; i++)
    {
        free(map[i]);
    }
    free(map);
}

/* Заполнение этажа подземелья: границы — стены #,
   внутри — пол с препятствиями плотностью 15-30% */
void fill_dungeon(char **map, int height, int width)
{
    int obstacle_pct = 0;
    int x = 0;
    int y = 0;

    obstacle_pct = OBST_MIN_PCT + rand() % (OBST_MAX_PCT - OBST_MIN_PCT + 1);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
                map[y][x] = CELL_WALL; /* замкнутая граница */
            else
                map[y][x] = CELL_EMPTY;
        }
    }

    /* препятствия — только во внутреннем пространстве */
    for (y = 1; y < height - 1; y++)
    {
        for (x = 1; x < width - 1; x++)
        {
            if (rand() % 100 < obstacle_pct)
            {
                if (rand() % 2 == 0)
                    map[y][x] = CELL_TREE;
                else
                    map[y][x] = CELL_ROCK;
            }
        }
    }
}

/* Поиск случайной свободной клетки ('.') */
int find_free_cell(const char **map, int height, int width, int *x, int *y)
{
    int attempts = 0;
    int cx = 0;
    int cy = 0;

    for (attempts = 0; attempts < MAX_MAP_DIM; attempts++)
    {
        cx = rand() % width;
        cy = rand() % height;
        if (map[cy][cx] == CELL_EMPTY)
        {
            *x = cx;
            *y = cy;
            return 1;
        }
    }

    /* резервный проход по порядку */
    for (cy = 0; cy < height; cy++)
    {
        for (cx = 0; cx < width; cx++)
        {
            if (map[cy][cx] == CELL_EMPTY)
            {
                *x = cx;
                *y = cy;
                return 1;
            }
        }
    }
    return 0;
}

/* Ставит лестницу вверх U рядом с точкой старта.
   Сначала проверяет 8 соседних клеток, при неудаче ищет ближайшую свободную
   спиралью (кроме самой точки старта). Возвращает успех/провал. */
int place_stair_up(char **map, int height, int width, int near_x, int near_y, int *ux, int *uy)
{
    int dx = 0;
    int dy = 0;
    int nx = 0;
    int ny = 0;
    int radius = 0;

    /* 1. непосредственные соседи точки старта */
    for (dy = -1; dy <= 1; dy++)
    {
        for (dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue; /* сама клетка старта не подходит */
            nx = near_x + dx;
            ny = near_y + dy;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                continue;
            if (map[ny][nx] == CELL_EMPTY)
            {
                map[ny][nx] = CELL_STAIR_UP;
                *ux = nx;
                *uy = ny;
                return 1;
            }
        }
    }

    /* 2. спиральный поиск ближайшей свободной клетки */
    for (radius = 2; radius <= MAX_MAP_DIM; radius++)
    {
        for (dy = -radius; dy <= radius; dy++)
        {
            for (dx = -radius; dx <= radius; dx++)
            {
                nx = near_x + dx;
                ny = near_y + dy;
                if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                    continue;
                if (nx == near_x && ny == near_y)
                    continue;
                if (map[ny][nx] == CELL_EMPTY)
                {
                    map[ny][nx] = CELL_STAIR_UP;
                    *ux = nx;
                    *uy = ny;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int is_walkable(char cell)
{
    return (cell == CELL_EMPTY ||
            cell == CELL_STAIR_DOWN ||
            cell == CELL_STAIR_UP);
}

/* Отрисовка этажа с туманом войны:
   видны только клетки в квадрате радиуса vis_radius вокруг героя,
   остальные скрыты символом '?' */
void render_dungeon(const char **map, int height, int width,
                    int hero_x, int hero_y, int vis_radius)
{
    int y = 0;
    int x = 0;
    int dist_x = 0;
    int dist_y = 0;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dist_x = x - hero_x;
            if (dist_x < 0)
                dist_x = -dist_x;
            dist_y = y - hero_y;
            if (dist_y < 0)
                dist_y = -dist_y;

            if (dist_x > vis_radius || dist_y > vis_radius)
            {
                putchar(CELL_UNKNOWN); /* туман войны */
            }
            else if (x == hero_x && y == hero_y)
            {
                putchar(CELL_HERO);
            }
            else
            {
                putchar(map[y][x]);
            }
        }
        putchar('\n');
    }
}

/* Чтение команды движения с защитой от дурака.
   Возвращает один из символов w/a/s/d/q в нижнем регистре. */
char read_move_command(void)
{
    char buffer[MAX_LINE_LEN];
    char cmd = 0;
    int i = 0;

    while (1)
    {
        printf("Куда идём? (W-вверх, A-влево, S-вниз, D-вправо, Q-домой): ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            return CMD_HOME; /* поток кончился — считаем отступлением домой */
        }

        for (i = 0; buffer[i] != '\0'; i++)
        {
            cmd = (char)tolower((unsigned char)buffer[i]);
            if (cmd == CMD_UP || cmd == CMD_DOWN ||
                cmd == CMD_LEFT || cmd == CMD_RIGHT || cmd == CMD_HOME)
            {
                return cmd;
            }
            if (cmd != ' ' && cmd != '\t' && cmd != '\n' && cmd != '\r')
            {
                break; /* первый осмысленный, но неверный символ */
            }
        }

        printf("Такого направления нет! Доступны W, A, S, D, Q.\n");
    }
}

/* Генерация нового этажа: заполняет карту, размещает героя,
   лестницу U рядом со стартом и лестницу D в случайном месте.
   Возвращает 1 при успехе, 0 при неудаче. */
int generate_floor(char **map, int *height, int *width,
                   int *hero_x, int *hero_y, int *down_x, int *down_y, int *up_x, int *up_y)
{
    if (!find_free_cell((const char **)map, *height, *width, hero_x, hero_y))
    {
        return 0;
    }

    if (!place_stair_up(map, *height, *width, *hero_x, *hero_y, up_x, up_y))
    {
        return 0;
    }

    if (!find_free_cell((const char **)map, *height, *width, down_x, down_y))
    {
        return 0;
    }
    map[*down_y][*down_x] = CELL_STAIR_DOWN;

    return 1;
}

/* Приключение: подземелье с туманом войны, лестницами и счётчиком глубины */
void run_dungeon(void)
{
    char **map = NULL;
    int height = 0;
    int width = 0;
    int hero_x = 0;
    int hero_y = 0;
    int down_x = 0;
    int down_y = 0;
    int up_x = 0;
    int up_y = 0;
    int depth = 1;      /* глубина: этаж 1 — старт */
    int vis_radius = 0; /* радиус тумана войны для этой вылазки */
    int new_x = 0;
    int new_y = 0;
    char cmd = 0;
    int running = 1;

    /* радиус видимости 3-5, выбирается один раз на всю вылазку */
    vis_radius = VIS_MIN_RADIUS + rand() % (VIS_MAX_RADIUS - VIS_MIN_RADIUS + 1);

    /* --- генерация первого этажа --- */
    height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

    map = create_map(height, width);
    if (map == NULL)
    {
        return;
    }

    fill_dungeon(map, height, width);

    if (!generate_floor(map, &height, &width,
                        &hero_x, &hero_y, &down_x, &down_y, &up_x, &up_y))
    {
        printf("Не удалось разместить героя и лестницы — этаж слишком тесен.\n");
        free_map(map, height);
        return;
    }

    printf("\nВы спускаетесь в подземелье! Глубина: %d. Радиус видимости: %d\n",
           depth, vis_radius);
    printf("Легенды: # — стена, T — корень, R — камень, D — вниз, U — наверх, @ — вы, ? — туман.\n");
    printf("Встаньте на U и нажмите Q, чтобы вернуться домой. D ведёт глубже.\n");

    while (running)
    {
        printf("\n--- Этаж %d (%dx%d), радиус видимости: %d ---\n",
               depth, width, height, vis_radius);
        render_dungeon((const char **)map, height, width, hero_x, hero_y, vis_radius);

        cmd = read_move_command();

        if (cmd == CMD_HOME)
        {
            if (hero_x == up_x && hero_y == up_y)
            {
                printf("Вы поднимаетесь по лестнице U и выбираетесь из подземелья!\n");
                running = 0;
            }
            else
            {
                printf("Домой можно уйти, только стоя на лестнице вверх (U)! Ищите её рядом со стартом.\n");
            }
            continue;
        }

        /* вычисляем новые координаты */
        new_x = hero_x;
        new_y = hero_y;
        if (cmd == CMD_UP)
            new_y = hero_y - 1;
        else if (cmd == CMD_DOWN)
            new_y = hero_y + 1;
        else if (cmd == CMD_LEFT)
            new_x = hero_x - 1;
        else if (cmd == CMD_RIGHT)
            new_x = hero_x + 1;

        /* стены по периметру — за границу не выйти (иначе был бы Segfault) */
        if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height)
        {
            printf("Впереди стена подземелья! Дальше пути нет.\n");
        }
        else if (map[new_y][new_x] == CELL_WALL ||
                 map[new_y][new_x] == CELL_TREE ||
                 map[new_y][new_x] == CELL_ROCK)
        {
            printf("Дальше не пройти — на пути %c!\n", map[new_y][new_x]);
        }
        else
        {
            hero_x = new_x;
            hero_y = new_y;

            /* герой встал на лестницу вниз — новый этаж */
            if (map[hero_y][hero_x] == CELL_STAIR_DOWN)
            {
                depth++;
                printf("Вы спускаетесь по лестнице D... Глубина: %d\n", depth);

                free_map(map, height); /* старый этаж освобождается */

                height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
                width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

                map = create_map(height, width);
                if (map == NULL)
                {
                    return;
                }

                fill_dungeon(map, height, width);

                if (!generate_floor(map, &height, &width,
                                    &hero_x, &hero_y, &down_x, &down_y, &up_x, &up_y))
                {
                    printf("Новый этаж оказался непригодным для жизни. Эвакуация!\n");
                    free_map(map, height);
                    return;
                }
            }
        }
    }

    free_map(map, height);
    printf("Память карты освобождена (free). Возвращаемся на ферму.\n");
}

/* --- РЕАЛИЗАЦИЯ ФУНКЦИЙ ЛАБ. 2-3 (без изменений по смыслу) --- */

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
    int i = 0;
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

        {
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
        }
        break;
    }
    return (int)val_long;
}

void print_inventory(const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    int i = 0;
    int id = 0;

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
    int i = 0;

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
    int i = 0;
    int id = 0;

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
        size_t replace_len = strlen(replacement);

        if (replace_len > tag_len)
        {
            memmove(pos + replace_len, pos + tag_len, remaining - tag_len + 1);
        }
        else if (replace_len < tag_len)
        {
            memmove(pos + replace_len, pos + tag_len, remaining - tag_len + 1);
        }

        memcpy(pos, replacement, replace_len);
        pos += replace_len;
        count++;
    }

    return count;
}

void process_npc_dialogue(const char *farmer_name, int day, const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    char line[MAX_LINE_LEN];
    const char *item0_name = item_names[ITEM_EMPTY];
    char day_str[16];

    int id0 = inventory[0];
    if (id0 >= 0 && id0 < MAX_ITEMS)
    {
        item0_name = item_names[id0];
    }

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

    printf("\n=== Обработка письма от старосты (Вариант 4) ===\n");

    sprintf(day_str, "%d", day);

    {
        int line_num = 0;
        while (fgets(line, sizeof(line), in) != NULL)
        {
            line_num++;

            {
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                {
                    line[len - 1] = '\0';
                }
            }

            replace_tag_in_line(line, "<NAME>", farmer_name);
            replace_tag_in_line(line, "<DAY>", day_str);
            replace_tag_in_line(line, "<ITEM>", item0_name);

            printf("%s\n", line);
            fprintf(out, "%s\n", line);
        }
    }

    fclose(in);
    fclose(out);

    printf("=== Готово. Результат сохранён в output.txt ===\n");
}