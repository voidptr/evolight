const int organism_lightmap[54] = {
  59, 58, 57, 56, 55, 54, 53, 52, 51,
  42, 43, 44, 45, 46, 47, 48, 49, 50,
  41, 40, 39, 38, 37, 36, 35, 34, 33,
  24, 25, 26, 27, 28, 29, 30, 31, 32,
  23, 22, 21, 20, 19, 18, 17, 16, 15,
  6,  7,  8,  9,  10, 11, 12, 13, 14
};

const int locus_lightmap[9][6] = {
  {59, 42, 41, 24, 23, 6},
  {58, 43, 40, 25, 22, 7},
  {57, 44, 39, 26, 21, 8},
  {56, 45, 38, 27, 20, 9},
  {55, 46, 37, 28, 19, 10},
  {54, 47, 36, 29, 18, 11},
  {53, 48, 35, 30, 17, 12},
  {52, 49, 34, 31, 16, 13},
  {51, 50, 33, 32, 15, 14}
};

const int mut_light = 0;
const int reset_light = 1; 
const int step_light = 2;
const int competition_light = 3;
const int selection_light = 4;
const int inheritance_light = 5;
