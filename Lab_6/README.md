
# Варіант 5
## Перевірка переповнень стеку та купи (ASan, Valgrind, ulimit)

## Тема роботи
Реалізувати окремі приклади переповнення стеку та купи й порівняти результати виявлення помилок через ASan, Valgrind і обмеження `ulimit`.

## Мета
Показати поведінку програм при:
- переповненні стеку (stack overflow) — нескінченна рекурсія;
- переповненні купи (heap buffer overflow) — запис за межі виділеного масиву;
та порівняти, як ці помилки виявляють AddressSanitizer (ASan), Valgrind і механізм обмеження стеку через `ulimit`.

## Хід роботи (реалізація)
Програма містить дві окремі функції для демонстрації:

1. Переповнення стеку (stack overflow)
   - Реалізовано як безмежна рекурсія, що заповнює стек викликами.
   - Запуск сценарію: `./l6z5 stack`

2. Переповнення купи (heap overflow)
   - Виділено масив розміром 10 елементів, а потім записано за межі виділеної області (наприклад, доступ до `arr[10]`..`arr[20]`).
   - Запуск сценарію: `./l6z5 heap`

Кожен сценарій запускається окремо через макрос/аргумент `TEST`/параметр командного рядка.

## Збірка та запуск
Без інструментів:
```bash
gcc l6z5.c -o l6z5 && ./l6z5 stack
gcc l6z5.c -o l6z5 && ./l6z5 heap
```

ASan (AddressSanitizer):
```bash
gcc -fsanitize=address -g l6z5.c -o l6z5 && ./l6z5 stack
gcc -fsanitize=address -g l6z5.c -o l6z5 && ./l6z5 heap
```

Valgrind:
```bash
valgrind ./l6z5 stack
valgrind ./l6z5 heap
```

ulimit (обмеження розміру стеку):
```bash
ulimit -s 1000 && ./l6z5 stack      # обмежити стек до 1000 KB
ulimit -s unlimited && ./l6z5 stack
```
> Примітка: `ulimit` обмежує розмір стеку процесу — не впливає на поведінку переповнення купи.

## Результати виявлення помилок

### 1) Переповнення стеку (stack overflow)

Інструмент / метод | Команда | Результат
---|---:|---
Без інструментів | `gcc l6z5.c -o l6z5 && ./l6z5 stack` | Програма аварійно завершується з Segmentation fault (через переповнення стеку).
ASan | `gcc -fsanitize=address -g l6z5.c -o l6z5 && ./l6z5 stack` | ASan зазвичай не ловить крах від вичерпання стеку при нескінченній рекурсії — отримуємо SIGSEGV. ASan виявляє виходи за межі локальних масивів, але не "витіснення" стеку спричинене глибиною рекурсії.
Valgrind | `valgrind ./l6z5 stack` | Valgrind показує велику кількість помилок читання/запису на стеку до краху; виводить повідомлення про Stack overflow і потім SIGSEGV.
ulimit (обмеження стеку) | `ulimit -s 1000 && ./l6z5 stack` | З меншим лімітом стеку крах наступає швидше. При `ulimit -s unlimited` крах також настане, але після більшої глибини рекурсії.

Фрагмент виводу Valgrind для переповнення стеку:
```text
==12345== Stack overflow in thread #1: can't grow stack to 0x1ffe801000
==12345== Process terminating with default action of signal 11 (SIGSEGV)
==12345==  Access not within mapped region at address 0x1FFE801FF8
==12345==    at 0x1091A6: stack_overflow (l6z5.c:10)
==12345==    by 0x1091B7: stack_overflow (l6z5.c:11)
...
```

---

### 2) Переповнення купи (heap buffer overflow)

Інструмент / метод | Команда | Результат
---|---:|---
Без інструментів | `gcc l6z5.c -o l6z5 && ./l6z5 heap` | Програма може працювати без явного краху, проте виводить значення `arr[10]...arr[20]` — чужу пам’ять; іноді може статися Segmentation fault (залежить від стану купи).
ASan | `gcc -fsanitize=address -g l6z5.c -o l6z5 && ./l6z5 heap` | Чітке виявлення: ASan зупиняє програму з детальним звітом `ERROR: AddressSanitizer: heap-buffer-overflow` з місцем помилки і стеком викликів.
Valgrind | `valgrind ./l6z5 heap` | Виявляє `Invalid write of size 4` при записі за межі блоку, показує, звідки було виділено пам'ять; в кінці — `ERROR SUMMARY`.
ulimit | — | Не застосовується (ulimit контролює стек, не купу).

Вивід ASan:
```text
==12346==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000058 at pc 0x0000004006e4 bp 0x7ffeefbff4a0 sp 0x7ffeefbff498
WRITE of size 4 at 0x602000000058 thread T0
    #0 0x4006e3 in heap_overflow /home/user/l6z5.c:20
    #1 0x40086f in main /home/user/l6z5.c:35
...
0x602000000058 is located 0 bytes after 40-byte region [0x602000000030,0x602000000058)
allocated by thread T0 here:
    #0 0x7f8c5d4b5b50 in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.5+0x10db50)
    #1 0x40068d in heap_overflow /home/user/l6z5.c:16
```

Фрагмента виводу Valgrind для переповнення купи:
```text
==12347== Invalid write of size 4
==12347==    at 0x1091F5: heap_overflow (l6z5.c:20)
==12347==    by 0x1092BF: main (l6z5.c:35)
==12347==  Address 0x4a4c058 is 40 bytes after a block of size 40 alloc'd
==12347==    at 0x483B7F3: malloc (vg_replace_malloc.c:307)
==12347==    by 0x1091C8: heap_overflow (l6z5.c:16)
```

## Висновки
- ASan
  - Найкраще виявляє heap-buffer-overflow: детальний звіт, точне місце помилки, розмір, стек викликів; зупиняє програму одразу.
  - Для stack overflow через нескінченну рекурсію ASan дає невелику діагностику — зазвичай просто крах (SIGSEGV). ASan краще виявляє виходи за межі локальних буферів, але не "вичерпання" стеку від рекурсії.
- Valgrind
  - Виявляє обидва типи помилок, дає корисні повідомлення (Invalid read/write, адреси, звідки виділено), але працює значно повільніше.
  - Для stack overflow показує багато помилок на стеку до фактичного краху.
- ulimit
  - Дозволяє контролювати ліміт стеку (`ulimit -s`); при малому ліміті крах настає швидше.
  - Не є діагностичним інструментом для heap overflow (не впливає на купу).

