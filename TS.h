//---------------------------------------------------------------------------

#ifndef TSH
#define TSH

#include "MOSPSolver.h"

struct MOSP_TS_Node {  // used to represent nodes in the network representations of solutions
					   // specifically used within the TS metaheuristic
  int mc;     // machine selected for the corresponding operation

  int m_prv;  // index of the previous operation in machine sequence
  int m_nxt;  // index of the next operation in machine sequence
  int j_prv;  // index of the previous operation in the job route
  int j_nxt;  // index of the next operation in the job route
			  // ---- if a nxt/prv node does not exist, the values are set to -1

  int i;       // index of the node in the array which is the same as operation index
  int weight;  // the processing time of the corresponding operation

  int head;    // earliest start time of an operation
  int tail;    // latest start time of an operation
  bool bCritical;   // = true when head = tail
};

class MOSP_TS;

struct MOSP_TS_NetSolution {  // network representation of solutions for TS metaheuristic
  MOSP_TS *TS;         // a pointer to the tabu search algorithm for problem
                       // and algorithm parameters

  MOSP_TS_Node *Nodes;    // for each operation, there is a node in the network
  int *FirstjNode;     // index of the first operation in the job tour
  int *LastjNode;      // index of the last operation in the job tour
  int *FirstmNode;     // index of the first node in the machine sequence
  int *LastmNode;      // index of the last operation in the machine sequence

  int Cmax;
  int LB;              // approximate lower bound for an interchange of nodes

  MOSP_TS_NetSolution(MOSP_TS *ts);
  ~MOSP_TS_NetSolution();

  void Copy(MOSP_TS_NetSolution *sol);

  bool InterpretNetwork();  // For the network definition using prv and nxt
							// variables associated with each node, along with
							// the first and last nodes for each job and each
							// machine, determine the heads (earliest start times)
							// tails (latest start times), criticality condition
							// for each node and the Cmax of the schedule.
							// returns true if the network is feasible (i.e.
							// does not contain cycles).

  bool Swap_m(int m, int n1, int n2); // Swap the two nodes with indexes n1 and n2 based on
									  // their machine sequence, returns true if successful
  bool Swap_j(int j, int n1, int n2); // Swap the two nodes with indexes n1 and n2 based on
									  // their job order, returns true if successful

  int GetCPath(int *pth);   // Finds an arbitrary critical path and put the node
                            // indexes in that path in "pth" and return its length
};

struct MOSP_Tabu {
  int u, v;      // indexes of nodes that define the arc (u, v)

  MOSP_Tabu() : u(-1), v(-1) {};
};

struct MOSP_Particle;  // defined the hPSOTS module and needed here to be defined
					   // as a friend of the MOSP_TS class

class MOSP_TS : public MOSP_Algorithm {
    friend class MOSP_Particle;
  protected:
	MOSP_TS(MOSP_Problem *P, char *N);

	int TabuSize;
	long nIterations;    // maximum number of iterations allowed
	float MaxTime;       // A limit on the computational time
	long nImpIterations; // maximum number of improvement iterations allowed

	int itr;        // current iteration number
	int itr_imp;    // iteration number of the last found best solution
	int LB;          // lower bound for the given problem

	int nTabus;        // current number of tabus in the list
	int TabuPntr;      // a pointer to the current position in the circular tabu list
	MOSP_Tabu *Tabus;  // the tabu list

	bool bTerminate;

  public:
	MOSP_TS(MOSP_Problem *P, int ts, long nitr=10000, long nimpitr=1000, float maxt=MAXFLOAT);
	~MOSP_TS();

	// ------------------------- Variables used by the constructive algorithm
	int *MCrt;          // Machine ready time
	bool *bMCready;     // = true if a machine is ready to process a job at a current time
	bool *bMCdone;      // = true if a machine is done with all its required jobs

	int *Jnft;          // Job's nearest finish time
	bool *bJcompleted;  // = true if all operations of a job are scheduled

	bool **bJM;              // = true if a job needs a machine
	bool *SchedulableJobs;   // = true if a job is ready to be processed on a machine
	bool *bScheduled;        // = true if an operations is scheduled and false otherwise
	int *SelJob;             // selected job for each machine

	//-------------------------- Variables used during the improvement phase
	int *jnxt;          // next schedulable operation in job tour
	int *mnxt;          // next schedulable operation in machine processing sequence

	int *CPath;         // Critical path

	MOSP_TS_NetSolution *S0, *S, *Sbest;    // initial, current and best solutions
	MOSP_TS_NetSolution *Stmp;   // temporary solution for evaluating moves

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	void AddTabu(int u, int v);
	void ClearTabus();
	bool isTabu(int u, int v);


	// ---------- Functions to be used by hPSOTS algorithm --------------------
	void hPSOTS_AllocateMemory();
	void hPSOTS_FreeMemory();

	void DS_MWR(int **MA);    // initialize with the DS/MWR algorithm with given
							  // matrix of machine assignments to jobs "MA"
							  // MA[c][j] = index of machine (zero based)
							  //            in processing center
							  //            c which is selected to process job j
	void DS_MCT(int **MA);    // Using MCT instead of MWR

	void Active_MCT(int **MA); // Active schedule generator with MCT rule

	void ConvertNetSol();

};


//---------------------------------------------------------------------------
#endif
