//--- This code is Copyright 2014, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef SCHProblemH
#define SCHProblemH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "values.h"
#include "SList.h"
#include "STree.h"

//---- Simple definition class used as the parent for the other classes
class SCH_Entity {
  char _ID[9];
  char _Description[65];

public:
  int i;  // an index for the entity to be used by algorithms

  SCH_Entity(char *ID, char *Des){
	strncpy(_ID, ID, 8);             _ID[8] = 0;
	strncpy(_Description, Des, 64);  _Description[64] = 0;
  }

  char *ID(void) { return _ID; }
  char *Description(void) { return _Description; }
};

//----------------------------------------- The problem definition class
class MOSP_Problem : SCH_Entity {
public:
  class SWC : public SCH_Entity {    // A class for workcenters
	public:
	  SList MCs;

	  SWC(char *ID, char *Des) :  SCH_Entity (ID, Des) {};
	  ~SWC();
  };

  class SMC : public SCH_Entity {    // A class for machines
	public:
	  float ReadyTime;   // Machine ready time
	  SWC *WC;           // A pointer to the worcenter to which it belongs

	  SMC(char *ID, char *Des, SWC *wc, float rt)
		  : SCH_Entity (ID, Des), WC(wc), ReadyTime(rt) {};
  };

  class SJob : public SCH_Entity {   // A class for jobs
	public:
	  float ReleaseTime, DueDate, Weight;
	  SList Operations;

	  SJob(char *ID, char *Des, float rt=0, float dd=MAXFLOAT, float w=1) :
			SCH_Entity(ID, Des), ReleaseTime(rt), DueDate(dd), Weight (w) {};
	  ~SJob();

	  bool bUsedWC(SWC *wc);    // true if wc is used by the job
  };

  struct SOpMC {  // A structure for defining the operation-machine relationship
	SMC *MC;     // machine
	float pt;    // Corresponding processing time

	SOpMC(SMC *mc, float t=0) : MC(mc), pt(t) {};
  };

  class SOperation : public SCH_Entity {  // A class for operations
	public:
	  SJob *Job;     // Its job
	  SWC *WC;       // Its workcenter
	  SList AltMCs;  // List of alternative machines belonging to WC
					 // with the SOpMC structure

	  SOperation(char *ID, char *Des, SJob *jb, SWC *wc, float *t=NULL);
	  SOperation(char *ID, char *Des, SJob *jb, SWC *wc, float t);  // special constructor for Matta's problems
	  ~SOperation();

	  float pt(SMC *mc);   // returns processing time on machine mc
	  float minpt();       // returns the minimum processing time for all machines of the WC
	  float maxpt();       // returns the maximum processing time for all machines of the WC
  };

  enum TObjective {
	MIN_MAKESPAN=1,
	MIN_MEAN_FLOWTIME,
	MIN_MEAN_LATENESS,
	MIN_MEAN_TARDINESS,
	MIN_MAX_LATENESS,
	MIN_MAX_TARDINESS,
	MIN_N_TARDY,
	MIN_MEAN_W_LATENESS,
	MIN_MEAN_W_TARDINESS,
	MIN_MEAN_W_FLOWTIME
  };

  // Original problem data - lists of the SCH classes
  TObjective Objective;   // Objective function
  SList WCs;              // List of workcenters
  SList Jobs;             // List of jobs

  MOSP_Problem(char *ID, char *Des, TObjective O=MIN_MAKESPAN);
  MOSP_Problem(char *ID, char *Des, int nWCs, int nJobs, int *nMCs, int *pt); // Matta's PMOSP instances
  MOSP_Problem(char *ID, char *Des, int nWCs, int nJobs, int qwLevel, int LL, int PTVL);  // Specially designed MOSP instances
  ~MOSP_Problem();

  SWC *AddWC(char *ID, char *Des);
  SWC *InsertWC(SWC *curWC, char *ID, char *Des);
  SWC *InsertWC(int icurWC, char *ID, char *Des);
  SMC *AddMC(char *ID, char *Des, SWC *wc, float rt);
  SMC *InsertMC(SMC *curMC, char *ID, char *Des, SWC *wc, float rt);
  SMC *InsertMC(int icurMC, char *ID, char *Des, SWC *wc, float rt);
  SJob *AddJob(char *ID, char *Des, float rt=0, float dd=0, float w=1);
  SJob *InsertJob(SJob *curJob, char *ID, char *Des, float rt=0, float dd=0, float w=1);
  SJob *InsertJob(int icurJob, char *ID, char *Des, float rt=0, float dd=0, float w=1);
  SOperation *AddOperation(char *ID, char *Des, SJob *j, SWC *p, float *t);
  SOperation *InsertOperation(SOperation *curOp, char *ID, char *Des, SJob *j, SWC *p, float *t);
  SOperation *InsertOperation(int icurOp, char *ID, char *Des, SJob *j, SWC *p, float *t);

  bool RemoveWC(SWC *wc);
  bool RemoveMC(SMC *mc);
  bool RemoveJob(SJob *jb);
  bool RemoveOperation(SOperation *op);

  bool ModifyWC(SWC *wc, char *id, char *des);
  bool ModifyMC(SMC *mc, char *id, char *des, float rt);
  bool ModifyJob(SJob *jb, char *id, char *des, float rt, float dd, float w, float l=1);
  bool ModifyOperation(SOperation *op, char *id, char *des, SWC* wc, float *t);

  bool UsedIDinWCs(char *id, SWC *except=NULL);
  bool UsedIDinMCs(char *id, SMC *except=NULL);
  bool UsedIDinJOs(char *id, SJob *except=NULL);
  bool UsedIDinOps(char *id, SOperation *except=NULL);
  bool UsedID(char *id);

  SWC *WCbyID(char *id);
  SMC *MCbyID(char *id);
  SJob *JObyID(char *id);
  SOperation *OpByID(char *id);

  int nOperations();
  int nOperationsByWC(SWC *wc);
  int nMachines();

  bool isProportionate();
};

#endif
