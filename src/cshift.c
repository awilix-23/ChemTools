#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#define PROGRAM_NAME "cshift"
/******************************************************************************/
typedef struct {
  FILE *ref_input;
  FILE *sub_input;
  FILE *output;
  char nuclei[3];
  double shift;
  bool aniso;
  bool help;
} arg_t;
/******************************************************************************/
arg_t getopts(int32_t, char **);
void help(void);
void close_files(arg_t);
double get_ref_shield(arg_t args);
void calculate_shifts(arg_t, double);
/******************************************************************************/
int32_t main(int32_t argc, char **argv)
{
  arg_t args = getopts(argc, argv);
  if (args.help) { help(); close_files(args); return 0; }

  double ref_shield = get_ref_shield(args);
  if (ref_shield == 0) {close_files(args); return 1;}

  calculate_shifts(args, ref_shield);

  close_files(args);
  return 0;
}


double get_ref_shield(arg_t args)
{
  double shield_const = 0;

  int16_t index = 0, mult = 0;
  char element[3];
  double iso = 0.0, aniso = 0.0, db = 0.0;

  while (fscanf(args.ref_input, "%hd %s %hd %lf %lf %lf",
        &index, element, &mult, &iso, &aniso, &db) != EOF) {
    if (strcmp(element, args.nuclei) == 0) {
      shield_const = (args.aniso) ? aniso : iso;
      break;
    }
  }

  return shield_const;
}

void calculate_shifts(arg_t args, double ref_shield)
{
  int16_t index = 0, mult = 0;
  char element[3];
  double sub_shield = 0.0, iso = 0.0, aniso = 0.0, db = 0.0;

  while (fscanf(args.sub_input, "%hd %s %hd %lf %lf %lf",
          &index, element, &mult, &iso, &aniso, &db) != EOF) {
      if (strcmp(element, args.nuclei) == 0) {
        sub_shield = (args.aniso) ? aniso : iso;
        fprintf(args.output, "%lf\n", args.shift + ref_shield - sub_shield);
      }
  }
}


arg_t getopts(int32_t argc, char **argv)
{
  int32_t c;
  arg_t args = {
    .nuclei = "h", .shift = 0, .aniso = false,
    .ref_input = NULL, .sub_input = stdin, .output = stdout,
    .help = false
  };

  static struct option const long_options[] = {
    {"nuclei", required_argument, NULL, 'n'},
    {"shift", required_argument, NULL, 's'},
    {"aniso", 0, NULL, 'a'},
    {"help", 0, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  while ((c = getopt_long(argc, argv, "n:s:ah", long_options, NULL)) != -1) {
    switch(c) {
      case 'n':
        strcpy(args.nuclei, optarg);
        break;
      case 's':
        args.shift = atof(optarg);
        break;
      case 'a':
        args.aniso = true;
        break;
      case 'h':
        args.help = true;
        break;
      default:
        break;
    }
  }

  if (argv[optind] == NULL) {
    args.help = true;
    return args;
  } else {
    args.ref_input = fopen(argv[optind], "r");
  }

  if (argv[optind + 1] != NULL) {
    args.sub_input = fopen(argv[optind + 1], "r");
  }

  return args;
}

void help(void)
{
  printf("usage: %s [OPTION]... REF_FILE [SUB_FILE]\n", PROGRAM_NAME);
  printf("Calculate chemical shifts for a given nuclei.\n");
  printf("  -h, --help\t\tDisplay this message again. :)\n");
  printf("  -n, --nuclei\tATOM\tNuclei to calculate shift for. (Default: h)\n");
  printf("  -s, --shift\tSHIFT\tChemical shift for reference. (Default: 0)\n");
  printf("  -a, --aniso\t\tCalculate anisotropic shielding instead.");
  printf("\nKnown Limitation(s):\n");
  printf("Inputs needs preprocessing => `grab nmr FILE | tail +3 > TMP`\n");
}

void close_files(arg_t args)
{
  if (args.ref_input != NULL) fclose(args.ref_input);
  fclose(args.sub_input);
  fclose(args.output);
}
