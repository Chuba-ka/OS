# Process File Mapping

Программа с двумя дочерними процессами и обменом через **memory-mapped files**.

## Сборка

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)

./bin/parent
./tests