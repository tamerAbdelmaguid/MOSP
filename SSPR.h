//---------------------------------------------------------------------------

#ifndef SSPRH
#define SSPRH
//---------------------------------------------------------------------------

#include "Problem.h"
#include "MOSPSolver.h"

//#define DEBUG_SSPR_RESULTS

#ifdef DEBUG_SSPR_RESULTS
#define PRNT_SSPR_DBG       // Uncomment this if you want to save debugging info to file "ssprDebug.txt"

#ifdef PRNT_SSPR_DBG
//#define PRNT_NSCB_DBG  // for showing neighborhood search and solution combination results
//#define PRNT_VNT_DBG   // for showing the generated vector, network and task list
//#define PRNT_DISTANCE_DBG   // for showing the details of the distance calculations
//#define PRNT_CONSTRUCTION_DBG   // for showing the details of the construction process
//#define PRNT_TS_DBG    // showing the details of Tabu Search
//#define PRNT_PR_DBG  // showing path relinking iterations info
#endif

#endif

int MOSPSSPRSolComp(void * Item1, void * Item2);

struct MOSP_SSPR_Node {  // used to represent nodes in the network representations of solutions
  int m_prv;  // index of the previous operation in machine sequence
  int m_nxt;  // index of the next operation in machine sequence
  int j_prv;  // index of the previous operation in the job route
  int j_nxt;  // index of the next operation in the job route
			  // ---- if a nxt/prv node does not exist, the values are set to -1

  int weight;  // the processing time of the corresponding operation

  int estart;    // [eta] earliest start time of an operation
  int lstart;    // [lambda] latest start time of an operation
  int tail;      // [gamma] tail of an operation, which is the remaining time to the completion of the schedule
  int slack;     // slack of an operation = lstart - estart
  bool bCritical;   // = true when estart = lstart

  int rank;      // number of all preceding nodes

  void Reset(){  // default node parameters
	m_prv = m_nxt = j_prv = j_nxt = -1;  // no connections with other nodes
	weight = 0;
	estart = lstart = tail = slack = -1;
	bCritical = false;
	rank = 0;
  };
};

class MOSP_SSPR;

class MOSP_SSPR_Solution {
	friend class MOSP_SSPR;

  private:
	MOSP_SSPR *sspr;

	// variables used for representing solutions as vectors of permutations
	//
	int **tau;    // vectors of permutations of jobs' tours represented as arrays
	int **pi;     // vectors of permutations of operations on machines represented as arrays
				  //----- Important note: The operations listed in tau and pi
				  //      are not zero-based, i.e. start at tau[j][1] and pi[m][1].
				  //      This is done to simplify the references to the
				  //      postions of the operations in both permutations,
				  //      which complies with the notations in the paper.
				  //      Meanwhile tau[j][0] = | tau_j |
				  //      and       pi[m][0] = | pi_m |
				  //

	int *jb;      // jb[i] = index of the job to which operation i belongs
	int *mc;      // mc[i] = index of the machine assigned to operation i
	int *tau_pos; // tau_pos[i] = position of operation i in tau
	int *pi_pos;  // pi_pos[i] = position of operation i in pi

	// variables used for the network representaion of the solution
	//
	MOSP_SSPR_Node *Nodes;    // for each operation, there is a node in the network
	int *FirstjNode;          // index of the first operation in the job tour
	int *LastjNode;           // index of the last operation in the job tour
	int *FirstmNode;          // index of the first node in the machine sequence
	int *LastmNode;           // index of the last operation in the machine sequence

	// Variables used within the network interpretation function
	int *MCrt;  // machine's ready time
	int *mnxt;  // next node index for the machine
	int *Jnft;  // job's nearest finish time
	int *jnxt;  // next node index for the job

	// Variables used within the task list conversion function to vectors
	int *jcur;  // current operation index of job
	int *mcur;  // current operation index of machine

	int *etgmx_1;
	int *etgmx_2;

	int Cmax;

	void InterpretTaskList();      // Interpret tasks list provided in sspr->Solution
                                   // into vectors of tau and pi

	void GenerateNetwork();		   // interprets tau and pi into network representation

	bool InterpretNetwork();       // Evaluate the earliest and latest start times
								   // for all operations based on a given complete
								   // network representation including all links
								   // and weights definitions.
								   //   Returns the status of the network
								   // representation if it is feasible or not.

	bool ConvertNetToTasks();      // Convert the current netowrk representation
								   // into tasks stored in sspr->Solution
								   // returns false for infeasible networks


  public:
	MOSP_SSPR_Solution(MOSP_SSPR *alg);
	~MOSP_SSPR_Solution();

    void AllocateMemory(MOSP_SSPR *alg);
	void FreeMemory();

	void Reset();
	void Copy(MOSP_SSPR_Solution *S);
	MOSP_SSPR_Solution &operator = (MOSP_SSPR_Solution &S){ Copy(&S);  return (*this); }

	bool InterpretMOSPSolution(MOSP_Solution *sol);  // Reads the task lists and interpret
													 // tasks into vectors only
													 // A MOSP algorithm needs to be intialized
													 // and its memory allocated for correct indexes

    bool isOperInAllCrtclPaths(int v);
	bool doesN1Improve(int v1);  // Check for conditions in Propositions 2 and 4
	bool doesN2Improve(int v2);  // Check for conditions in Propositions 3 and 4

	void alpha_omega_N1(int v1,	int &apr_alpha, int &apr_omega);
	int phi_N1(int v1);
	void Deltas_N1(int v1, int x1, int &hat_delta, int &check_delta);

	void alpha_omega_N2(int v2, int mk2, int &apr_alpha, int &apr_omega);
	int phi_N2(int v2, int mk2);
	void Deltas_N2(int v2, int mk2, int x2, int &hat_delta, int &check_delta);

	void alpha_omega_B1(int j, int x1, int x2, int &apr_alpha, int &apr_omega);
	void alpha_omega_B2(int m1, int m2, int x1, int x2, int &apr_alpha, int &apr_omega);

	void N1Results(int v1, int &alpha, int &omega, int *&apr_Cmax, int &minX);
		 //--- apr_Cmax must be array of int with size of nWCs+1
	void N2Results(int v2, int k2, int &alpha, int &omega, int *&apr_Cmax, int &minX);
		 //--- apr_Cmax must be array of int with size of nJobs+1

	int CmaxValue() { return Cmax; }

		   #ifdef PRNT_SSPR_DBG
			  void AssertVectors(char *heading);
			  void AssertNetwork(char *heading);
			  void AssertTasks(char *heading);
		   #endif
};

struct MOSP_SubPerumutation {
  int x1;  // starting position in the first permutation
  int x2;  // starting position in the second permutation
  int l;   // length of the subpermutation
};

class MOSP_SSPR : public MOSP_Algorithm {
  friend class TfrmSSPR;
  friend class TfrmSSPRMultiRuns;

  public:
	// Data structures for moves definitions
	enum NSFunction {
	  N1 = 1,
	  N2
	};

	struct NSMove {
	  NSFunction F;
	  int v;   // operation index
	  int x;   // move to position
	  int m2;  // move to machine for N2 only
	  int aprCmax;  // the value of phi + delta(x)
	};

  private:
	int LB;  // Lower bound for the given problem instance
	int pSize;  // population size
	int deltaThreshold;  // Threshold distance value for accepting new solutions
						 // in the reference set of solutions

	SList R; // reference set of solutions
	SList U; // set of generated solutions in each iteration
	int sspritr;

	int Epsilon;

	bool bTerminate;  // = true for intermediate terminations

  protected:
	int bestCmax;     // Best found makespan updated throughout iterations
	MOSP_SSPR_Solution *bestSol;   // Best found solution throughout iterations
    bool bOptSolFound;  // = true if a solution with Cmax = LB is found

	int nBestMoves;   // Maximum limit on the number of moves to be stored for improvement

	// The following variables are used for calculating distances ---------------------
	int mxsz;  // maximum size for the precedence relationship matrix
	int nr;    // number of subpermutations in the intersection set of two permutations
	MOSP_SubPerumutation *r;  // list of all subpermutations (intersection set)
	char **a;  // precedence relationship matrix to be used internally
			   // in calculating distance between two solutions
	int *Suma_i, *Suma_j; // summations in "a" matrix along the row and along the column
	//---------------------------------------------------------------------------------

	// The following variables are used for constructing new solutions using random DSG
	int *MCrt;          // Machine ready time
	bool *bMCready;     // = true if a machine is ready to process a job at a current time
	bool *bMCdone;      // = true if a machine is done with all its required jobs

	int *Jnft;          // Job's nearest finish time
	bool *bJcompleted;  // = true if all operations of a job are scheduled

	bool **bJM;              // = true if a job needs a machine
	bool *SchedulableJobs;   // = true if a job is ready to be processed on a machine
	bool *bScheduled;        // = true if an operations is scheduled and false otherwise
	int *SelJob;             // selected job for each machine
	int *SumSchedTime;       // summation of scheduled times on a machine
	//---------------------------------------------------------------------------

	// The following variables are used for the implementation of Tabu Search (TS)
	int TabuSize;        // Allowed maximum tabu size
	int nTSiterations;   // maximum number of tabu search iterations allowed
	int nImpIterations;  // maximum number of improvement iterations allowed
	int nRandMoves;      // number of random moves to be conducted for diversification
	int nTabus;          // current number of tabus in the list
	int TabuPntr;        // a pointer to the current position in the circular tabu list
	NSMove *TabuMoves;
	MOSP_SSPR_Solution *bestLocalSol;   // Best found solution throughout iterations
	int initialLocalCmax;  // Initial makespan passed to the tabu search
	int bestLocalCmax;   // The best makespan value for the current solution being improved by TS
	int *apr_Cmax;   // Stores approximate Cmax values for neighborhood moves
	//---------------------------------------------------------------------------

	// The following variables are used for the implementation of Path Relinking (PR)
	int PRSolCounter;
	int *jbDist, *wcDist;
	int TotaljbDist, TotalwcDist;
	bool *bUseTau, *bUseVPi;
	int *tmpTau, *tmpPi;
	//---------------------------------------------------------------------------

	int nIterations;      // Maximum number of improvement iterations allowed

  public:
	MOSP_SSPR(MOSP_Problem *P, int ps=20, int dth=-1, int ts=20, int ntsitr=500,
							   int ntsimpitr=150, int nbstmvs=5, int nrndmvs=10,
							   int nitr=100, int Epsl=2);
	~MOSP_SSPR();

	int nMoves;          // Actual number of best moves stored
	NSMove *bestMoves;   // a list of best moves in an iteration

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	void CalculateLowerBound();
	int CalculateLowerBound(int &lb1, int &lb2, int &lb3);
	int SuggestDeltaThreshold(double DivFactor = 3.0);
	int ConstructInitialPopulation();  // returns the number of solutions constructed
	MOSP_SSPR_Solution *DSG(int &Makespan, int MSR=3);  // dense schedule generator. Returns new solution
	MOSP_SSPR_Solution *ConstructSolution();

	void EnumerateMoves(MOSP_SSPR_Solution *Sol, bool bStrictImp=false);
	                                               // enumerate all possible
												   // moves and store the best
												   // ones in bestMoves

	int ApplyN(MOSP_SSPR_Solution *Sol, NSMove mv);  // returns resultant Cmax

	int TauIntersectionSize(int j, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2);
	int TauDistance(int j, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2);
	int WCIntersectionSize(int w, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2);
	int WCDistance(int w, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2);
	int Distance(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2, int &jD, int &wcD);
												   // Dissimilarity measure
	bool isDistanceGreaterThan(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2, int DistLimit);

	// Functions used by Tabu Search
	int SuggestTabuSize();
	void TabuSearch(MOSP_SSPR_Solution *Sol);
	void AddTabu(NSFunction F, int v, int x, int m=-1);
	void ClearTabus();
	bool isTabu(NSFunction F, int v, int x, int m=-1);
	void EnumerateNonTabuMoves(MOSP_SSPR_Solution *Sol, bool bStrictImp=false);
	int ApplyNandUpdateTabuList(MOSP_SSPR_Solution *Sol, NSMove mv);  // returns resultant Cmax

	// Functions used by Path Relinking
	int DetailedDistances(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2);
												   // Detailed distances used for path relinking
	MOSP_SSPR_Solution *PathRelinking(MOSP_SSPR_Solution *Si, MOSP_SSPR_Solution *Sg);
	void BestTauSubpermutationMove(MOSP_SSPR_Solution *S2, MOSP_SSPR_Solution *S1,
								   int j, int &x1, int &x2, int &x);
									// returns the feasible subpermutation move
									// that results in the largest distance reduction
	void ApplyB1(MOSP_SSPR_Solution *S, int j, int x1, int x2, int x);
	void BestVPiSubpermutationMove(MOSP_SSPR_Solution *S2, MOSP_SSPR_Solution *S1,
	                               int sw, int &x1, int &x2, int &x, int &fromMC, int &toMC);
	void ApplyB2(MOSP_SSPR_Solution *S, int x1, int x2, int x, int fromMC, int toMC);


		   #ifdef DEBUG_SSPR_RESULTS
			  MOSP_SSPR_Solution *seedSol;
			  int nOpsByMCinSeedSol(int mc);
			  bool SetSeedSolution(MOSP_Solution *Sol);
			  void EnumerateMoves(bool bStrictImp=false);  // enumerate moves for seedSol;
			  int ApplyN(int mv);  // apply the NS function bestMoves[mv] to seedSol;
			  int ApplyNandUpdateTabuList(int mv);  // apply the NS function bestMoves[mv]
													// to seedSol and update tabu list;

			  #ifdef PRNT_NSCB_DBG
				void GenerateNSParameters(MOSP_Problem::SOperation *op);
				void GenerateB1Parameters(MOSP_Problem::SJob *jb, int x1, int x2);
				void GenerateB2Parameters(MOSP_Problem::SMC *mc, int x1, int x2);
			  #endif

              void SetTabuSize(int ts);
		   #endif
};

struct TaguchiExperiment {
  double A;
  int B, C, D, E, F;
};

extern TaguchiExperiment Taguchi[];

#endif
