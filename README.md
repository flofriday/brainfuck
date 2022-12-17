# Brainfuck

A repository to play with different brainfuck implementations

## Motivation 

I am currently working on a jit compiler generator as my bachelor thesis so I
wanted to actually implement some jit compiler to get a feeling for it and 
brainfuck is a small enough language that suits itself to it.

In the future I might use this repository to benchmark the generated JIT from
my bachelor thesis and compare with other JIT libraries.

## Progress

At the moment we only have a singel interpreter, I am currently researching 
some ways of how we could use llvm or other jit libraries to write a JIT 
compiler.

## Programs

## brainint

brainint is a naive interpreter that doesn't do any code analysis etc. The only 
optimization is that it only calculates all jump targets once and caches the 
results.

<!-- Ideas for further programs: brainbyte (a bytecode interpreter with code 
analysis), brainllvm (a jit compiler with llvm backend), brainunijit 
(a template based jit with unijit) -->

## Build it

You need the following requirements:
clang, ldd (not needed on macOS), cmake, ninja

```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```

## Usage

At the moment only the brainfuck interpreter `brainint` is available. The 
interpreter reads its input from stdin so you can use it like:

```bash
./brainint mandelbrot.bf
```