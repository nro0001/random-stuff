//Nick Openshaw
//COMP3500
//PARALLELIZED THREAD DESCRIPTION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "pgma_io.h"

#define DEFAULT_NUM_BUCKETS 4 /* Default number of buckets for the histogram */

/* A grayscale image */
typedef struct
{
    int xsize; /* Image width, in pixels */
    int ysize; /* Image height, in pixesl */
    int maxg;  /* Maximum grayscale value (usually 255) */
    int *data; /* Pixel values (xsize*ysize ints, row-major from upper-left) */
} image_t;

/* Histogram for a grayscale image */
typedef struct
{
    int num_buckets;       /* Number of buckets in the histogram */
    int grays_per_bucket;  /* Number of gray values per bucket */
    long *count;           /* Pixel counts (one count per bucket) */
} histogram_t;

/*
 * Computes a histogram for the given image, with the given number of buckets,
 * storing the result in the given histogram structure.
 *
 * Returns 1 on success and 0 on failure.
 */
int compute_histogram(image_t *image, int buckets, histogram_t *histogram);

/*
 * Displays the given histogram in a human-readable form.
 */
void print_histogram(histogram_t *histogram);

int main(int argc, char **argv) {
    image_t image;
    histogram_t histogram;
    char *filename;
    int num_buckets;
    double start_time, stop_time;

    /* Determine the filename and number of buckets from command-line args */
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: histogram filename.pgm [num_buckets]\n");
        return 1;
    }

    filename = argv[1];

    num_buckets = DEFAULT_NUM_BUCKETS;
    if (argc == 3)
    {
        num_buckets = atoi(argv[2]);
        if (num_buckets < 1 || num_buckets > 128)
        {
            fprintf(stderr, "Number of buckets must be 1, 2, 4, 8, ..., 128\n");
            return 1;
        }
    }

    /* Load the image from disk */
    fprintf(stderr, "Reading PGM file...\n");
    pgma_read(filename, &image.xsize, &image.ysize, &image.maxg, &image.data);

    /* Compute the histogram and report the amount of elapsed time */
    fprintf(stderr, "Computing histogram...\n");
    start_time = omp_get_wtime();
    if (compute_histogram(&image, num_buckets, &histogram))
    {
        stop_time = omp_get_wtime();
        print_histogram(&histogram);
        fprintf(stderr, "Time to compute histogram: %1.3f seconds\n",
                stop_time - start_time);
    }

    /* Free heap-allocated memory (mostly to keep valgrind quiet) */
    free(image.data);
    free(histogram.count);

    return 0;
}

int compute_histogram(image_t *image, int num_buckets, histogram_t *histogram)
{
    int bucket, i;
    size_t size;

    if ((image->maxg + 1) % num_buckets != 0)
    {
        fprintf(stderr,
                "%d gray values cannot be evenly divided into %d buckets\n",
                image->maxg + 1,
                num_buckets);
        return 0;
    }

    histogram->num_buckets = num_buckets;

    /* If the histogram contains 4 buckets and gray values are in the range
       0..255, then there are 256 possible gray values, so 256/4 = 64 values
       are assigned to each bucket. */
    histogram->grays_per_bucket = (image->maxg + 1) / num_buckets;

    /* The histogram->count array contains one long value for each bucket */
    size = num_buckets * sizeof(long);
    histogram->count = (long *)malloc(size);
    if (histogram->count == NULL)
    {
        perror("malloc");
        return 0;
    }

    /* Initialize all of the counts to zero */
    memset(histogram->count, 0, size);

    /* For each pixel, increment the count for the corresponding bucket */
    for (i = 0; i < image->xsize * image->ysize; i++)
    {
        bucket = image->data[i] / histogram->grays_per_bucket;
        histogram->count[bucket]++;
    }

    return 1;
}

void print_histogram(histogram_t *histogram)
{
    int i, cutoff;
    long total = 0;
    for (i = 0; i < histogram->num_buckets; i++)
    {
        total += histogram->count[i];
    }

    for (i = 0, cutoff = 0;
         i < histogram->num_buckets;
         i++, cutoff += histogram->grays_per_bucket)
    {
        printf("Gray values %d-%d:\t%ld (%2.2f%%)\n",
               cutoff,
               cutoff + histogram->grays_per_bucket - 1,
               histogram->count[i],
               histogram->count[i] * 100.0 / total);
    }
}
