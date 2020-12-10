#include <pthread.h>
#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>
#include <time.h>

typedef struct thread_arg
{
    int threadIdx;
    int width;
    int height;
    int num_threads;
    int windows_per_thread;
    unsigned char *original_image;
    unsigned char *new_image;

} thread_arg;

unsigned getIndexOfWindow(unsigned radius, unsigned grid_number)
{
    return ((radius * (grid_number / radius)) * 4 * 4 + (grid_number % radius) * 8);
}

//calculates the max value of the parsed array
unsigned find_maximum(unsigned a[], int n)
{
    int c, max = 0;

    for (c = 0; c < n; c++)
        if (a[c] > max)
            max = a[c];
    return max;
}

void pooling(unsigned index, unsigned color_at_pixel, unsigned width, unsigned char *image, unsigned char *new_image, unsigned thread_id)
{

    // pool [pixel[i], pixel[i+1], pixel[i+width], pixel[i+width+1]] of image to get 1 pixel for new_image
    for (int i = index; i < index + 4; i++)
    {
        unsigned grid[4] = {(unsigned)image[i], (unsigned)image[i + 4], (unsigned)image[i + width * 4], (unsigned)image[i + (width + 1) * 4]};
        new_image[color_at_pixel + (i - index)] = find_maximum(grid, 4);
    }
}

void process(int height, int width, int num_threads, int threadIdx, unsigned char *original_image, unsigned char *new_image, int windows_per_thread)
{
    //  printf("height %d width %d num_threads %d threadIdx %d original_image[0] %d", height, width, num_threads, threadIdx, original_image[0]);
    int count = 0;
    // Radius helps us read through the 1d array image, as if it were a 2D array with width/2 windows on the x and y axis
    unsigned radius = width / 2;

    // As long as the current thread has not completed its quota of windows (4 pixel grids to do pooling on), do pooling
    for (int i = 0; i < windows_per_thread; i++)
    {
        // start index to get where on the image the thread should start doing the pooling
        unsigned index_at_new_image = (threadIdx * windows_per_thread + i) * 4;
        pooling(getIndexOfWindow(radius, threadIdx * windows_per_thread + i), (index_at_new_image), width, original_image, new_image, threadIdx);
    }

    // If I am at the last thread and I have windows (grids od 4 pixels to be pooled) that have not been pooled...
    if (threadIdx == num_threads - 1)
    {
        unsigned remaining_windows = (width * height / 4) % num_threads;
        if (remaining_windows > 0)
        {
            for (int i = 0; i < remaining_windows; i++)
            {

                // Get the start index of pooling at the remaining window
                unsigned index_at_new_image = ((threadIdx + 1) * windows_per_thread + i) * 4;
                pooling(getIndexOfWindow(radius, (threadIdx + 1) * windows_per_thread + i), (index_at_new_image), width, original_image, new_image, threadIdx);
            }
        }
    }
}

// Start function for the background thread
void *bg_func(void *arg)
{
    struct thread_arg *args = (struct thread_arg *)arg;
    process(args->height, args->width, args->num_threads, args->threadIdx, args->original_image, args->new_image, args->windows_per_thread);
    return arg;
}

// Foreground thread and main entry point
int main(int argc, char *argv[])
{
    char *input_filename = "Test_3.png";
    double average = 0;
    int num_threads = 1;

    unsigned char *image, *new_image;
    unsigned width, height;

    unsigned error = lodepng_decode32_file(&image, &width, &height, input_filename);
    if (error)
        printf("error %u: %s\n", error, lodepng_error_text(error));

    size_t original_size = width * height * 4 * sizeof(unsigned char);
    size_t compressed_size = width * height * sizeof(unsigned char);

    image = (unsigned char *)malloc(original_size);

    new_image = (unsigned char *)malloc(compressed_size);

    // Number of 2x2 grids containing 4 pixels to do pooling on
    int num_windows = width * height / 4;

    // Number of 2x2 grids containing 4 pixels to do pooling on to give per thread to process
    int windows_per_thread = num_windows / num_threads;

    struct timespec t0, t1;
    timespec_get(&t0, TIME_UTC);
    process(height, width, 1, 0, image, new_image, windows_per_thread);

    timespec_get(&t1, TIME_UTC);
    double diff = (double)(t1.tv_sec - t0.tv_sec) + ((double)(t1.tv_nsec - t0.tv_nsec) / 1000000000L);
    average += diff;

    printf("%d threads took on average: %lf seconds\n", num_threads, average);

    return 0;
}
