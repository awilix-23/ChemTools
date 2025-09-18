#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#define PROGRAM_NAME "mio"
#define PI 3.1415
/******************************************************************************/
typedef struct {
  char element[3];
  double coord[3];
} Atom;

typedef struct {
  int length;
  int *array;
} IntArray;
/******************************************************************************/
void grab_indices(char *, IntArray *);
double distance(double *, double *);
double bond_angle(double *, double *, double *);
void centroid(Atom *, IntArray *, double *);
void convert(double *, char *);
/******************************************************************************/
int main(int argc, char *argv[])
{
  /* Initialization */
  int c;
  int atom_ind = 0;
  double dist_lim = 2.5;

  int calc_cent = 0;
  IntArray cent_ind;
  int calc_bond = 0;
  IntArray bond_ind;

  FILE *fp;
  char *in_file;
  int max_count;
  double energy;
  Atom *molecule;

  /* Parse Flags */
  static struct option const long_options[] = {
    {"limit", required_argument, NULL, 'l'},
    {"atoms", required_argument, NULL, 'a'},
    {"centroid", required_argument, NULL, 'c'},
    {"bond-angle", required_argument, NULL, 'b'},
    {NULL, 0, NULL, 0}
  };

  while ((c = getopt_long(argc, argv, "l:a:c:b:", long_options, NULL)) != -1) {
    switch(c) {
      case 'l':
        dist_lim = atof(optarg);
        break;
      case 'a':
        atom_ind = atoi(optarg) - 1;
        break;
      case 'c':
        calc_cent = 1;
        grab_indices(optarg, &cent_ind);
        break;
      case 'b':
        calc_bond = 1;
        grab_indices(optarg, &bond_ind);
        break;
      default:
        /* Add help function later ! */
        break;
    }
  }

  /* File check */
  if (argv[optind] == NULL) {
    fprintf(stderr, "%s: Input file required\n", PROGRAM_NAME);
    return 1;
  } else {
    in_file = argv[optind];
  }

  if ((fp = fopen(in_file, "r")) == NULL) {
    fprintf(stderr, "%s: Cannot open %s\n", PROGRAM_NAME, in_file);
    return 1;
  }

  fscanf(fp, "%d", &max_count);
  molecule = (Atom *)malloc((max_count + calc_cent) * sizeof(Atom));
  fscanf(fp, "%*s = %lf", &energy);

  /* Construct molecule */
  for (int i = 0; i <= max_count - 1; i++) {
    fscanf(fp, "%2s %lf %lf %lf",
        molecule[i].element,
        &molecule[i].coord[0],
        &molecule[i].coord[1],
        &molecule[i].coord[2]);
  }
  if (calc_cent) {
    strcpy(molecule[max_count].element, "Cn");
    centroid(molecule, &cent_ind, molecule[max_count].coord);
  }

  fclose(fp);

  /* Distance Calculations */
  int compare = atom_ind;
  double dist;
  for (int i = 0; i <= max_count - 1 + calc_cent; i++) {
    if ((compare != i) && ((dist = distance(molecule[compare].coord,
              molecule[i].coord)) <= dist_lim)) {
      convert(&dist, "pm");
      printf("%2d%2s -- %2d%2s\t%.2f pm\n",
          compare + 1, molecule[compare].element,
          i + 1, molecule[i].element,
          dist);
    }
  }

  /* Bond Calculations */
  if (calc_bond) {
    double angle;
    int i1, i2, i3;

    i1 = bond_ind.array[0];
    i2 = bond_ind.array[1];
    i3 = (calc_cent) ? max_count : bond_ind.array[2];

    angle = bond_angle(molecule[i1].coord, molecule[i2].coord,
        molecule[i3].coord);

    printf("%2d%2s -- %2d%2s -- %2d%2s\t%.2f\n",
        i1 + 1, molecule[i1].element,
        i2 + 1, molecule[i2].element,
        i3 + 1, molecule[i3].element,
        angle);
  }

  free(molecule);
  return 0;
}
/******************************************************************************/
void grab_indices(char *string, IntArray *indices)
{
  int commas = 0;
  char *token, *copy;

  for (const char *p = string; *p; p++) {
    if (*p == ',') commas++;
  }

  copy = malloc((strlen(string) + 1) * sizeof(char));
  strcpy(copy, string);

  indices->length = commas + 1;
  indices->array = malloc((commas + 1) * sizeof(int));

  token = strtok(copy, ",");
  for (int i = 0; i <= commas; i++) {
    indices->array[i] = atoi(token) - 1;
    token = strtok(NULL, ",");
  }

  free(copy);
}

double distance(double *a1, double *a2)
{
  double sum = 0;
  for (int i = 0; i <= 2; i++) {
    sum += (a1[i] - a2[i]) * (a1[i] - a2[i]);
  }
  return sqrt(sum);
}

double bond_angle(double *a1, double *a2, double *a3)
{
  double vec12, vec13, vec23, sum, angle, complement;

  vec12 = distance(a1, a2);
  vec13 = distance(a1, a3);
  vec23 = distance(a2, a3);
  sum = vec12 * vec12 + vec13 * vec13 - vec23 * vec23;

  angle =  acos( sum / (2 * vec12 * vec13) );
  angle *= 180 / PI;

  if ((complement = 180 - angle) > angle) {
    return complement;
  } else {
    return angle;
  }
}

void centroid(Atom *molecule, IntArray *indices, double *cent)
{
  double sumx = 0;
  double sumy = 0;
  double sumz = 0;
  for (int i = 0; i <= indices->length - 1; i++) {
    sumx += molecule[indices->array[i]].coord[0];
    sumy += molecule[indices->array[i]].coord[1];
    sumz += molecule[indices->array[i]].coord[2];
  }
  cent[0] = sumx / indices->length;
  cent[1] = sumy / indices->length;
  cent[2] = sumz / indices->length;
}

void convert(double *distance, char *unit)
{
  if (strcmp(unit, "pm") == 0) {
    *distance *= 100;
  } else if (strcmp(unit, "au") == 0) {
    *distance *= 1.89;
  }
}
