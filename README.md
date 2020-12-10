# ECSE420: Parallel Programming - Final Project

## Pre-requesite
You first need to install emcc, to do so follow the official emscripten [documentation](https://emscripten.org/docs/getting_started/downloads.html)
<br>
<br>
There are 3 versions of the max-pooling programs that you can run. Each one of them gives a different performance (see results section of report for more details)

## 1. Sequential Max-pooling in JavaScript
From the root of the directory, go to the seq_js folder containing the code:<br>
```cd seq_js```
Open that JavaScript file in the browser

## 2. Sequential Max-pooling in WebAssembly
Fom the root of the directory, go to the seq_wasm folder:<br>
```cd seq_wasm```
Run the following command:<br>
```emcc lodepng.c -o test.js functions.c -s INITIAL_MEMORY=672137216 -s --embed-file Test_3.png```

## 3. Parallelized Max-pooling in WebAssembly
Fom the root of the directory, go to the seq_wasm folder: <br>
```cd para_wasm```
Set the number of commands within the main function, for instance 3:<br>
```int num_threads=3```
Run the following command to have 3 background threads running in parallel:<br>
```emcc lodepng.c -o test.js functions.c -s USE_PTHREADS=1 -s INITIAL_MEMORY=672137216 -s --embed-file Test_3.png -s PTHREAD_POOL_SIZE=3```
