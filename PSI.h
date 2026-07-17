//---------------------------------------------------------------------------

#ifndef PSIH
#define PSIH

#include "MOSPSolver.h"

//------------- Definitions of classes used to manage processing sequences
//              of operations on machines
//

struct MOSP_PS {  // a class for managing processing sequence on a given machine
  MOSP_Problem::SMC *MC;
  SList PS;  // a processing sequence of operations

  MOSP_PS(MOSP_Problem::SMC *mc) : MC(mc) {};

  bool AppendOp(MOSP_Problem::SOperation *op);
  bool InsertOp(MOSP_Problem::SOperation *newop, MOSP_Problem::SOperation *op);
  bool InsertOp(MOSP_Problem::SOperation *newop, int pos);
  bool Swap(MOSP_Problem::SOperation *op1, MOSP_Problem::SOperation *op2);
  bool Swap(int pos1, int pos2);
};

struct MOSP_VPS {  // a class for managing vectors of processing sequences on machines
  MOSP_Problem *Problem;
  SList PSs;

  MOSP_VPS(MOSP_Problem *P);
  MOSP_VPS(MOSP_VPS *vps);
  ~MOSP_VPS();

  bool AppendOp(MOSP_Problem::SMC *mc, MOSP_Problem::SOperation *op);
  bool isIncluded(MOSP_Problem::SOperation *op);
  bool Copy(MOSP_VPS *vps);

  bool operator ==(MOSP_VPS &vps);
  bool operator !=(MOSP_VPS &vps);
  MOSP_VPS &operator =(MOSP_VPS &vps);
  bool isComplete();   // Means all operations are included

  MOSP_Problem::SMC *MCAssignedToOP(MOSP_Problem::SOperation *op);
  MOSP_PS *PSContainingOP(MOSP_Problem::SOperation *op);

  MOSP_PS *PS(MOSP_Problem::SMC *mc);
};

//-------------- Definition of classes used to manage operations' orders within
//               jobs.
//

struct MOSP_OO {   // operations' order within a job
  MOSP_Problem::SJob *job;
  SList OO;    // Operation's order

  MOSP_OO(MOSP_Problem::SJob *j) : job(j) {};

  bool AppendOp(MOSP_Problem::SOperation *op);
  bool InsertOp(MOSP_Problem::SOperation *newop, MOSP_Problem::SOperation *op);
  bool InsertOp(MOSP_Problem::SOperation *newop, int pos);
  bool Swap(MOSP_Problem::SOperation *op1, MOSP_Problem::SOperation *op2);
  bool Swap(int pos1, int pos2);
};

struct MOSP_VOO {  // a class for managing vectors of operations orders within jobs
  MOSP_Problem *Problem;
  SList OOs;

  MOSP_VOO(MOSP_Problem *P);
  MOSP_VOO(MOSP_VOO *voo);
  ~MOSP_VOO();

  bool AppendOp(MOSP_Problem::SOperation *op);
  bool isIncluded(MOSP_Problem::SOperation *op);
  bool Copy(MOSP_VOO *voo);

  bool operator ==(MOSP_VOO &voo);
  bool operator !=(MOSP_VOO &voo);
  MOSP_VOO &operator =(MOSP_VOO &voo);
  bool isComplete();   // Means all operations are included

  MOSP_OO *OO(MOSP_Problem::SJob *jb);
};

//-------------- This is a simple algorithm that interprets a given vector of
//               processing sequences and operations' orders into semi-active
//               schedule
//

class MOSP_PSI : public MOSP_Algorithm {
  protected:
	MOSP_PSI(MOSP_Problem *P, MOSP_VPS *vps, MOSP_VOO *voo, char *N);

	int *nxt;   // Next schedulable operation index

	float *mcrt,  // an array for the machine ready times
		  *jbrt;  // an array for the jobs release times which represent the
				  // minimum time at which the next unscheduled operation can start

  public:
	MOSP_VPS VPS;  // vector of processing sequences on machines
	MOSP_VOO VOO;  // vector of operations' orders within jobs
	MOSP_VPS *tmpVPS;
	MOSP_VOO *tmpVOO;

	MOSP_PSI(MOSP_Problem *P, MOSP_VPS *vps, MOSP_VOO *voo);
	~MOSP_PSI();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//---------------------------------------------------------------------------
#endif
