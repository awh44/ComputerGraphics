#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "status.h"

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius);
void usage(char *prog);

int main(int argc, char **argv)
{
	status_t error = SUCCESS;
	char *filename;
	double u_inc;
	double radius;

	if ((error = parse_args(argc, argv, &filename, &u_inc, &radius)))
	{
		usage(argv[0]);
		goto exit0;
	}

	printf("-f: %s, -u: %lf, -r: %lf\n", filename, u_inc, radius);

exit0:
	return error;
}

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius)
{
	*filename = "cpts_in.txt";
	*u_inc = .09;
	*radius = 1.0;
	
	char opt;
	while ((opt = getopt(argc, argv, "f:u:r:")) > 0)
	{
		switch (opt)
		{
			case 'f':
			{
				*filename = optarg;
				break;
			}

			case 'u':
			{
				char *end;
				*u_inc = strtod(optarg, &end);
				if (*u_inc < 0.0 || *u_inc > 1.0)
				{
					return ARGS_ERROR;
				}
				break;
			}

			case 'r':
			{
				char *end;
				*radius = strtod(optarg, &end);
				break;
			}
			
			case '?':
			{
				return ARGS_ERROR;
			}
		}
	}

	return optind == argc ? SUCCESS : ARGS_ERROR;
}

void usage(char *prog)
{
	fprintf(stderr,
		"usage: %s [-f filename] [-u 0.0 < increment < 1.0 ] [-r sphere radius]\n", prog);
}
