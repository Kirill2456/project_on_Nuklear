CC = gcc
TARGET = main
PREF_C = ./sources/
PREF_O = ./obj/
SRC = $(wildcard $(PREF_C)*.c) 
OBJ = $(patsubst $(PREF_C)%.c, $(PREF_O)%.o, $(SRC))

LDLIBS = -Wall -lws2_32 -lmingw32 -lm -lSDL2main -lSDL2_image -lSDL2  -lopengl32 
LDLIBS += -L./SDL2-devel-2.30.9-mingw/SDL2_image-devel-2.8.2-mingw/SDL2_image-2.8.2/x86_64-w64-mingw32/lib
LDLIBS += -L./SDL2-devel-2.30.9-mingw/SDL2-2.30.9/x86_64-w64-mingw32/lib

CFLAG = -I./head
CFLAG += -I./SDL2-devel-2.30.9-mingw/SDL2-2.30.9/x86_64-w64-mingw32/include
CFLAG += -I./SDL2-devel-2.30.9-mingw/SDL2_image-devel-2.8.2-mingw/SDL2_image-2.8.2/x86_64-w64-mingw32/include
CFLAG += -I./nuklear_cross/nuklear_drivers
CFLAG += -I./nuklear_cross 
CFLAG += -I./nuklear/demo/sdl_opengl2

HEADERS = $(wildcard head/*)

$(TARGET) : $(PREF_O) $(OBJ)
	$(CC) -g $(CFLAG) $(OBJ) -o $(TARGET) $(LDLIBS)

$(PREF_O) :
	mkdir $(PREF_O)

$(PREF_O)%.o : $(PREF_C)%.c $(HEADERS)
	$(CC) -c -g $(CFLAG) $< -o $@

clean :
	rm $(TARGET) $(PREF_O)*.o