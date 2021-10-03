@echo off
cl /Zi /I deps/include /I deps/lib main.c /link shell32.lib opengl32.lib user32.lib gdi32.lib glew32.lib glfw3_mt.lib glfw3.lib /LIBPATH:deps/lib