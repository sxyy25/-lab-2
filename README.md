ЛАБА 3.
2лаба:Инверсия карманов:
 Вывести инвентарь до и после преобразования.
3лаба:
Шаблонизатор диалогов для NPC
В input.txt хранится текст письма от старосты деревни, содержащий специальные теги-заглушки: <NAME>, <DAY> и <ITEM>. Программа должна построчно считать текст и заменить все вхождения <NAME> на текущее имя фермера, <DAY> — на номер текущего дня (current_day), а <ITEM> — на текстовое название предмета, который сейчас лежит в нулевом слоте инвентаря (inventory[0]). Готовый персонализированный текст письма сохранить в output.txt. 
Сычугов Илья Александрович
МБС-601 2-ая подгруппа.

void process_npc_dialogue(const char *farmer_name, int day, const int *inventory, const char (*item_names)[MAX_NAME_LEN])
{
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");
    char line[MAX_LINE_LEN];
    char day_str;          // Объявление в начале функции (совместимо с C89)
    int line_num = 0;
    size_t len;
    const char *item0_name = item_names[ITEM_EMPTY];
    int id0 = inventory;

    if (id0 >= 0 && id0 < MAX_ITEMS)
    {
        item0_name = item_names[id0];
    }

    if (!in || !out)
    {
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    printf("\n=== Обработка письма от старосты (Вариант 4) ===\n");

    while (fgets(line, sizeof(line), in) != NULL)
    {
        line_num++;
        len = strlen(line);

        if (len == MAX_LINE_LEN - 1 && line[len - 1] != '\n')
        {
            fprintf(stderr, "Предупреждение: строка %d обрезана.\n", line_num);
        }

        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        replace_tag_in_line(line, "<NAME>", farmer_name);

        snprintf(day_str, sizeof(day_str), "%d", day);
        replace_tag_in_line(line, "<DAY>", day_str);

        replace_tag_in_line(line, "<ITEM>", item0_name);

        printf("%s\n", line);
        fprintf(out, "%s\n", line);
    }

    fclose(in);
    fclose(out);
    printf("=== Готово. Результат сохранён в output.txt ===\n");
}

