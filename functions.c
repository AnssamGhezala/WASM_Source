#include <pthread.h>
#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>

typedef struct thread_arg
{
    int threadIdx;
    int width;
    int height;
    int num_threads;
} thread_arg ;

// Start function for the background thread
void *bg_func(void *arg) {
    int     *iter = (void *)arg;
    printf("HELLO FROM THREAD %d ", *iter);
    // *iter = fibonacci(*iter);
    return arg;
}

// Foreground thread and main entry point
int main(int argc, char *argv[]) {
    char* input_filename = "small.png";
    char* output_filename = "Test_1_output.png"; 

    unsigned char* image, * new_image;
    unsigned width, height;

    unsigned error = lodepng_decode32_file(&image, &width, &height, input_filename);
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

    int num_threads = 4;
    pthread_t   thread_array[num_threads];
    
    int thread_args[4] = {5,10,12,30};

    for (int i =0; i < num_threads; i++) {

        // Create the background thread
        if (pthread_create(&thread_array[i], NULL, bg_func, &thread_args[i])) {
            perror("Thread create failed");
            return 1;
        }
    }

    for (int i =0; i < num_threads; i++) {
         // Wait for background thread to finish
        if (pthread_join(thread_array[i], NULL)) {
            perror("Thread join failed");
            return 2;
        }
    }

    // Show the result from background and foreground threads
    printf("\n");

    return 0;
}