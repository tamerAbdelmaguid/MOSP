//---------------------------------------------------------------------------
#ifndef randomH
#define randomH

extern float Znorm [];

void advance_random();
bool flip(float prob);
void initrandomnormaldeviate();
double noise(double mu , double sigma);
double randomnormaldeviate();
double frandom();
int rnd(int low, int high);
float rndreal(float lo , float hi);
void warmup_random(float random_seed);
float NormalRandom(float mean, float stdev);

//---------------------------------------------------------------------------
#endif
