# pastel2d

![image](https://github.com/user-attachments/assets/8b784810-1b6f-4b8a-95e9-2cb9f267fd27)

**pastel2d** is 2D very thin static game library built on top of SDL3 and LuaJIT. Its main focus is its ability to embed
graphics, audio and libraries into it, making it possible to distribute a single executable containing the whole game.

Features:
 - binary resource embedding (graphics, fonts, audios, Lua scripts)
 - 2D scene construction
 - TTF support
 - MOD music support
 - WAV sound effect support
 - C and C++ library versions

How to use:
 - Example in C: [example.c](https://github.com/andrenho/pastel2d/blob/master/example/example.c)
 - Example in C++: [example.cc](https://github.com/andrenho/pastel2d/blob/master/example/example.cc)
 - How to embed binaries and use the library: [CMakeLists.txt](https://github.com/andrenho/pastel2d/blob/de8d75955d64305763db24ad35a28c36a2c552ef/CMakeLists.txt#L154)
