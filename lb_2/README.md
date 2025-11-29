## Сборка

```bash

# Создаём и заходим в папку сборки
mkdir -p build && cd build
cmake .. 
make 

# Запуск программы, где после идет число с количеством потоков, а после количество запусков. ./ParallelArraySummer 2 10
./ParallelArraySummer
