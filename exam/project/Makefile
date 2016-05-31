#server makefile

CFLAGS = -lpthread \
		 -lglut -lGL -lGLU -lm

LDCFLAGS += -std=c99 \
			-Wall

# 编译器
CC = gcc

# 源文件
SRC_C = main.c print.c image.c

# 最终生成文件
SRC_O = $(patsubst %.c,%.o,$(SRC_C))

OBJ := main

.PHONY: all

all: $(OBJ)
$(OBJ): $(SRC_O)
	@echo [Link $@]
	$(CC) $(LDCFLAGS) -o $@ $(SRC_O) $(CFLAGS)
	@echo [done]
	sync

%.o: %.c
	@echo [Compile $^ ...]
	$(CC)  -c $^ -o $@

clean:
	rm *.o

