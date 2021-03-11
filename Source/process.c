/*----------------------------------------------------------------------------------------------------
 * SOURCE FILE:    process.c
 *
 * PROGRAM:        process
 *
 * FUNCTIONS:      void usage (char **argv)
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
 * This program creates a number of worker processes to perform a basic prime number factorization.
 * The original process creates n workers, where n is the user specified value at the command line.
 * -------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "./lib/primedecompose.h"

// Function Prototypes
void usage (char **);

// Globals
#define OPTIONS 		"?w:"
#define MAX_FACTORS 	1024

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
 * Main entrypoint into process application
 * -------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int p_num, task_count, range, index, status = 0;
    pid_t child_pid, wpid;
    long elapsed, total_elapsed = 0;

    mpz_t dest[MAX_FACTORS];
    mpz_t *n;
    mpz_t start_num;

	FILE *fptr;
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
	
    p_num = atoi(argv[optind]);          		// number of processes
    task_count = atoi(argv[optind + 1]); 		// number of tasks to run per process
    range = p_num * task_count;          		// range of numbers to factorize
    mpz_init_set_str(start_num, argv[optind + 2], 10);

    n = (mpz_t *)malloc(sizeof(mpz_t) * range); // initialize numbers list

    for (int i = 0; i < range; i++)
    {
        mpz_add_ui(*(n + i), start_num, (unsigned long int)i); // create a range of numbers starting from start_num
    }       
	
    for (int i = 0; i < p_num; i++)
    {
        index = (i * task_count); // create fan of processes, each with unique starting number
        child_pid = fork();
        if (child_pid <= 0)
            break;
    }

    if (child_pid == -1)
    {
        perror("Fork Failed!");
        return 1;
    }
	
    if (child_pid == 0)
    {
        for (int i = 0; i < task_count; i++)
        {
            int l = decompose(*(n + index + i), dest, &elapsed, filename);
            for (int j = 0; j < l; j++)
            {
                mpz_clear(dest[j]);
            }
            total_elapsed += elapsed;
        }
		fptr = fopen(filename, "a");
		fprintf(fptr, "Process ID: %ld, time used: %ld msec\n", (long)getpid(), total_elapsed);
        printf("Process ID: %ld, time used: %ld msec\n", (long)getpid(), total_elapsed);

        exit(0);
    }
    else
    {
        while ((wpid = wait(&status)) > 0) // Wait for all child processes
        {
        }
        free(n);
    }
    return 0;
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

