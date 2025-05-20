executable := vm
build-path := ./build

obj-path := $(build-path)/obj
test-obj-path := $(build-path)/test-obj
release-obj-path := $(build-path)/release-obj

release-exec-path := $(build-path)/$(executable)
exec-path := $(build-path)/debug-$(executable)
test-exec-path := $(build-path)/test-$(executable)

src-folder := ./src
test-folder := ./tests
headers := ./src/headers

SDL_CFLAGS ?= $(shell pkg-config --cflags sdl3)
SDL_LIBS ?= $(shell pkg-config --libs sdl3)

release-flags := -O3 -Wno-cpp -lm $(SDL_CFLAGS) $(SDL_LIBS)
debug-flags := -g -O0 -Wall -Wextra -Wpedantic -Wno-cpp -lm $(SDL_CFLAGS) $(SDL_LIBS)
valgrind-flags := --leak-check=yes --track-origins=yes -s --leak-check=full --show-leak-kinds=all

src-files := $(wildcard $(src-folder)/*.c)
main-file := $(src-folder)/main.c
src-files-no-main := $(filter-out $(main-file), $(src-files))
test-files := $(wildcard $(test-folder)/*.c)

src-objs := $(patsubst $(src-folder)/%.c, $(obj-path)/%.o, $(src-files))
src-objs-no-main := $(patsubst $(src-folder)/%.c, $(obj-path)/%.o, $(src-files-no-main))
test-objs := $(patsubst $(test-folder)/%.c, $(test-obj-path)/%.o, $(test-files))
release-objs := $(patsubst $(src-folder)/%.c, $(release-obj-path)/%.o, $(src-files))

$(shell mkdir -p $(obj-path) $(test-obj-path) $(release-obj-path))

# create objs with release flags
$(release-obj-path)/%.o: $(src-folder)/%.c
	cc $(release-flags) -I$(headers) -c $< -o $@

# create objs with debug flags
$(obj-path)/%.o: $(src-folder)/%.c
	cc $(debug-flags) -I$(headers) -c $< -o $@

# create test objs
$(test-obj-path)/%.o: $(test-folder)/%.c
	cc $(debug-flags) -I$(headers) -I$(test-folder) -c $< -o $@
	
# build release executable
$(release-exec-path): $(release-objs)
	cc $(release-flags) $^ -o $@

# build debug executable
$(exec-path): $(src-objs)
	cc $(debug-flags) $^ -o $@

# build test executable
$(test-exec-path): $(test-objs) $(src-objs-no-main)
	cc $(debug-flags) $^ -o $@

.PHONY: build-release
build-release: $(release-exec-path)

.PHONY: build-all
build-all: $(exec-path) $(release-exec-path) $(test-exec-path)

.PHONY: build
build: $(exec-path)

.PHONY: debug
debug: $(exec-path)
	$(exec-path) $(ARGS)

.PHONY: release
release: build-release
	$(exec-path) $(ARGS)

.PHONY: clean
clean:
	rm -rf $(build-path)

.PHONY: valgrind
valgrind: $(exec-path)
	valgrind $(valgrind-flags) $(exec-path) $(ARGS)

.PHONY: test
test: $(test-exec-path)
	$(test-exec-path)

.PHONY: fmt
fmt:
	clang-format -i -style=file $(wildcard $(test-folder)/*.c) $(wildcard $(src-folder)/*.c) $(wildcard $(headers)/*.h)

#aliases

.PHONY: br
br: build-release
	
.PHONY: b
b: build
	
.PHONY: ba
ba: build-all

.PHONY: d
d: debug

.PHONY: r
r: release

.PHONY: v
v: valgrind

.PHONY: bv
bv: build valgrind
	
.PHONY: t
t: test
