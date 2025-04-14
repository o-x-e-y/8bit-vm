executable := custom-emulator
build-path := ./build
exec-path := $(build-path)/$(executable)

src := ./src
headers := ./src/headers

debug-flags := -g -O0 -Wall
release-flags := -O3
valgrind-flags := --leak-check=yes --track-origins=yes -s --leak-check=full --show-leak-kinds=all

src_files := $(wildcard $(src)/*.c)

.PHONY: build
build: $(src_files)
	cc $(debug-flags) $(src_files) -o $(exec-path)

.PHONY: build-release
build-release: $(src_files)
	cc $(src_files) $(release-flags) -o $(exec-path)

.PHONY: debug
debug: $(src_files)
	cc $(debug-flags) $(src_files) -o $(exec-path) && $(exec-path)

.PHONY: release
release: $(src_files)
	cc $(release-flags) $(src_files) -o $(exec-path) && $(exec-path)

.PHONY: valgrind
valgrind: $(exec-path)
	valgrind $(valgrind-flags) $(exec-path)

.PHONY: fmt
fmt:
	clang-format -i -style=file $(wildcard $(src)/*.c) $(wildcard $(headers)/*.h)

#aliases

.PHONY: b
b: build

.PHONY: br
br: build-release

.PHONY: d
d: debug

.PHONY: r
r: release

.PHONY: v
v: valgrind

.PHONY: bv
bv: build valgrind
