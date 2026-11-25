# String: локальная сборка и тесты

Реализация класса `String` - объявления в `string.hpp`, реализация в `src/`
(условия задачи - в [readme.md](readme.md)).
Все тесты запускаются **локально** на Linux, macOS или WSL.
Нативный Windows не поддерживается - используй WSL.

## Что нужно установить

Три вещи (отдельно GoogleTest ставить **не надо** - CMake скачает его сам,
git тоже не нужен):

- CMake **≥ 3.16**
- C++-компилятор с поддержкой **C++20** (gcc 10+ или clang 12+);
  если компилятор умеет C++23 - сборка автоматически пойдёт на C++23
- `make`

Интернет нужен **только при первом configure** - CMake скачает GoogleTest
с github. После этого кэш лежит в папке сборки и сеть больше не нужна.

## Установка (один раз)

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake
```

### macOS

1. Компилятор и `make` - Command Line Tools:

   ```bash
   xcode-select --install
   ```

2. CMake (в Command Line Tools его **нет**):

   ```bash
   brew install cmake
   ```

   Нет Homebrew - скачай `.dmg` с [cmake.org](https://cmake.org/download/)
   и распакуй (sudo не нужен).

Проверка:

```bash
cmake --version   # должно быть >= 3.16
```

## Сборка и запуск тестов

Скрипт собирает проект и прогоняет все тесты:

```bash
bash build.sh
```

В конце печатается явный итог - `✅ Все тесты прошли` или
`❌ Тесты не прошли` (имя упавшего теста будет в выводе ctest).

Один конкретный тест (сборка + только этот кейс):

```bash
bash build.sh String.PopBack
```

Вручную - те же шаги, что делает `build.sh`:

```bash
cmake -B build
cmake --build build
cd build && ctest --output-on-failure
```

**Ожидаемый результат:** все тесты зелёные -
27 тестов (`ExampleTest.Simple`, `String.Basic`, `String.PopBack`, …),
в конце `100% tests passed, 0 tests failed out of 27`.

## Где писать решение

- `string.hpp` - объявления класса `String` и операторов (можно дополнять,
  например приватными хелперами);
- `src/*.cpp` - реализация. Можно раскладывать по нескольким файлам -
  CMake соберёт их в библиотеку `string_impl` и слинкует с тестами и play.
  Других `.cpp` вне `src/` не создавай.

Проект собирается на **C++23**, если компилятор его поддерживает,
иначе на **C++20**.

### Play (своя песочница)

```bash
printf "Vasya\n" | ./build/play_string
```

Печатает:

```
Enter your name:
Hello, Vasya. Have a nice day!
```

В `play/main.cpp` можно писать свой сценарий использования `String` -
таргет всегда включается в сборку, удобно для отладки.

### Тесты по отдельности

```bash
bash build.sh String.PopBack                          # собрать + один тест через ctest
./build/test_string --gtest_filter=String.PopBack     # то же напрямую
./build/test_string                                   # весь набор напрямую
cd build && ctest -N                                  # список всех тестов
```

### Как искать ошибки в решении

Каждый gtest-кейс зарегистрирован в ctest отдельно, поэтому упавший тест
называется прямо в выводе скрипта:

```
2/27 Test #2: String.Basic ...***Failed
```

или при зависании:

```
1/27 Test #1: String.LongStrings ...***Timeout 7.00 sec
```

- **`***Failed`** - не прошёл ассерт: `--output-on-failure` показывает
  ожидаемое/фактическое значение и `file:line` проверки
- **`***Timeout`** - тест завис (обычно бесконечный цикл в реализации);
  скрипты в этом случае сами печатают подсказку
- скрипты останавливаются на **первой** ошибке (`--stop-on-failure`);
  чтобы увидеть все упавшие тесты разом: `cd build && ctest --output-on-failure`
- живой вывод: `./build/test_string` - в терминале видно каждый
  `[ RUN ] ...`, последний напечатанный и есть зависший
- ASan-сборка при ошибке памяти печатает стек - читай верхние кадры,
  там файл/строка в твоём коде

## Сборка в IDE (CLion / VS Code)

Это обычный CMake-проект, поэтому IDE собирают его без скриптов:

- **CLion**: открой папку проекта - CLion сам выполнит configure по
  `CMakeLists.txt`; таргеты `test_string` / `play_string` появятся в
  конфигурациях, тесты - в панели тестов (gtest).
- **VS Code**: установи расширение CMake Tools, открой папку, выбери
  компилятор (kit) - configure/build/тесты доступны из панели;
  список тестов - в панели Testing (ctest).

Важно:

- первый configure требует интернет (CMake скачивает GoogleTest);
- ASan: отдельный профиль/пресет с `-DWITH_ASAN=ON` (на Linux работает
  сразу, на macOS ctest выставляет `detect_leaks`);
- `TIMEOUT 7` - свойство ctest: при запуске тестов через ctest IDE его
  соблюдает, при прямом запуске бинарника таймаута нет;
- нативный Windows: обычная сборка возможна, ASan - нет; используй WSL
  (VS Code: Remote - WSL, CLion: WSL toolchain).

## Сборка с ASan (обязательно прогонять чтобы поймать UB)

Перед сдачей обязаны быть зелёными **и** обычная сборка, **и** ASan-сборка.
Скрипт для ASan-сборки:

```bash
bash build_asan.sh
```

Вручную - те же шаги, добавляется один флаг:

```bash
cmake -B build-asan -DWITH_ASAN=ON
cmake --build build-asan
cd build-asan && ctest --output-on-failure
```

CMake сам делает всю платформенную работу
(на macOS для ctest автоматически выставляется `ASAN_OPTIONS=detect_leaks=1`).

Что ловит ASan-сборка:

| ОС      | ООБ / use-after-free / двойной free | Утечки памяти (LSan)            |
| ------- | ----------------------------------- | ------------------------------- |
| Linux   | да                                  | да (включён по умолчанию)       |
| macOS   | да                                  | да (включается ctest-ом)        |

Проверить, что ASan действительно активен:

```bash
ASAN_OPTIONS=verbosity=1 ./build-asan/test_string --gtest_list_tests
```

В выводе должны быть строки `AddressSanitizer ... Init done`
(и на Linux - `LeakSanitizer: checking for leaks`).

## Troubleshooting

- **Первый configure висит/падает на скачивании** - нужен доступ в интернет
  (gtest тянется с github). После успешного configure сеть больше не нужна.
- **CMake старый** - `cmake --version` показывает < 3.16. Поставь новый:
  Linux - из репозитория дистрибутива или [cmake.org](https://cmake.org/download/),
  macOS - `brew upgrade cmake`.
- **`No rule to make target` / сломанная сборка** - удали папку сборки и
  запусти скрипт заново: `rm -rf build && bash build.sh`.
- **Тест «висит»** - скорее всего в реализации бесконечный цикл; ctest сам
  убьёт тест через 7 секунд, покажет его имя с `***Timeout`, а скрипт
  напечатает подсказку про возможный бесконечный цикл.
- **`CMake Error: Unable to (re)create the private pkgRedirects directory`
  (или «фантомная» папка `build`, которая то есть, то нет)** - рассинхрон
  файловой системы WSL на `/mnt/c`. Удали `build`
  (`rm -rf build`), а если он «не удаляется/File exists» - перезапусти WSL
  из Windows (`wsl --shutdown`) и собери заново. Скрипт тоже подскажет это.
- **macOS: `clang: command not found`** - `xcode-select --install`.
- **macOS: ASan-отчёты без file:line** - собери ASan-сборку с
  `-DCMAKE_BUILD_TYPE=Debug` или прогони `dsymutil` по бинарнику.
- **WSL** - работает как Linux: те же команды и скрипты, надо открыть папку с проектом и работать в ней.
  Если на `/mnt/c` регулярно странности с файлами - перенеси проект в
  Linux-ФС: `cp -r /mnt/c/Users/<ты>/Downloads/string ~/string` и работай в `~/string`.
- **Нативный Windows** - не поддерживается: собирай в WSL.
- **Полная очистка** - `rm -rf build build-asan`.
