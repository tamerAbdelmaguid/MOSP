//--- This code is Copyright 2014, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//---------------------------------------------------------------------------

#ifndef MOSPSolverH
#define MOSPSolverH

#include "Algorithm.h"
#include "Problem.h"

float fmax(float a, float b);
float fmin(float a, float b);

class MOSP_Solution {
  public:
    struct SchTask {
	  MOSP_Problem::SOperation *Operation;
	  MOSP_Problem::SMC *SelectedMC;        // = NULL if not selected
	  bool bScheduled;    // Needed by the construction algorithm
	  float StartTime;
	  float EndTime;
	  SchTask(MOSP_Problem::SOperation *op, MOSP_Problem::SMC *mc, float s, float e)
			: Operation(op), SelectedMC(mc), StartTime(s), EndTime(e), bScheduled(true)	{};
	  SchTask(MOSP_Problem::SOperation *op)
				: Operation(op), SelectedMC(NULL), bScheduled(false)
				{ StartTime = EndTime = MAXFLOAT; };
    };

  public:
    MOSP_Problem *Problem;
    SList ScheduledTasks;

    MOSP_Solution(MOSP_Problem *P, bool bConstructTasks = true);
	~MOSP_Solution();

	float JobCompletionTime(MOSP_Problem::SJob *jb);

	SchTask *TaskAssociatedToOperation(MOSP_Problem::SOperation *op);
	float ComputeObjectiveValue(MOSP_Problem::TObjective OBJ=(MOSP_Problem::TObjective)0);

	bool Copy(MOSP_Solution *Sol);
	MOSP_Solution& operator=(MOSP_Solution &sol);
	bool operator ==(MOSP_Solution &Sol);

	bool isComplete();
	bool isFeasible();
};

class MOSP_Algorithm : public Algorithm {
  protected:
	MOSP_Algorithm(MOSP_Problem *P, char *N);

	// Data structures that are used to include all problem data from an easily
	//  accessible memory locations to subdue the effect of memory access time
	//  on the efficiency of the algorithms.
	//
  public:
	int nWCs, nMCs, nJobs, nOps;    // number of...
	MOSP_Problem::SWC **W;          // set of all workcenters
	MOSP_Problem::SMC **M;          // set of all machines
	int *Wa, *Wz;                   // set of indexes for workcenters to
									//   identify the starting and ending indexes
									//   of the machines that belong to a WC
	int *Mw;                        // workcenter index for a machine

	MOSP_Problem::SJob **J;         // set of all jobs
	MOSP_Problem::SOperation **O;   // set of all operations
	int *Ja, *Jz;                   // set of indexes for jobs to
									//   identify the starting and ending indexes
									//   of the operations that belong to a job
	int *Oj;                        // job index for an operation

	int **pt;                       // processing time matrix of a job on a machine
									// Note: the processing time is reduced here as
									//       integer for speed purposes. If this is
									//       not possible, then it should be changed
									//       back to float.

	MOSP_Solution *Solution;        // Solution of the problem in tasks representation
	MOSP_Solution::SchTask **tsk;   // set of all task pointers in Solution

	enum TSolutionStatus { INFEASIBLE, FEASIBLE } SolStatus;

	MOSP_Algorithm(MOSP_Problem *P);
	~MOSP_Algorithm();

	void AllocateArrays();
	void FreeArrays();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//------------------- An implementation of the dense schedule (DS) generator
//

int tskComp(void * Item1, void * Item2);

class MOSP_DSG : public MOSP_Algorithm {
  protected:
	MOSP_DSG(MOSP_Problem *P, char *N);

  public:
	enum T_JobDispatchingRule {
		   FCFS=0,   // First Come First Serve
		   SPT,      // Shortest Processing Time
		   LPT,      // Longest Processing Time
		   MCT,      // Minimum Completion Time
		   MOR,      // Most Operations Remaining
		   LOR,      // Least Operations Remaining
		   MWR,      // Most Work Remaining
		   LWR,      // Least Work Remaining
		   RANDOM
	} JobDispatchingRule;

	enum T_MCSelectionRule {
		   MIN_LOAD=0,  // Minimum load
		   MINCT,       // Minimum completion time
		   MCRANDOM
	} MCSelectionRule;

	MOSP_DSG(MOSP_Problem *P, T_JobDispatchingRule DR, T_MCSelectionRule SR);
	~MOSP_DSG();

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

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//------------- Network representation of solutions
//

class MOSP_VectorSol {
	friend class MOSP_NSTester;

  private:
	MOSP_Algorithm *Alg;

	SList *tau;    // vectors of permutations of jobs' tours
	SList *pi;    // vectors of permutations of operations on machines

	int *mc;       // mc[i] = index of the machine assigned to operation i

  public:
	MOSP_VectorSol(MOSP_Algorithm *a);
	MOSP_VectorSol(MOSP_VectorSol *V);
	~MOSP_VectorSol();

	void Copy(MOSP_VectorSol *V);

	char *u;       // inclusion status in job and machine permutations to be used
				   //   within the network construction and properties calculations
				   // u[i] = 0 if operation i is not included in neither machine nor job permutations
				   //      = 1 if operation i is included in job permutation but not in machine permutation
				   //      = 2 if operation i is not included in job permutaion but included in machien permutation
				   //      = 3 if operation i is included in both job and machine permutations

	void Assert(char *heading);    // make output to the debug file to show the vector's content

	void ConvertTasksToVect();    // Convert current solution (Alg->Solution)
								  // into vector representation 

	bool AppendOp2Mc(int m, MOSP_Problem::SOperation *op);
	bool AppendOp2Mc(MOSP_Problem::SMC *m, MOSP_Problem::SOperation *op);
	bool AppendOp2Jb(MOSP_Problem::SOperation *op);
	bool RemoveOp(MOSP_Problem::SOperation *op);

	int nInTau(int j){ if(j<0 || j>=Alg->nJobs) return -1;  return tau[j].Count(); }
	int nInPi(int m){ if(m<0 || m>=Alg->nMCs) return -1;  return pi[m].Count(); }

	SList::SNode *headOfTau(int j){ if(j<0 || j>=Alg->nJobs) return NULL;   return tau[j].head(); }
	SList::SNode *headOfPi(int m){ if(m<0 || m>=Alg->nMCs) return NULL;  return pi[m].head(); }

	void Clear();        // clear all vectors

	bool isComplete();   // check if all operations are included
	bool isFeasible();   // check if there is any conflict. The solution does not have to be complete!

	int Psi_j(MOSP_VectorSol *V); // dissimilarity measure between this vector and
								  // another vector V for taus only
	int Psi_w(MOSP_VectorSol *V); // dissimilarity measure for pis

	bool RemoveTauSubpermutation(int j, int x1, int x2);
	bool RemovePiSubpermutation(int m, int x1, int x2);

	void RemoveTauOperation(MOSP_Problem::SOperation *op){ u[op->i] &= 2; };
	void RemovePiOperation(MOSP_Problem::SOperation *op){ u[op->i] &= 1; };

	void UnremoveAll();

	bool MoveOpToTauPos(MOSP_Problem::SOperation *op, int x, bool bchkpos=true);
	bool MoveOpToPiPos(MOSP_Problem::SOperation *op, int k, int x, bool bchkpos=true);

	bool MoveTauSubpermutation(int j, int x1, int x2, int x, bool bchkpos=true);
	bool MovePiSubpermutation(int from_m, int x1, int x2, int to_m, int x, bool bchkpos=true);

	int TauPosition(MOSP_Problem::SOperation *op);
	int PiPosition(MOSP_Problem::SOperation *op);

	MOSP_Problem::SMC *MCofOp(MOSP_Problem::SOperation *op);
};

struct MOSP_Node {  // used to represent nodes in the network representations of solutions
  int mc;     // index of the selected machine, = -1 if no machine is assigned

  int m_prv;  // index of the previous operation in machine sequence
  int m_nxt;  // index of the next operation in machine sequence
  int j_prv;  // index of the previous operation in the job route
  int j_nxt;  // index of the next operation in the job route
			  // ---- if a nxt/prv node does not exist, the values are set to -1

  int weight;  // the processing time of the corresponding operation

  int estart;    // [eta] earliest start time of an operation
				 // ---> = -1 if the operation is not included in the job tour and machine sequence
  int lstart;    // [lambda] latest start time of an operation
  int tail;      // [gamma] tail of an operation, which is the remaining time to the completion of the schedule
  int slack;     // slack of an operation = lstart - estart
  bool bCritical;   // = true when estart = lstart

  int rank;      // number of all preceding nodes

  void Reset(){  // default node parameters
	mc = -1;     // means no machine is assigned
	m_prv = m_nxt = j_prv = j_nxt = -1;  // no connections with other nodes
	weight = 0;
	estart = lstart = tail = slack = -1;
	bCritical = false;
	rank = 0;
  };
};

class MOSP_Network {
	friend class MOSP_NSTester;

  private:
	MOSP_Algorithm *Alg;         // a pointer to the tabu search algorithm for problem
								 // and algorithm parameters

	// Variables used within the network interpretation function
	int *MCrt;  // machine's ready time
	int *mnxt;  // next node index for the machine
	int *Jnft;  // job's nearest finish time
	int *jnxt;  // next node index for the job

	// Variables used within the task conversion function to network
	int *jcur;  // current operation index of job
	int *mcur;  // current operation index of machine

  public:
	MOSP_Node *Nodes;    // for each operation, there is a node in the network
	int *FirstjNode;     // index of the first operation in the job tour
	int *LastjNode;      // index of the last operation in the job tour
	int *FirstmNode;     // index of the first node in the machine sequence
	int *LastmNode;      // index of the last operation in the machine sequence

	int Cmax;
	int LB;              // approximate lower bound for an interchange of nodes

	MOSP_Network(MOSP_Algorithm *a);
	MOSP_Network(MOSP_Network *nt);
	~MOSP_Network();

	void Assert(char *);  // make output into dbgFile to show the network elements

	void Copy(MOSP_Network *nt);

	int GetCPath(int *pth);   // Find an arbitrary critical path and put the node
							  // indexes in that path in "pth" and return its length


	void ConvertTasksToNet();      // Convert current solution (Alg->Solution)
								   // into network representation

	void InterpretNetwork(bool &bFeasible, bool &bComplete);
								   // Evaluate the earliest and latest start times
								   // for all operations based on a given complete
								   // network representation including all links
								   // and weights definitions.
								   //   Returns the status of the network
								   // representation if it is feasible or not
								   // and if it represents a complete solution or not

	bool ConvertNetToTasks();      // Convert the current netowrk representation
								   // into tasks stored in Alg->Solution
								   // returns false for infeasible networks

	bool InterpretVectors(MOSP_VectorSol *V, bool bckf=true);
								   // interprets job orders and processing
								   // sequences into network representation
								   // return false if infeasible

	bool InterpretIncompleteNetwork();   // all nodes are included by some arcs are not
	bool ConvertIncompleteNetToTasks();

	void Reset();

	int sizeof_tau(int j);
	int sizeof_pi(int m);

	MOSP_Node *tauNode_at(int j, int x);
	MOSP_Node *piNode_at(int m, int x);

	int tau_pos(MOSP_Node *nd);
	int pi_pos(MOSP_Node *nd);

	bool isThereAPath(MOSP_Node *nd1, MOSP_Node *nd2);

	// The following functions work with complete networks of feasible solutions
	//     only
	//
	void alpha_omega_N1(MOSP_Problem::SOperation *v1,
							int &alpha, int &omega, int &apr_alpha, int &apr_omega);
	int phi_N1(MOSP_Problem::SOperation *v1);
	void ApproxDeltas_N1(MOSP_Problem::SOperation *v1, int x1,
							int &hat_delta, int &check_delta);
	void EstDeltas_N1(MOSP_Problem::SOperation *v1, int x1,
							int &hat_delta, int &check_delta);
	void ExactDeltas_N1(MOSP_Network *Net_, MOSP_Problem::SOperation *v1, int x1,
							int &hat_delta, int &check_delta);

	void alpha_omega_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2,
							int &alpha, int &omega, int &apr_alpha, int &apr_omega);
	int phi_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2);
	void ApproxDeltas_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2, int x2,
							int &hat_delta, int &check_delta);
	void EstDeltas_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2, int x2,
							int &hat_delta, int &check_delta);
	void ExactDeltas_N2(MOSP_Network *Net_, MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2,
							int x2, int &hat_delta, int &check_delta);

	void alpha_omega_B1(int j, int x1, int x2, int &alpha, int &omega,
						int &apr_alpha, int &apr_omega);
	void alpha_omega_B2(int m1, int m2, int x1, int x2, int &alpha,
	                    int &omega, int &apr_alpha, int &apr_omega);
};

// An algorithm used for demonstrating neighborhood search moves to improve
// a given initial solution
//

class MOSP_NSTester : public MOSP_Algorithm {
  protected:
	MOSP_Solution *iSol;    // pointer to initial solution passed to the algorithm

	MOSP_NSTester(MOSP_Problem *P, char *N);

  public:
	MOSP_NSTester(MOSP_Problem *P, MOSP_Solution *is);
	~MOSP_NSTester();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	bool GetN1Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int j, int x,
					SList &N1E, SList &N1L, SList &N1S, int &_x, int &x_);
		// Finds sets E and L of operation in position x for function N1

	bool GetN2Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int m, int x,
					SList &N2E, SList &N2L, SList &N2S, int &_x, int &x_);
		// Finds sets E and L of operation in position x for function N2

	bool GetN3Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int from_m, int to_m, int x,
					SList &N3E, SList &N3L, SList &N3S, int &_x, int &x_);
		// Finds sets E and L of operation in position x for function N3
};

//---------------------------------------------------------------------------
#endif
