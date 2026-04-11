del build-pico -force -recurse -ErrorAction SilentlyContinue
del build-pico2 -force -recurse -ErrorAction SilentlyContinue
cmake -S . -B build-pico -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico
cmake -S . -B build-pico2 -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico2
cmake --build build-pico --config Release --parallel
cmake --build build-pico2 --config Release --parallel
#echo "------------------------------------------------------"
copy build-pico/Recognizer-ILI9341.uf2 release/Recognizer-ILI9341-pico.uf2
copy build-pico2/Recognizer-ILI9341.uf2 release/Recognizer-ILI9341-pico2.uf2
