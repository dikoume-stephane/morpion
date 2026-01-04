g++ -std=c++17 -Iinclude -o morpion src/*.cpp src/Core/*.cpp src/Graphics/*.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui.cpp libs/imgui/imgui_tables.cpp libs/imgui/imgui_widgets.cpp libs/imgui/backends/imgui_impl_sdl3.cpp libs/imgui/backends/imgui_impl_sdlrenderer3.cpp -o build/morpion.exe -I./libs/imgui -I./libs/imgui/backends -lSDL3 -lSDL3_image
cd build
.\morpion.exe
cd ..