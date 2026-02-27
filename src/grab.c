#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#define PROGRAM_NAME "grab"
#define MAX_PATTERN_SIZE 64
#define MAX_LINE_SIZE 128
/******************************************************************************/
typedef struct {
  char pattern[MAX_PATTERN_SIZE];
  FILE *input;
  FILE *output;
  bool help;
} arg_t;
/******************************************************************************/
arg_t getopts(int32_t, char **);
void find_match(arg_t);
void help(void);
void close_files(arg_t);
/******************************************************************************/
int32_t main(int32_t argc, char **argv)
{
  arg_t args = getopts(argc, argv);
  if (args.help) { help(); close_files(args); return 0; }

  find_match(args);

  close_files(args);
  return 0;
}


void find_match(arg_t args)
{
  bool match_found = false;
  char line[MAX_LINE_SIZE];

  uint32_t pattern_length = strlen(args.pattern) + 2;
  char pattern[pattern_length], compare[pattern_length];

  sprintf(pattern, "$%s", args.pattern);

  while (fgets(line, MAX_LINE_SIZE, args.input) != NULL) {
    strncpy(compare, line, pattern_length - 1);

    if (match_found) {
      if (line[0] == '$') { match_found = false; return; }
      fputs(line, args.output);
    }

    if (strcmp(compare, pattern) == 0) {
      fputs(line, args.output);
      match_found = true;
    } 
  }
}


arg_t getopts(int32_t argc, char **argv)
{
  int32_t c;
  arg_t args = {.pattern = "", .input = stdin, .output = stdout, .help = false};

  static struct option const long_options[] = {
    {"output", required_argument, NULL, 'o'},
    {"help", 0, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  while ((c = getopt_long(argc, argv, "o:h", long_options, NULL)) != -1) {
    switch(c) {
      case 'o':
        args.output = fopen(optarg, "w");
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
    strcpy(args.pattern, argv[optind]);
  }

  if (argv[optind + 1] != NULL) {
    args.input = fopen(argv[optind + 1], "r");
  }

  return args;
}

void help(void)
{
  printf("usage: %s [OPTION]... FIELD [FILE]\n", PROGRAM_NAME);
  printf("Grab TURBOMOLE control field.\n");
  printf("  -h, --help\t\tDisplay this message again. :)\n");
  printf("  -o, --output\tFILE\tOutput file.\n");
}

void close_files(arg_t args)
{
  fclose(args.input);
  fclose(args.output);
}
