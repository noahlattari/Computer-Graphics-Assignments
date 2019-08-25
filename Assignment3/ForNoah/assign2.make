# Linux (default)
EXE = interactionExample
LDFLAGS = -lGL -lGLU -lglut

# Windows (cygwin)
ifeq ($(OS), "Windows_NT")
    EXE = hello.exe
    LDFLAGS = -lopengl32 -lglu32 -lglut32
endif
# OS X
ifeq "$(shell uname)" "Darwin"
            LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
endif

$(EXE) : interactionExample.cpp
            g++ -o $@ $< $(CFLAGS) $(LDFLAGS)
