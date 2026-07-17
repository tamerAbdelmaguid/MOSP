//---------------------------------------------------------------------------

#ifndef hPSOTSH
#define hPSOTSH

#include "TS.h"


struct MOSP_Particle { // a class for particle definition in PSO
  float **X;   // a vector of real numbers which takes the form of a materix.
			   // each raw in the matrix correspond to a processing center
			   // and the columns correspond to jobs.
  float **delta;
  float **Xbest;  // best found position for the particle
  int **MA;       // machine assignments vector which is based on X

  float *Y;       // termporary storage for X values of a given job
  int *O;         // A list that is used to store the sorting results of X

  MOSP_TS *TS;    // associated tabu search class

  MOSP_Particle(MOSP_Problem *P, int ts, float delta0);   // ts = size of tabu list
  ~MOSP_Particle();

  void RandomizeX();  // provide initial random values for X
  void XtoM();        // convert X to M
  void DS_MWR();      // a function that calls the initialization procedure of TS
					  // to generate an initial solution based on machine assignments
					  // given in M
  void DS_MCT();
  void Active_MCT();  // Active schedule construction with MCT

  void Construct();   // Construct initial solution

  // a set of functions that access algorithm information from TS
  int Cmax();         // current makespan for the particle
  int bestCmax();     // best makespan found by the particle
  int itr_imp();      // number of iterations conducted so far without improvement
  bool Terminated();  // = true if no nontabu moves can be found

  void ClearTabu();   // Clear tabu list

  void UpdateBestPosition();
};

class MOSP_hPSOTS : public MOSP_Algorithm {
  protected:
	MOSP_hPSOTS(MOSP_Problem *P, char *N);

	int nParticles;      // number of particles

	int TabuSize;
	long itr_max_PSO;    // maximum number of iterations allowed for PSO
	long itr_max_TS;     // maximum number of iterations allowed for TS
	long itr_max_imp;    // maximum number of improvement iterations allowed for TS
	float MaxTime;       // A limit on the computational time

	int itr_PSO;         // current iteration number for PSO
	int itr_TS;          // TS iteration number

	int LB;              // lower bound for the given problem

	bool bTerminate;

	SList Particles;   // set of particles

	int bestCmax;      // current best makespan value
	float **Xbest;     // best globally found position with the best makespan

	float omega, delta0, c1, c2;

  public:
	MOSP_hPSOTS(MOSP_Problem *P, int np, long nitr_pso, int ts, long nitr_ts,
	            long nitr_imp, float tl, float omeg, float d1, float const1, float const2);
	~MOSP_hPSOTS();

    void EvaluateLB();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	void UpdateParticlesPositions();
	void UpdateParticlesSolutions();
};


//---------------------------------------------------------------------------
#endif
