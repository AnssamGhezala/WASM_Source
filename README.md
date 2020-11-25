# Execute the following
emcc lodepng.c  -O2 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=5 -o functions.html functions.c