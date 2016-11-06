SHARED_SRC := $(wildcard src/shared/*.c)
SHARED_OBJ := $(addprefix build/shared/, $(notdir $(SHARED_SRC:.c=.o)))

SERVER_SRC := $(wildcard src/server/*.c)
CLIENT_SRC := $(wildcard src/client/*.c)

all: folder server.bin client.bin


folder:
	mkdir build/shared -p

build/libchatshared.a: $(SHARED_OBJ)
	ar -cvq $@ $(SHARED_OBJ)

build/shared/%.o: src/shared/%.c
	gcc -c -g  -o $@ $<

server.bin: $(SERVER_SRC) build/libchatshared.a
	gcc -g -o build/server.bin $(SERVER_SRC) -Lbuild -lchatshared -lpthread

client.bin: $(CLIENT_SRC) build/libchatshared.a
	gcc -g -o build/client.bin $(CLIENT_SRC) -Lbuild -lchatshared -lpthread

prepare_dir: 
	mkdir -p build/shared

clean:
	rm -r build
all: prepare_dir server.bin client.bin
