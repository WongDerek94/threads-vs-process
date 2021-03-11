/*----------------------------------------------------------------------------------------------------
 * SOURCE FILE:    thread.c
 *
 * PROGRAM:        thread
 *
 * FUNCTIONS:      void* task(void *thread_data)
 *                 void usage (char **argv)
 *
 * DATE:           January 23rd, 2021
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Derek Wong
 *
 * PROGRAMMER:     Derek Wong
 *
 * NOTES:
 * This program creates a number of worker threads to perform a basic prime number factorization.
 * The original process creates n threads, where n is the user specified value at the command line.
 * -------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#include "./lib/primedecompose.h"

// Function Prototypes
void* task(void *);
void usage (char **);

// Globals
#define OPTIONS 		"?w:"
#define MAX_FACTORS 	1024

struct ThreadInfo
{
    int id;
    char *filename;
    int count;
    mpz_t *list;
    long *elapsed;
    mpz_t *dest;
};

/*----------------------------------------------------------------------------------------------------
 * FUNCTION:       main
 *
 * DATE:           January 23rd, 2021
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Derek Wong
 *
 * PROGRAMMER:     Derek Wong
 *
 * INTERFACE:      int main (int argc, char *argv[])
 *
 * RETURNS:        int
 *
 * NOTES:
 * Main entrypoint into thread application
 * -------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int t_num, task_count, range;
    long elapsed;
    pthread_t *threads;
    struct ThreadInfo **thread_info_list;

    mpz_t **dest;
    mpz_t *n;
    mpz_t start_num;
	
    char *filename;

    int opt;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) // validate options and get filename
	{
		switch (opt)
        {
        case 'w':
            filename = optarg;
            break;
        default:
			case '?':
				usage(argv);
            exit(1);
        }
	}

	if (argc < 6) // check for valid number of command-line arguments
    {
        usage(argv);
        return 1;
    }

    t_num = atoi(argv[optind]);          // number of threads
    task_count = atoi(argv[optind + 1]); // number of tasks to run per threads
    range = t_num * task_count;          // range of numbers to factorize
    mpz_init_set_str(start_num, argv[optind + 2], 10);

    n = (mpz_t *)malloc(sizeof(mpz_t) * range); // initialize numbers list
	
    for (int i = 0; i < range; i++)
    {
        mpz_add_ui(*(n + i), start_num, (unsigned long int)i); 	// create a range of numbers starting from start_num
    }

    threads = malloc(sizeof(pthread_t) * t_num);       					// initialize worker list
    thread_info_list = malloc(sizeof(struct ThreadInfo *) * range); 	// initialize thread_info list
    dest = malloc(sizeof(mpz_t *) * t_num);            					// dest list for workers to store result
    for (int i = 0; i < t_num; i++)                        				// assign tasks to workers
    {
        thread_info_list[i] = (struct ThreadInfo *)malloc(sizeof(struct ThreadInfo));
        thread_info_list[i]->id = i;
        thread_info_list[i]->filename = filename;
        thread_info_list[i]->count = task_count;
        thread_info_list[i]->list = n + (i * task_count);
        thread_info_list[i]->elapsed = &elapsed;
        dest[i] = malloc(sizeof(mpz_t) * MAX_FACTORS);
        thread_info_list[i]->dest = dest[i];
        pthread_create(&threads[i], NULL, task, (void *)thread_info_list[i]);
    }
    for (int i = 0; i < t_num; i++)
	{
        pthread_join(threads[i], NULL);
	}
	
    for (int i = 0; i < range; i++)
    {
        free(thread_info_list[i]);
    }
	
    for (int i = 0; i < t_num; i++)
    {
        free(dest[i]);
    }

    free(thread_info_list);
    free(threads);
    free(dest);
    free(n);

    return 0;
}

/*----------------------------------------------------------------------------------------------------
 * FUNCTION:       task
 *
 * DATE:           January 23rd, 2021
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Derek Wong
 *
 * PROGRAMMER:     Derek Wong
 *
 * INTERFACE:      void* task(void *thread_data)
 *
 * RETURNS:        void
 *
 * NOTES:
 * Performs a prime number decomposition task and writes factorization results to file
 * Reports total elapsed time to console
 * -------------------------------------------------------------------------------------------------*/
void* task(void *thread_data)
{
    int i, j, l = 0;
    long total_elapsed = 0;
    struct ThreadInfo *thread_info = (struct ThreadInfo *)thread_data;
    for (i = 0; i < thread_info->count; i++)
    {
        l = decompose(*((thread_info->list) + i), thread_info->dest, thread_info->elapsed, thread_info->filename);
        for (j = 0; j < l; j++)
        {
            mpz_clear(thread_info->dest[j]);
        }
        total_elapsed += *(thread_info->elapsed);
    }
	FILE *fptr;
	fptr = fopen(thread_info->filename, "a");
    fprintf(fptr, "Thread ID: %ld, time used: %ld msec\n", (long)thread_info->id, total_elapsed);
    printf("Thread ID: %d, time used: %ld msec\n", thread_info->id, total_elapsed);
    return NULL;
}

/*----------------------------------------------------------------------------------------------------
 * FUNCTION:       usage
 *
 * DATE:           January 23rd, 2021
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Derek Wong
 *
 * PROGRAMMER:     Derek Wong
 *
 * INTERFACE:      void usage (char **argv)
 *
 * RETURNS:        void
 *
 * NOTES:
 * Reports usage of application at the command line
 * -------------------------------------------------------------------------------------------------*/
void usage (char **argv)
{
      fprintf(stderr, "Usage: %s <number of workers> <number of tasks per worker> <starting number to be factored> -w <filename>\n", argv[0]);
      exit(1);
}
