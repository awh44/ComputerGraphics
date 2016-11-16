#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sellipsoid.h"
#include "status.h"

typedef struct
{
	long num_u;
	long num_v;
	uint8_t use_flat;
	sellipsoid_t sellipsoid;
} args_t;

status_t parse_args(int argc, char **argv, args_t *args);
void usage(char *prog);
void print_to_iv(mesh_t *mesh);

int main(int argc, char **argv)
{
	status_t error = SUCCESS;

	args_t args;
	if ((error = parse_args(argc, argv, &args)))
	{
		usage(argv[0]);
		goto exit0;
	}

	mesh_t *mesh;
	if ((mesh = mesh_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	if ((error = sellipsoid_calculate_mesh_points(&args.sellipsoid, mesh, args.num_u, args.num_v)))
	{
		goto exit1;
	}

	/*
	if ((error = mesh_calculate_faces(mesh)))
	{
		fprintf(stderr, "ERROR: could not calculate faces for mesh\n");
		goto exit3;
	}
	*/

	/*
	if (!args.use_flat)
	{
		if ((error = bezier_surface_calculate_mesh_normals(bezier, mesh)))
		{
			fprintf(stderr, "ERROR: could not calculate normals for mesh\n");
			goto exit3;
		}
	}
	*/

	print_to_iv(mesh);

exit1:
	mesh_uninitialize(mesh);
exit0:
	return error;
}

status_t parse_args(int argc, char **argv, args_t *args)
{
	args->num_u = 19;
	args->num_v = 9;
	args->use_flat = 1;
	args->sellipsoid.s1 = 1;
	args->sellipsoid.s2 = 1;
	args->sellipsoid.A = 1;
	args->sellipsoid.B = 1;
	args->sellipsoid.C = 1;

	uint8_t seen_S = 0;
	uint8_t seen_F = 0;

#define CHECK_OR_RETURN(cond) do { if (cond) { return ARGS_ERROR; } } while (0)

	char opt;
	while ((opt = getopt(argc, argv, "u:v:FSr:t:A:B:C:")) > 0)
	{
		switch (opt)
		{	
			case 'u':
			{
				char *end;
				args->num_u = strtol(optarg, &end, 10);
				CHECK_OR_RETURN(args->num_u < 2 || *end != '\0');
				break;
			}

			case 'v':
			{
				char *end;
				args->num_v = strtol(optarg, &end, 10);
				CHECK_OR_RETURN(args->num_v < 2 || *end != '\0');
				break;
			}

			case 'r':
			{
				char *end;
				args->sellipsoid.s1 = strtod(optarg, &end);
				CHECK_OR_RETURN(*end != '\0');
				break;
			}

			case 't':
			{
				char *end;
				args->sellipsoid.s2 = strtod(optarg, &end);
				CHECK_OR_RETURN(*end != '\0');
				break;
			}

			case 'A':
			{
				char *end;
				args->sellipsoid.A = strtod(optarg, &end);
				CHECK_OR_RETURN(*end != '\0');
				break;
			}

			case 'B':
			{
				char *end;
				args->sellipsoid.B = strtod(optarg, &end);
				CHECK_OR_RETURN(*end != '\0');
				break;
			}

			case 'C':
			{
				char *end;
				args->sellipsoid.C = strtod(optarg, &end);
				CHECK_OR_RETURN(*end != '\0');
				break;
			}

			case 'F':
			{
				CHECK_OR_RETURN(seen_S);
				args->use_flat = 1;
				seen_F = 1;
				break;
			}

			case 'S':
			{
				CHECK_OR_RETURN(seen_F);
				args->use_flat = 0;
				seen_S = 1;
				break;
			}

			case '?':
			{
				return ARGS_ERROR;
			}
		}
	}

#undef CHECK_OR_RETURN

	return optind == argc ? SUCCESS : ARGS_ERROR;
}

void usage(char *prog)
{
	fprintf(stderr,
		"usage: %s\n"
		"	[-u 1 < number of u samples] [-v 1 < number of v samples]\n"
		"	[-r s1 value] [-t s2 value] [-A A value] [-B B value] [-C C value]\n"
		"	[-S smooth-shaded or -F flat-shaded]\n", prog);
}


void print_to_iv(mesh_t *mesh)
{
	printf("#Inventor V2.0 ascii\n");
	//mesh_print_to_iv(mesh, stdout);

	point3d_vec_t *points = mesh->points;
	size_t num = point3d_vec_size(points);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_print_to_iv(point3d_vec_get(points, i), stdout, 0.1);
	}
}
