all: buildDir cpi

buildDir: ./build
	mkdir -p build

cpi: build/assembler.o build/interpreter.o build/lexer.o build/main.o \
	build/node.o build/parser.o build/util.o build/semantic.o build/bytecodegen.o
	clang++ -g -O0 -o build/cpi build/*.o

build/main.o: src/main.cpp
	clang++ -g -c -O0 -std=c++14 src/main.cpp -o build/main.o

build/%.o: src/%.cpp src/%.h
	clang++ -g -c -O0 -std=c++14 $< -o $@

clean:
	rm build/*.o