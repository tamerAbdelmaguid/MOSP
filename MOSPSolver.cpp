//--- This code is Copyright 2017, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//#define REPORT_NETWORK_RESULTS
//#define REPORT_VECTOR_RESULTS

#ifdef REPORT_NETWORK_RESULTS
  #define REPORT_INTERMEDIATE_RESULTS
#endif

#ifdef REPORT_VECTOR_RESULTS
  #define REPORT_INTERMEDIATE_RESULTS
#endif

//#define REPORT_NSTester_RESULTS

#include "MOSPSolver.h"
#include "random.h"
#include "SList.h"
#include "STree.h"

#ifdef REPORT_INTERMEDIATE_RESULTS
FILE *dbgFile;
#endif

float fmax(float a, float b)
{
  if(a > b) return a;
  return b;
}

float fmin(float a, float b)
{
  if(a < b) return a;
  return b;
}
//----------------------------------------------- Solution Class
MOSP_Solution::MOSP_Solution(MOSP_Problem *P, bool bConstructTasks) : Problem(P)
{
  // Since scheduled tasks are well defined as related to operations
  // we will allocate necessary memory space for all of them here unless
  // bConstructTasks == false
  //
  if(bConstructTasks){
    SList::SNode *jnd = Problem->Jobs.head();
    while(jnd){
      MOSP_Problem::SJob *job = (MOSP_Problem::SJob *)jnd->Data();
      SList::SNode *opnd = job->Operations.head();
      while(opnd){
		MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
        MOSP_Solution::SchTask *newTsk = new MOSP_Solution::SchTask(op);
        ScheduledTasks.Add(newTsk);
        opnd = opnd->Next();
      }
      jnd = jnd->Next();
    }
  }
}

MOSP_Solution::~MOSP_Solution()
{
  // free allocated memory for scheduled tasks
  //
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
    SchTask *tsk = (SchTask *)nd->Data();
    delete tsk;
    nd = nd->Next();
  }
  ScheduledTasks.Clear();
}

float MOSP_Solution::JobCompletionTime(MOSP_Problem::SJob *jb)
{
  SList::SNode *tsknd = ScheduledTasks.head();

  float ct = -1;
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(!tsk->bScheduled){
	  ct = -MAXFLOAT;
	  break;
	}
	if(tsk->Operation->Job == jb){
	  if(ct < tsk->EndTime) ct = tsk->EndTime;
	}
	tsknd = tsknd->Next();
  }
  return ct;
}

MOSP_Solution::SchTask *MOSP_Solution::TaskAssociatedToOperation(
												  MOSP_Problem::SOperation *op)
{
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
    SchTask *tsk = (SchTask *)nd->Data();
    if(tsk->Operation == op) return tsk;
    nd = nd->Next();
  }
  return NULL;
}

float MOSP_Solution::ComputeObjectiveValue(MOSP_Problem::TObjective OBJ)
{
  // Calculate the objective function value for a given solution "Sol"
  //   --> the return value is -MAXFLOAT if the schedule is not complete
  //
  SList::SNode *tsknd, *jnd;
  float obj = 0;
  MOSP_Problem::TObjective OBJECTIVE = (OBJ == 0)? Problem->Objective : OBJ;
  switch(OBJECTIVE){
    case MOSP_Problem::MIN_MAKESPAN:
		   tsknd = ScheduledTasks.head();
           while(tsknd){
			 MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
             if(!tsk->bScheduled){
               obj = -MAXFLOAT;
               break;
             }
			 if(obj < tsk->EndTime) obj = tsk->EndTime;
             tsknd = tsknd->Next();
           }
		   break;

    case MOSP_Problem::MIN_MEAN_FLOWTIME:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (ct - jb->ReleaseTime);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case MOSP_Problem::MIN_MEAN_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (ct - jb->DueDate);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case MOSP_Problem::MIN_MEAN_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += fmax((ct - jb->DueDate), 0.0f);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case MOSP_Problem::MIN_MAX_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 float lat = ct - jb->DueDate;
			 if(obj < lat) obj = lat;
			 jnd = jnd->Next();
		   }
		   break;

	case MOSP_Problem::MIN_MAX_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 float tard = fmax((ct - jb->DueDate), 0.0f);
			 if(obj < tard) obj = tard;
			 jnd = jnd->Next();
		   }
		   break;

	case MOSP_Problem::MIN_N_TARDY:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 if(ct > jb->DueDate) obj++;
			 jnd = jnd->Next();
		   }
		   break;

	case MOSP_Problem::MIN_MEAN_W_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += ((ct - jb->DueDate) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

    case MOSP_Problem::MIN_MEAN_W_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (fmax((ct - jb->DueDate), 0.0f) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
           break;

    case MOSP_Problem::MIN_MEAN_W_FLOWTIME:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
			 float ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += ((ct - jb->ReleaseTime) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
           break;
  }
  return obj;
}

bool MOSP_Solution::Copy(MOSP_Solution *Sol)
{
  if(Problem != Sol->Problem) return false;

  SList::SNode *curnd = ScheduledTasks.head();
  SList::SNode *nd = Sol->ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	SchTask *curtsk = (SchTask *)curnd->Data();
	memcpy(curtsk, tsk, sizeof(SchTask));
	nd = nd->Next();
	curnd = curnd->Next();
  }
  return true;
}

MOSP_Solution& MOSP_Solution::operator=(MOSP_Solution &sol)
{
  Copy(&sol);
  return *this;
}

bool MOSP_Solution::operator ==(MOSP_Solution &Sol)
{
  if(Problem != Sol.Problem) return false;
  SList::SNode *nd = Sol.ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	SchTask *thisTsk = TaskAssociatedToOperation(tsk->Operation);
	if(thisTsk->bScheduled != tsk->bScheduled || thisTsk->SelectedMC != tsk->SelectedMC ||
	   thisTsk->StartTime != tsk->StartTime) return false;
	nd = nd->Next();
  }
  return true;
}

bool MOSP_Solution::isComplete()
{
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	if(!tsk->SelectedMC) return false;
	nd = nd->Next();
  }
  return true;
}

bool MOSP_Solution::isFeasible()
{
  // start by sorting all tasks in a non-decreasing order of their start times
  ScheduledTasks.QSort(tskComp, ASCENDING);

  // check for overlaps in task intervals on machines and jobs
  SList::SNode *cur = ScheduledTasks.head();
  while(cur){
	SchTask *tsk = (SchTask *)cur->Data();
	SList::SNode *nxt = cur->Next();
	while(nxt){
	  SchTask *nxttsk = (SchTask *)nxt->Data();
	  if(nxttsk->StartTime >= tsk->EndTime) break;
	  if(nxttsk->SelectedMC == tsk->SelectedMC ||
		 nxttsk->Operation->Job == tsk->Operation->Job) return false;
	  nxt = nxt->Next();
	}
	cur = cur->Next();
  }

  // check for any logical contradiction between processing sequences on machines
  // and processing orders of jobs (the case of cycles in the network representation)
  //
  bool bFeasible = true;
  int nUsedMCs = Problem->nMachines();
  int nIncludedJobs = Problem->Jobs.Count();
  int nMCs = nUsedMCs;
  int nJobs = nIncludedJobs;

  MOSP_Problem::SMC **M = new MOSP_Problem::SMC *[nMCs];
  SList::SNode *nd = Problem->WCs.head();
  int m=0;
  while(nd){
	MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  M[m] = mc;
	  m++;
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }

  int j=0;
  MOSP_Problem::SJob **J = new MOSP_Problem::SJob *[nJobs];
  nd = Problem->Jobs.head();
  while(nd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)nd->Data();
	J[j] = jb;
    jb->i = j;
	j++;
	nd = nd->Next();
  }

  SList::SNode **mnxt = new SList::SNode *[nMCs];    // next task node for the machine
  for(m=0; m<nMCs; m++){
	mnxt[m] = NULL;
	nd = ScheduledTasks.head();
	while(nd){
	  SchTask *tsk = (SchTask *)nd->Data();
	  if(tsk->SelectedMC == M[m]){ mnxt[m] = nd;  break; }
	  nd = nd->Next();
	}
	if(!mnxt[m]) nUsedMCs--;
  }

  SList::SNode **jnxt = new SList::SNode *[nJobs];   // next task node for the job
  for(j=0; j<nJobs; j++){
	jnxt[j] = NULL;
	nd = ScheduledTasks.head();
	while(nd){
	  SchTask *tsk = (SchTask *)nd->Data();
	  if(tsk->Operation->Job == J[j]){ jnxt[j] = nd;  break; }
	  nd = nd->Next();
	}
	if(!jnxt[j]) nIncludedJobs--;
  }

  // Check for the infeasibility now
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<nMCs; m++){
	  if(!mnxt[m]) continue;
	  SchTask *mtsk = (SchTask *)mnxt[m]->Data();
	  j = mtsk->Operation->Job->i;
	  SchTask *jtsk = (SchTask *)jnxt[j]->Data();
	  if(mtsk == jtsk){
		mnxt[m] = mnxt[m]->Next();
		while(mnxt[m]){
		  SchTask *tsk = (SchTask *)mnxt[m]->Data();
		  if(tsk->SelectedMC == M[m]) break;
		  mnxt[m] = mnxt[m]->Next();
		}
		jnxt[j] = jnxt[j]->Next();
		while(jnxt[j]){
		  SchTask *tsk = (SchTask *)jnxt[j]->Data();
		  if(tsk->Operation->Job == J[j]) break;
		  jnxt[j] = jnxt[j]->Next();
		}

		if(!mnxt[m]) nUsedMCs--;
		if(!jnxt[j]) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  delete [] J;
  delete [] M;
  delete [] mnxt;
  delete [] jnxt;

  return bFeasible;
}

//----------------------------------------------- MOSP_Algorithm Class
MOSP_Algorithm::MOSP_Algorithm(MOSP_Problem *P)
  : Algorithm(P, "MOSP Solver")
{
  Solution = NULL;
}

MOSP_Algorithm::MOSP_Algorithm(MOSP_Problem *P, char *N)
  : Algorithm(P, N)
{
  Solution = NULL;
}

MOSP_Algorithm::~MOSP_Algorithm()
{
  if(Solution) delete Solution;
}

void MOSP_Algorithm::AllocateArrays()
{
  SList::SNode *nd, *tsknd;

  nWCs = Problem->WCs.Count();
  nMCs = 0;
  nd = Problem->WCs.head();
  while(nd){
	MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)nd->Data();
	nMCs += wc->MCs.Count();
	nd = nd->Next();
  }

  W = new MOSP_Problem::SWC *[nWCs];
  M = new MOSP_Problem::SMC *[nMCs];
  Wa = new int[nWCs];
  Wz = new int[nWCs];
  Mw = new int[nMCs];

  nd = Problem->WCs.head();
  int i=0, j=0;
  while(nd){
	MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)nd->Data();
	W[i] = wc;
	wc->i = i;
	Wa[i] = j;

	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  M[j] = mc;
	  mc->i = j;
	  Mw[j] = i;
	  j++;
	  mcnd = mcnd->Next();
	}
	Wz[i] = j-1;

	i++;
	nd = nd->Next();
  }

  nJobs = Problem->Jobs.Count();
  nOps = 0;
  nd = Problem->Jobs.head();
  while(nd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)nd->Data();
	nOps += jb->Operations.Count();
	nd = nd->Next();
  }

  J = new MOSP_Problem::SJob *[nJobs];
  O = new MOSP_Problem::SOperation *[nOps];
  tsk = new MOSP_Solution::SchTask *[nOps];
  Ja = new int[nJobs];
  Jz = new int[nJobs];
  Oj = new int[nOps];

  nd = Problem->Jobs.head();
  tsknd = Solution->ScheduledTasks.head();
  i = j = 0;
  while(nd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)nd->Data();
	J[i] = jb;
	jb->i = i;
	Ja[i] = j;

	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  O[j] = op;
	  op->i = j;
	  Oj[j] = i;

	  MOSP_Solution::SchTask *task = (MOSP_Solution::SchTask *)tsknd->Data();
	  tsk[j] = task;

	  j++;
	  opnd = opnd->Next();
	  tsknd = tsknd->Next();
	}
	Jz[i] = j-1;

	i++;
	nd = nd->Next();
  }

  pt = new int *[nJobs];
  for(i=0; i<nJobs; i++){
	pt[i] = new int[nMCs];
	memset((void *)pt[i], 0, nMCs*sizeof(int));  // if pt is zero, then a job doesn't need the machine
	for(j=0; j<nMCs; j++){
	  for(int k=Ja[i]; k<=Jz[i]; k++){
		nd = O[k]->AltMCs.head();
		while(nd){
		  MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)nd->Data();
		  if(opmc->MC == M[j]){        // Remember that a job is to visit a machine
			pt[i][j] = opmc->pt;       // only once...
			break;
		  }
		  nd = nd->Next();
		}
	  }
	}
  }
}

void MOSP_Algorithm::FreeArrays()
{
  delete [] W;
  delete [] M;
  delete [] Wa;
  delete [] Wz;
  delete [] Mw;

  delete [] J;
  delete [] O;
  delete [] tsk;
  delete [] Ja;
  delete [] Jz;
  delete [] Oj;

  for(int i=0; i<nJobs; i++){
	delete [] pt[i];
  }
  delete [] pt;
}

void MOSP_Algorithm::AllocateMemory()
{
  if(Solution) delete Solution;
  Solution = new MOSP_Solution(Problem);
  AllocateArrays();
}

void MOSP_Algorithm::Initialize()
{
}

void MOSP_Algorithm::Improve()
{
}

bool MOSP_Algorithm::Stop()
{
  return true;
}

void MOSP_Algorithm::Finalize()
{
  // do nothing
}

void MOSP_Algorithm::FreeMemory()
{
  FreeArrays();

  // Keep the solution available for other algorithmic and interface uses
}

void MOSP_Algorithm::ComputeObjectiveValue()
{
  // Calculate the objective function value for the current solution "Solution"
  //   --> the objective value is -MAXFLOAT if the schedule is not complete
  //
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//------------------------ tskComp function to be used by sorting algorithms

int tskComp(void * Item1, void * Item2)
{
  MOSP_Solution::SchTask *tsk1, *tsk2;
  tsk1 = (MOSP_Solution::SchTask *)Item1;
  tsk2 = (MOSP_Solution::SchTask *)Item2;
  return(tsk1->StartTime - tsk2->StartTime);
}

//--------------------------------------------- Dense Schedule Generator (DSG)

MOSP_DSG::MOSP_DSG(MOSP_Problem *P, char *N)
  : MOSP_Algorithm(P, N)
{
}

MOSP_DSG::MOSP_DSG(MOSP_Problem *P, T_JobDispatchingRule DR, T_MCSelectionRule SR)
  : MOSP_Algorithm(P, "Dense Schedule Generator")
{
  JobDispatchingRule = DR;
  MCSelectionRule = SR;
}

MOSP_DSG::~MOSP_DSG()
{
}

void MOSP_DSG::AllocateMemory()
{
  MOSP_Algorithm::AllocateMemory();

  MCrt = new int[nMCs];
  for(int i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  bMCready = new bool[nMCs];
  bMCdone = new bool[nMCs];

  Jnft = new int[nJobs];
  bJcompleted = new bool[nJobs];
  for(int i=0; i<nJobs; i++){
	Jnft[i] = J[i]->ReleaseTime;
	bJcompleted[i] = false;
  }

  bJM = new bool *[nJobs];
  for (int i = 0; i < nJobs; i++){
	bJM[i] = new bool[nMCs];
	for(int m=0; m<nMCs; m++){
	  bJM[i][m] = (pt[i][m] > 0)? true : false;
	}
  }

  SchedulableJobs = new bool[nJobs];
  SelJob = new int[nMCs];
  bScheduled = new bool[nOps];
  SumSchedTime = new int[nMCs];
  for(int i=0; i<nMCs; i++) SumSchedTime[i] = 0;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  dbgFile = fopen("Debug.txt", "a+");
	 #endif
}

void MOSP_DSG::Initialize()
{
  int i, j, m;
  int t;   // Current scheduling time

  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);

  // Start with all operations unscheduled
  for(i=0; i<nOps; i++) bScheduled[i] = false;

  while(true){ // Main loop of the algorithm
	// Check for each machine if it is needed by any job or not
	for(m=0; m<nMCs; m++){
	  bool b = true;
	  for(j=0; j<nJobs; j++){
		if(bJM[j][m]){
		  b = false;
		  break;
		}
	  }
	  bMCdone[m] = b;
	}

	// If all machines are done, then stop the algorithm
	bool bAllDone = true;
	for(m=0; m<nMCs; m++){
	  if(!bMCdone[m]){
		bAllDone = false;
		break;
	  }
	}
	if(bAllDone) break;

	// Find the minimum machine ready time which is the current schedule time
	t = MAXINT;
	for(m=0; m<nMCs; m++){
	  if(!bMCdone[m] && MCrt[m] < t) t = MCrt[m];
	}

	// Define the set of schedulable machines at time t
	for(m=0; m<nMCs; m++){
	  bMCready[m] = (MCrt[m] == t && !bMCdone[m])? true : false;
	}

	// For each machine ready in time t, select a job
	//
	for(m=0; m<nMCs; m++){
		if(!bMCready[m]) continue;

		// Check to see if there is any job that needs to be scheduled on machine m
		//   preference is made first to jobs that has nearest finish time less than
		//   or equal t. IF there isn't any job with that condition, all schedulable
		//   jobs are considered.
		//
		int nsj = 0;  // start with zero number of schedulable jobs
		for(j=0; j<nJobs; j++){
		  if(bJM[j][m] && Jnft[j] <= t){
			SchedulableJobs[j] = true;
			nsj++;
		  }
		  else SchedulableJobs[j] = false;
		}
		if(nsj == 0){
		  for(j=0; j<nJobs; j++){
			if(bJM[j][m]){
			  SchedulableJobs[j] = true;
			  nsj++;
			}
			else SchedulableJobs[j] = false;
		  }
		  if(nsj == 0){  // Which shouldn't happen anyway!!!
			bMCdone[m] = true;
			continue;
		  }
		}

		// Now apply the job dispatching rule to select a job from all schedulable
		// jobs.
		//
		int Mr;  // The measure based upon which a job will be selected
		for(j=0; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  SelJob[m] = j;
		  switch(JobDispatchingRule){
			case FCFS:   // First Come First Serve
						 Mr = Jnft[j];  // use the job's nearest finish time for arrival order to a machine
						 break;
			case SPT:    // Shortest Processing Time
			case LPT:    // Longest Processing Time
						 Mr = pt[j][m];
						 break;
			case MCT:    // Minimum Completion Time
						 Mr = Jnft[j] + pt[j][m];
						 break;
			case MOR:    // Most Operations Remaining
			case LOR:    // Least Operations Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) Mr++;
						 }
						 break;
			case MWR:    // Most Work Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   Mr += O[i]->maxpt();
						 }
						 break;
			case LWR:    // Lease Work Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   Mr += O[i]->minpt();
						 }
						 break;
		  }
		  break;
		}
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  int opsr, wrk;
		  switch(JobDispatchingRule){
			case FCFS:   // First Come First Serve
						 if(Jnft[j] < Mr){
						   SelJob[m] = j;
						   Mr = Jnft[j];
						 }
						 break;
			case SPT:    // Shortest Processing Time
						 if(pt[j][m] < Mr){
						   SelJob[m] = j;
						   Mr = pt[j][m];
						 }
						 break;
			case LPT:    // Longest Processing Time
						 if(pt[j][m] > Mr){
						   SelJob[m] = j;
						   Mr = pt[j][m];
						 }
						 break;
			case MCT:    // Minimum Completion Time
						 if(Jnft[j] + pt[j][m] < Mr){
						   SelJob[m] = j;
						   Mr = Jnft[j] + pt[j][m];
						 }
						 break;
			case MOR:    // Most Operations Remaining
						 opsr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) opsr++;
						 }
						 if(opsr > Mr){
						   Mr = opsr;
						   SelJob[m] = j;
						 }
						 break;
			case LOR:    // Least Operations Remaining
						 opsr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) opsr++;
						 }
						 if(opsr < Mr){
						   Mr = opsr;
						   SelJob[m] = j;
						 }
						 break;
			case MWR:    // Most Work Remaining
						 wrk = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   wrk += O[i]->maxpt();
						 }
						 if(wrk > Mr){
						   Mr = wrk;
						   SelJob[m] = j;
						 }
						 break;
			case LWR:    // Lease Work Remaining
						 wrk = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   wrk += O[i]->minpt();
						 }
						 if(wrk < Mr){
						   Mr = wrk;
						   SelJob[m] = j;
						 }
						 break;
			case RANDOM: // Random
						 if(flip(0.5)) SelJob[m] = j;
		  }
		}
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections using the machines'
	//   selection rule
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
      if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
        if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  if(MCSelectionRule == MIN_LOAD){
			// Select the machine with the minimum load
			if(SumSchedTime[m] <= SumSchedTime[m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else if(MCSelectionRule == MINCT){
			// Select the machine that results in lower completion time
			if(pt[j][m] <= pt[j][m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else{
			// Select a machine randomly
			SelMC = (flip(0.5))? m : m2;
		  }

		  if(SelMC == m) SelJob[m2] = -1;
		  else{
			SelJob[m] = -1;
			break;
		  }
		}
	  }
	}

	// Now, for all machines that have assigned values in SelJob and in the current
	// list of ready machines, schedule their jobs and update all necessary varaibles.
	//
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j == -1) continue;

	  // define which operation index
	  for(i=Ja[j]; i<=Jz[j]; i++) if(O[i]->WC == M[m]->WC) break;
	  if(i > Jz[j]) continue;  // <-- this should not happen!!

	  // Update machine and job times and set operation and task related variables
	  tsk[i]->SelectedMC = M[m];
	  tsk[i]->StartTime = (Jnft[j] > MCrt[m])? Jnft[j] : MCrt[m];
	  Jnft[j] = tsk[i]->StartTime + pt[j][m];
	  MCrt[m] = Jnft[j];
	  tsk[i]->EndTime = Jnft[j];
	  SumSchedTime[m] += pt[j][m];
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // Now job j does not need any machine in the workcenter p
	  int p = Mw[m];  // index of the WC to which machine m belongs
	  for(int m2=Wa[p]; m2<=Wz[p]; m2++) bJM[j][m2] = false;

	  // Check if all operations of job j are completed
	  bool bAllDone = true;
	  for(int i2=Ja[j]; i2<=Jz[j]; i2++){
		if(!bScheduled[i2]){
		  bAllDone = false;
		  break;
		}
	  }
	  bJcompleted[j] = bAllDone;
	}
  } // End of main loop
}

bool MOSP_DSG::Stop()
{
  return true;
}

void MOSP_DSG::Finalize()
{
  MOSP_Algorithm::Finalize();
  SolStatus = FEASIBLE;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  char buf[256];
		  sprintf(buf, "Initial solution from DSG:\n\n");
		  fprintf(dbgFile, buf);
		  sprintf(buf, "\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fprintf(dbgFile, buf);
		  for(int i=0; i<nOps; i++){
			sprintf(buf, "\t%i\t%s\t%s\t%s\t%i\t%i\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), (int)(tsk[i]->StartTime),
						 (int)(tsk[i]->EndTime));
			fprintf(dbgFile, buf);
		  }
		  sprintf(buf, "\n\tCmax = %i\n\n", (int)Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN));
		  fprintf(dbgFile, buf);
	 #endif
}

void MOSP_DSG::FreeMemory()
{
  MOSP_Algorithm::FreeMemory();

  delete [] MCrt;
  delete [] bMCready;
  delete [] bMCdone;

  delete [] Jnft;
  delete [] bJcompleted;
  for (int i = 0; i < nJobs; i++) delete [] bJM[i];
  delete [] bJM;
  delete [] SchedulableJobs;
  delete [] SelJob;
  delete [] bScheduled;
  delete [] SumSchedTime;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fclose(dbgFile);
	 #endif
}

void MOSP_DSG::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//---------------------------------------- Vector representation of solutions

MOSP_VectorSol::MOSP_VectorSol(MOSP_Algorithm *a)
{
  Alg = a;
  tau = new SList[Alg->nJobs];
  pi = new SList[Alg->nMCs];
  mc = new int[Alg->nOps];
  memset(mc, -1, Alg->nOps * sizeof(int));
  u = new char[Alg->nOps];
  memset(u, 0, Alg->nOps * sizeof(char));
}

MOSP_VectorSol::MOSP_VectorSol(MOSP_VectorSol *V)
{
  Alg = V->Alg;
  tau = new SList[Alg->nJobs];
  pi = new SList[Alg->nMCs];
  mc = new int[Alg->nOps];
  u = new char[Alg->nOps];
  Copy(V);
}

MOSP_VectorSol::~MOSP_VectorSol()
{
  delete [] tau;
  delete [] pi;
  delete [] mc;
  delete [] u;
}

void MOSP_VectorSol::Copy(MOSP_VectorSol *V)
{
  if(Alg != V->Alg) return;
  for(int j=0; j<Alg->nJobs; j++) tau[j] = V->tau[j];
  for(int m=0; m<Alg->nMCs; m++) pi[m] = V->pi[m];
  memcpy(mc, V->mc, sizeof(int)*Alg->nOps);
  memcpy(u, V->u, sizeof(char)*Alg->nOps);
}

#ifdef REPORT_VECTOR_RESULTS
void MOSP_VectorSol::Assert(char *heading)
{
  int m, j;
  char buf[256];
  fprintf(dbgFile, "\n\n%s:\n\n", heading);
  fprintf(dbgFile, "\ta) pi\'s:\n");
  for(m=0; m<Alg->nMCs; m++){
	sprintf(buf, "\t\t%s: ", Alg->M[m]->ID());
	fprintf(dbgFile, buf);
	SList::SNode *pind = headOfPi(m);
	if(!pind) fprintf(dbgFile, "\n");
	while(pind){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)pind->Data();
	  if(u[op->i] & 2){
		sprintf(buf, " %s ", op->ID());
		fprintf(dbgFile, buf);
	  }
	  pind = pind->Next();
	  if(pind) sprintf(buf, "->");
	  else sprintf(buf, "\n");
	  fprintf(dbgFile, buf);
	}
  }
  fprintf(dbgFile, "\n\tb) taus:\n");
  for(j=0; j<Alg->nJobs; j++){
	sprintf(buf, "\t\t%s: ", Alg->J[j]->ID());
	fprintf(dbgFile, buf);
	SList::SNode *tund = headOfTau(j);
	if(!tund) fprintf(dbgFile, "\n");
	while(tund){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)tund->Data();
	  if(u[op->i] & 1){
		sprintf(buf, " %s ", op->ID());
		fprintf(dbgFile, buf);
	  }
	  tund = tund->Next();
	  if(tund) sprintf(buf, "->");
	  else sprintf(buf, "\n");
	  fprintf(dbgFile, buf);
	}
  }
  fprintf(dbgFile, "\n\n");
  fflush(dbgFile);
}
#endif

void MOSP_VectorSol::ConvertTasksToVect()
{
  Clear();

  SList &Tasks = Alg->Solution->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC){
	  AppendOp2Mc(tsk->SelectedMC, tsk->Operation);
	  AppendOp2Jb(tsk->Operation);
	  u[tsk->Operation->i] = 3;
	}
  }

		  #ifdef REPORT_VECTOR_RESULTS
			  Assert("Vector representation after interpreting tasks");
		  #endif
}

bool MOSP_VectorSol::AppendOp2Mc(int m, MOSP_Problem::SOperation *op)
{
  if(mc[op->i] != -1) return false;
  if(m < Alg->Wa[op->WC->i] || m > Alg->Wz[op->WC->i]) return false;

  pi[m].Add(op);
  mc[op->i] = m;
  return true;
}

bool MOSP_VectorSol::AppendOp2Mc(MOSP_Problem::SMC *m, MOSP_Problem::SOperation *op)
{
  return AppendOp2Mc(m->i, op);
}

bool MOSP_VectorSol::AppendOp2Jb(MOSP_Problem::SOperation *op)
{
  if(tau[op->Job->i].IndexOf((void *)op) != -1) return false;
  tau[op->Job->i].Add(op);
  return true;
}

bool MOSP_VectorSol::RemoveOp(MOSP_Problem::SOperation *op)
{
  if(mc[op->i] == -1) return false;
  pi[mc[op->i]].Remove((void *)op);
  mc[op->i] = -1;
  tau[op->Job->i].Remove((void *)op);
  u[op->i] = 0;
  return true;
}

void MOSP_VectorSol::Clear()
{
  for(int j=0; j<Alg->nJobs; j++) tau[j].Clear();
  for(int m=0; m<Alg->nMCs; m++) pi[m].Clear();
  memset(mc, -1, Alg->nOps * sizeof(int));
  memset(u, 0, Alg->nOps * sizeof(char));
}

bool MOSP_VectorSol::isComplete()   // assuming no replication of operations anywhere
{                                   // in taus and pi's
  int sum = Alg->nOps;
  for(int i=0; i<Alg->nOps; i++) sum -= (int)u[i] / 3;

  if(sum != 0) return false;

  /*for(int j=0; j<Alg->nJobs; j++) sum += tau[j].Count();
  if(sum != Alg->nOps) return false;

  for(int m=0; m<Alg->nMCs; m++) sum -= pi[m].Count();
  if(sum != 0) return false;*/

  return true;
}

bool MOSP_VectorSol::isFeasible()
{
  bool bFeasible = true;
  int j, m;

  int nUsedMCs = Alg->nMCs;          // number of machines that have nodes
  int nIncludedJobs = Alg->nJobs;    // number of jobs with nodes included

  SList::SNode **mnxt = new SList::SNode *[Alg->nMCs];    // next node index for the machine
  for(m=0; m<Alg->nMCs; m++){
	mnxt[m] = pi[m].head();
	if(!mnxt[m]) nUsedMCs--;
  }

  SList::SNode **jnxt = new SList::SNode *[Alg->nJobs];   // next node index for the job
  for(j=0; j<Alg->nJobs; j++){
	jnxt[j] = tau[j].head();
	if(!jnxt[j]) nIncludedJobs--;
  }

  // Check for the infeasibility now
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<Alg->nMCs; m++){
	  if(!mnxt[m]) continue;
	  MOSP_Problem::SOperation *mop = (MOSP_Problem::SOperation *)mnxt[m]->Data();
	  j = mop->Job->i;
	  MOSP_Problem::SOperation *jop = (MOSP_Problem::SOperation *)jnxt[j]->Data();
	  if(mop == jop){
		mnxt[m] = mnxt[m]->Next();
		jnxt[j] = jnxt[j]->Next();
		if(!mnxt[m]) nUsedMCs--;
		if(!jnxt[j]) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  delete [] mnxt;
  delete [] jnxt;

  return bFeasible;
}

int MOSP_VectorSol::Psi_j(MOSP_VectorSol *V)
{
  int ret = 0;
  for(int j=0; j<Alg->nJobs; j++){
	int p = psi(tau[j], V->tau[j]);
	if(p >= 0) ret += p;
	else return -1;
  }
  return ret;
}

int MOSP_VectorSol::Psi_w(MOSP_VectorSol *V)
{
  int ret = 0;
  SList P1, P2;
  for(int w=0; w<Alg->nWCs; w++){
	P1.Clear();
	P2.Clear();
	for(int m=Alg->Wa[w]; m<=Alg->Wz[w]; m++){
	  P1.Add(pi[m]);
	  P2.Add(V->pi[m]);
	  P1.Add((void *)Alg->M[m]);
	  P2.Add((void *)Alg->M[m]);
	}
	int p = psi(P1, P2);
	if(p >= 0) ret += p;
	else return -1;
  }
  return ret;
}

bool MOSP_VectorSol::RemoveTauSubpermutation(int j, int x1, int x2)
{
  if(x1 > x2) return false;
  SList &tau_j = tau[j];
  int l = tau_j.Count();
  if(x2 > l) return false;

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  for(; i<=x2; i++){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	u[op->i] &= 2;
	nd = nd->Next();
  }
  return true;
}

bool MOSP_VectorSol::RemovePiSubpermutation(int m, int x1, int x2)
{
  if(x1 > x2) return false;
  SList &pi_m = pi[m];
  int l = pi_m.Count();
  if(x2 > l) return false;

  int i=1;
  SList::SNode *nd = pi_m.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  for(; i<=x2; i++){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	u[op->i] &= 1;
	nd = nd->Next();
  }
  return true;
}

void MOSP_VectorSol::UnremoveAll()
{
  memset(u, 3, Alg->nOps * sizeof(char));
}

bool MOSP_VectorSol::MoveOpToTauPos(MOSP_Problem::SOperation *op, int x, bool bchkpos)
{
  SList &tau_j = tau[op->Job->i];
  int l = tau_j.Count();

  if(bchkpos) if(x < 1 || x > l) return false;

  tau_j.Remove((void *)op);

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  if(nd) tau_j.AddBefore(nd, op);
  else tau_j.Add(op);

		 #ifdef REPORT_VECTOR_RESULTS
			Assert("Vector solution after moving operation in job tau");
		 #endif

  return true;
}

bool MOSP_VectorSol::MoveOpToPiPos(MOSP_Problem::SOperation *op, int k, int x, bool bchkpos)
{
  int m = mc[op->i];
  SList &pi_m = pi[m];
  SList &pi_k = pi[k];
  int lm = pi_m.Count();
  int lk = pi_k.Count();

  if(bchkpos){
	if(x < 1) return false;
	if(m == k) if(x > lm) return false;
	else if(x > lk+1) return false;
  }

  pi_m.Remove((void *)op);

  int i=1;
  SList::SNode *nd = pi_k.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  if(nd) pi_k.AddBefore(nd, (void *)op);
  else pi_k.Add((void *)op);

		 #ifdef REPORT_VECTOR_RESULTS
			Assert("Vector solution after moving operation between machine permutations");
		 #endif

  return true;
}

bool MOSP_VectorSol::MoveTauSubpermutation(int j, int x1, int x2, int x, bool bchkpos)
{
  SList &tau_j = tau[j];
  int l = tau_j.Count();

  if(bchkpos){
	if(x1 < 1 || x1 > x2 || x2 > l) return false;
	if(x1 == 1 && x2 == l){
	  if(x == 1) return true; // trivial!!
	  else return false;
	}
	if(x < 1 || x > l - (x2 - x1)) return false;
  }

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  SList tmpOp;
  for(; i<=x2; i++){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	tmpOp.Add((void *)op);
	SList::SNode *nxtnd = nd->Next();
	tau_j.Remove(nd);
	nd = nxtnd;
  }

  i=1;
  nd = tau_j.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  for(SList::SNode *tmpnd = tmpOp.head(); tmpnd; tmpnd = tmpnd->Next()){
	if(nd) tau_j.AddBefore(nd, tmpnd->Data());
	else tau_j.Add(tmpnd->Data());
  }

		 #ifdef REPORT_VECTOR_RESULTS
			Assert("Vector solution after moving job subpermutation");
		 #endif

  return true;
}

bool MOSP_VectorSol::MovePiSubpermutation(int m, int x1, int x2, int k, int x, bool bchkpos)
{
  SList &pi_m = pi[m];
  SList &pi_k = pi[k];
  int lm = pi_m.Count();
  int lk = pi_k.Count();

  if(bchkpos){
	if(x1 < 1 || x1 > x2 || x2 > lm || x < 1) return false;
	if(m == k){
	  if(x1 == 1 && x2 == lm){
		if(x == 1) return true; // trivial!!
		else return false;
	  }
	  if(x > lm - (x2 - x1)) return false;
	}
	else{
	  if(x > lk+1) return false;
	}
  }

  int i=1;
  SList::SNode *nd = pi_m.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  SList tmpOp;
  for(; i<=x2; i++){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	tmpOp.Add((void *)op);
	SList::SNode *nxtnd = nd->Next();
	pi_m.Remove(nd);
	nd = nxtnd;
  }

  i=1;
  nd = pi_k.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  for(SList::SNode *tmpnd = tmpOp.head(); tmpnd; tmpnd = tmpnd->Next()){
	if(nd) pi_k.AddBefore(nd, tmpnd->Data());
	else pi_k.Add(tmpnd->Data());
  }

		 #ifdef REPORT_VECTOR_RESULTS
			Assert("Vector solution after moving machine subpermutation");
		 #endif

  return true;
}

int MOSP_VectorSol::TauPosition(MOSP_Problem::SOperation *op)
{
  SList &tau_j = tau[op->Job->i];
  return tau_j.IndexOf((void *)op) + 1;
}

int MOSP_VectorSol::PiPosition(MOSP_Problem::SOperation *op)
{
  SList &pi_m = pi[mc[op->i]];
  return pi_m.IndexOf((void *)op) +  1;
}

MOSP_Problem::SMC *MOSP_VectorSol::MCofOp(MOSP_Problem::SOperation *op)
{
  return Alg->M[mc[op->i]];
}

//---------------------------------------- Network representation of solutions

MOSP_Network::MOSP_Network(MOSP_Algorithm *a)
{
  Alg = a;
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  Nodes = new MOSP_Node[nOps];
  FirstjNode = new int[nJobs];
  LastjNode = new int[nJobs];
  FirstmNode = new int[nMCs];
  LastmNode = new int[nMCs];

  MCrt = new int[nMCs];    // machine's ready time
  mnxt = new int[nMCs];    // next node index for the machine
  Jnft = new int[nJobs];   // job's nearest finish time
  jnxt = new int[nJobs];   // next node index for the job

  jcur = new int[nJobs];
  mcur = new int[nMCs];
}

MOSP_Network::MOSP_Network(MOSP_Network *nt)
{
  Alg = nt->Alg;
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  Nodes = new MOSP_Node[nOps];
  FirstjNode = new int[nJobs];
  LastjNode = new int[nJobs];
  FirstmNode = new int[nMCs];
  LastmNode = new int[nMCs];

  MCrt = new int[nMCs];    // machine's ready time
  mnxt = new int[nMCs];    // next node index for the machine
  Jnft = new int[nJobs];   // job's nearest finish time
  jnxt = new int[nJobs];   // next node index for the job

  jcur = new int[nJobs];
  mcur = new int[nMCs];

  Copy(nt);
}

MOSP_Network::~MOSP_Network()
{
  delete [] Nodes;
  delete [] FirstjNode;
  delete [] LastjNode;
  delete [] FirstmNode;
  delete [] LastmNode;

  delete [] MCrt;
  delete [] mnxt;
  delete [] Jnft;
  delete [] jnxt;
  delete [] jcur;
  delete [] mcur;
}

#ifdef REPORT_NETWORK_RESULTS
void MOSP_Network::Assert(char *heading)
{
  char buf[256];
  fprintf(dbgFile, "\n\n%s:\n\n", heading);
  fprintf(dbgFile, "\ta) List of Nodes:\n\n");
  fprintf(dbgFile, "\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\tweight\testart\tlstart\ttail\tslack\tRank\tCritical\n");
  for(int i=0; i<Alg->nOps; i++){
	if(Nodes[i].mc != -1)
		 sprintf(buf, "\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%c\n",
				 i, Alg->O[i]->ID(), Nodes[i].j_prv, Nodes[i].j_nxt,
				 Alg->M[Nodes[i].mc]->ID(), Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? '*' : ' '));
	else
		 sprintf(buf, "\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%c\n",
				 i, Alg->O[i]->ID(), Nodes[i].j_prv, Nodes[i].j_nxt,
				 "N/A", Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? '*' : ' '));
	fprintf(dbgFile, buf);
  }
  fprintf(dbgFile, "\n\n\tb) List of Jobs:\n\n");
  fprintf(dbgFile, "\tID\tFirst\tLast\n");
  for(int j=0; j<Alg->nJobs; j++){
	sprintf(buf, "\t%s\t%i\t%i\n", Alg->J[j]->ID(), FirstjNode[j], LastjNode[j]);
	fprintf(dbgFile, buf);
  }
  fprintf(dbgFile, "\n\n\tc) List of Machines:\n\n");
  fprintf(dbgFile, "\tID\tFirst\tLast\n");
  for(int m=0; m<Alg->nMCs; m++){
	sprintf(buf, "\t%s\t%i\t%i\n", Alg->M[m]->ID(), FirstmNode[m], LastmNode[m]);
	fprintf(dbgFile, buf);
  }
  sprintf(buf, "\n\t\tCmax = %i\n\n", Cmax);
  fprintf(dbgFile, buf);
  fflush(dbgFile);
}
#endif

void MOSP_Network::Copy(MOSP_Network *nt)
{     // Assuming no change in the problem (nJobs, nMCs, and nOps)
  if(Alg != nt->Alg) return;
  memcpy(Nodes, nt->Nodes, Alg->nOps * sizeof(MOSP_Node));
  memcpy(FirstjNode, nt->FirstjNode, Alg->nJobs * sizeof(int));
  memcpy(LastjNode, nt->LastjNode, Alg->nJobs * sizeof(int));
  memcpy(FirstmNode, nt->FirstmNode, Alg->nMCs * sizeof(int));
  memcpy(LastmNode, nt->LastmNode, Alg->nMCs * sizeof(int));
  Cmax = nt->Cmax;
}

int MOSP_Network::GetCPath(int *pth)
{
  int m, i, l=0;  // There should be at least on critical operation

  // Find a first critical node
  for(i=0; i<Alg->nOps; i++){
	if(Nodes[i].bCritical && Nodes[i].m_prv == -1 && Nodes[i].j_prv == -1) break;
  }
  pth[l] = i;

  while(true){
	int jnxt = Nodes[i].j_nxt;
	int mnxt = Nodes[i].m_nxt;

	if(jnxt == -1 && mnxt == -1) break;

	if(jnxt != -1 && mnxt == -1){  // the next operation in job route must be critical
	  i = jnxt;
	  pth[++l] = i;
	}

	if(jnxt == -1 && mnxt != -1){  // the next operation in machine sequence must be critical
	  i = mnxt;
	  pth[++l] = i;
	}

	if(jnxt != -1 && mnxt != -1){
	  if(Nodes[jnxt].bCritical && Nodes[mnxt].bCritical){
		if(Nodes[jnxt].estart <= Nodes[mnxt].estart){
		  i = jnxt;
		  pth[++l] = i;
		}
		else{
		  i = mnxt;
		  pth[++l] = i;
		}
	  }
	  else if(Nodes[mnxt].bCritical){     // It must be the next operation in machine sequence critical
		i = mnxt;
		pth[++l] = i;
	  }
	  else{
		i = jnxt;
		pth[++l] = i;
	  }
	}
  }

  return l+1;
}

void MOSP_Network::ConvertTasksToNet()    // Assuming the the solution is feasible
                                          // and task start times are their earliest starts
{                                         // however it can be partial (incomplete)
  Reset();                                // Note: node ranks and latest start times are
                                          //       not always correct whenever
                                          //       task durations are zeros.....
  memset(jcur, -1, Alg->nJobs * sizeof(int));
  memset(mcur, -1, Alg->nMCs * sizeof(int));

  SList &Tasks = Alg->Solution->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  int rem = Alg->nOps; // number of remaining operations which still need to define their tails

  Cmax = 0;
  for(SList::SNode *nd = Tasks.head(); nd; nd = nd->Next()){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)nd->Data();
	int i = tsk->Operation->i;
	if(tsk->SelectedMC){
	  int m = tsk->SelectedMC->i;
	  int j = Alg->Oj[i];

	  Nodes[i].mc = m;
	  Nodes[i].estart = tsk->StartTime;
	  Nodes[i].weight = tsk->EndTime - tsk->StartTime;
	  if(tsk->EndTime > Cmax) Cmax = tsk->EndTime;

	  Nodes[i].m_prv = mcur[m];
	  if(mcur[m] != -1){
		Nodes[mcur[m]].m_nxt = i;
		Nodes[i].rank = Nodes[mcur[m]].rank + 1;
	  }
	  else FirstmNode[m] = i;     // the rank of the first machine node is determined by job nodes
	  LastmNode[m] = i;
	  mcur[m] = i;

	  Nodes[i].j_prv = jcur[j];
	  if(jcur[j] != -1){
		Nodes[jcur[j]].j_nxt = i;
		Nodes[i].rank += (Nodes[jcur[j]].rank + 1);
	  }
	  else{
		FirstjNode[j] = i;
		Nodes[i].rank += 1;
	  }
	  LastjNode[j] = i;
	  jcur[j] = i;
	}
	else rem--;
  }

  // Evaluate the latest start times of the nodes and determine their criticality conditions
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = -1, imaxEFT = -1;
	for(int i = 0; i < Alg->nOps; i++){
	  if(Nodes[i].lstart != -1) continue;  // tail is already defined
	  int eft = Nodes[i].estart + Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	  else if(eft == maxEFT)
		if(Nodes[i].rank > Nodes[imaxEFT].rank){
		  maxEFT = eft;
		  imaxEFT = i;
		}
	}

	int mlft, jlft, lft;
	if(Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = Nodes[Nodes[imaxEFT].m_nxt].lstart;
	if(Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = Nodes[Nodes[imaxEFT].j_nxt].lstart;
	lft = ((mlft < jlft)? mlft : jlft);
	Nodes[imaxEFT].lstart = lft - Nodes[imaxEFT].weight;
	Nodes[imaxEFT].tail = Cmax - Nodes[imaxEFT].lstart;
	Nodes[imaxEFT].slack = Nodes[imaxEFT].lstart - Nodes[imaxEFT].estart;
	if(Nodes[imaxEFT].slack == 0) Nodes[imaxEFT].bCritical = true;
	else Nodes[imaxEFT].bCritical = false;

	rem--;
  }

	 #ifdef REPORT_NETWORK_RESULTS
        Assert("Network representation based on tasks list");
	 #endif
}

void MOSP_Network::InterpretNetwork(bool &bFeasible, bool &bComplete)
{
  int i, j, m;

  bFeasible = bComplete = true;

  for(i=0; i<Alg->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  int nUsedMCs = Alg->nMCs;          // number of machines that have nodes
  int nIncludedJobs = Alg->nJobs;    // number of jobs with nodes included

  for(m=0; m<Alg->nMCs; m++){
	MCrt[m] = Alg->M[m]->ReadyTime;
	mnxt[m] = FirstmNode[m];
	if(mnxt[m] == -1) nUsedMCs--;
  }

  for(j=0; j<Alg->nJobs; j++){
	Jnft[j] = Alg->J[j]->ReleaseTime;
	jnxt[j] = FirstjNode[j];
	if(jnxt[j] == -1) nIncludedJobs--;
  }

  if(nIncludedJobs != Alg->nJobs) bComplete = false;

  // Determine the earliest start times for all operations, and determine Cmax
  Cmax = 0;
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<Alg->nMCs; m++){
	  i = mnxt[m];
	  if(i==-1) continue;
	  j = Alg->O[i]->Job->i;
	  if(jnxt[j] == i){
		Nodes[i].estart = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		MCrt[m] = Jnft[j] = Nodes[i].estart + Nodes[i].weight;
		if(Cmax < MCrt[m]) Cmax = MCrt[m];
		mnxt[m] = Nodes[i].m_nxt;
		jnxt[j] = Nodes[i].j_nxt;

		MOSP_Node &ndi = Nodes[i];
		if(ndi.m_prv != -1) ndi.rank = Nodes[ndi.m_prv].rank + 1;
		if(ndi.j_prv != -1) ndi.rank += (Nodes[ndi.j_prv].rank + 1);
		else ndi.rank += 1;

		if(mnxt[m] == -1) nUsedMCs--;
		if(jnxt[j] == -1) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  int rem = Alg->nOps; // number of remaining operations which need to define start/end times
  for(i=0; i<Alg->nOps; i++)
	if(Nodes[i].estart == -1){
		 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fprintf(dbgFile, "\n\n\t------------ Incomplete schedule is encountered!!!");
		 #endif
	   rem--;
	}
  bComplete = (rem == Alg->nOps)? true : false;

  if(!bFeasible){
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n\t------------ Infeasible solution is encountered!!!");
	 #endif

	Cmax = MAXINT;
  }
  else{
	// Now, we define the latest start and criticality conditions for nodes
	while(rem > 0){
	  // Search for an operation with the most earliest finish time
	  int maxEFT = -1, imaxEFT = -1;
	  for(i = 0; i < Alg->nOps; i++){
		if(Nodes[i].estart == -1) continue;  // the operation is not in the network
		if(Nodes[i].lstart != -1) continue;  // latest start is already defined
		int eft = Nodes[i].estart + Nodes[i].weight;
		if(eft > maxEFT){
		  maxEFT = eft;
		  imaxEFT = i;
		}
		else if(eft == maxEFT)
		  if(Nodes[i].rank > Nodes[imaxEFT].rank){
			maxEFT = eft;
			imaxEFT = i;
		  }
	  }

	  int mlft, jlft, lft;
	  if(Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	  else mlft = Nodes[Nodes[imaxEFT].m_nxt].lstart;
	  if(Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	  else jlft = Nodes[Nodes[imaxEFT].j_nxt].lstart;
	  lft = ((mlft < jlft)? mlft : jlft);
	  Nodes[imaxEFT].lstart = lft - Nodes[imaxEFT].weight;
	  Nodes[imaxEFT].tail = Cmax - Nodes[imaxEFT].lstart;
	  Nodes[imaxEFT].slack = Nodes[imaxEFT].lstart - Nodes[imaxEFT].estart;
	  if(Nodes[imaxEFT].slack == 0) Nodes[imaxEFT].bCritical = true;
	  else Nodes[imaxEFT].bCritical = false;
	  rem--;
	}
  }

	 #ifdef REPORT_NETWORK_RESULTS
	   Assert("Network representation");
	 #endif
}

bool MOSP_Network::ConvertNetToTasks()
{
  bool bF, bC;
  InterpretNetwork(bF, bC);
  if(!bF) return false;

  for(int i=0; i<Alg->nOps; i++){
	if(Nodes[i].estart != -1){
	  Alg->tsk[i]->SelectedMC = Alg->M[Nodes[i].mc];
	  Alg->tsk[i]->StartTime = Nodes[i].estart;
	  Alg->tsk[i]->EndTime = Nodes[i].estart + Nodes[i].weight;
	}
	else{
	  Alg->tsk[i]->SelectedMC = NULL;
	  Alg->tsk[i]->StartTime = 0;
	  Alg->tsk[i]->EndTime = 0;
	}
  }

  return true;
}

bool MOSP_Network::InterpretIncompleteNetwork()
{
  int i, j, k, m;
  int jprv, mprv;

  for(i=0; i<Alg->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  // Determine nodes' ranks to be used in an ordered assignment of
  // of earliest start times
  //
  // First, define initial rank values for nodes that do not have both machine
  // and job prdecessors.
  //
  int r=0;
  for(i=0; i<Alg->nOps; i++){
	if(Nodes[i].m_prv == -1 && Nodes[i].j_prv == -1){
	  Nodes[i].rank = 1;
	  r++;
	}
	else{
	  Nodes[i].rank = 0;
	}
  }

  // Second, loop through the remaining nodes to define their ranks and check for
  // cycles...
  while(r < Alg->nOps){
	bool bFound;
	for(i=0; i<Alg->nOps; i++){
	  if(Nodes[i].rank != 0) continue;
	  bool bpm = false, bpj = false;
	  jprv = Nodes[i].j_prv;
	  mprv = Nodes[i].m_prv;
	  if(jprv != -1){
		if(Nodes[jprv].rank > 0){
		  Nodes[i].rank += (Nodes[jprv].rank + 1);
		  bpj = true;
		}
	  }
	  else bpj = true;
	  if(mprv != -1){
		if(Nodes[mprv].rank > 0){
		  Nodes[i].rank += (Nodes[mprv].rank + 1);
		  bpm = true;
		}
	  }
	  else bpm = true;
	  if(!bpj || !bpm){
		Nodes[i].rank = 0;
		bFound = false;
		continue;
	  }
	  bFound = true;
	  r++;
	  break;
	}
	if(!bFound) return false;      // there is a cycle...
  }

  // Determine the earliest start times for all operations, and determine Cmax
  int rem = 0;
  Cmax = 0;
  for(r=0; r<Alg->nOps; r++){
	// find operation i with the lowest rank
	i = -1;
	int lowest_rank = MAXINT;
	for(k=0; k<Alg->nOps; k++){
	  if(Nodes[k].estart == -1 && Nodes[k].rank != -1){
		int k_rank = Nodes[k].rank;
		if(k_rank < lowest_rank){
		  lowest_rank = k_rank;
		  i = k;
		}
	  }
	}
	if(i == -1) break;

	jprv = Nodes[i].j_prv;
	mprv = Nodes[i].m_prv;

	int jes, mes;
	if(jprv == -1) jes = 0;
	else jes = Nodes[jprv].estart + Nodes[jprv].weight;
	if(mprv == -1) mes = 0;
	else mes = Nodes[mprv].estart + Nodes[mprv].weight;

	Nodes[i].estart = (jes > mes)? jes : mes;
	rem++;

	int cti = Nodes[i].estart + Nodes[i].weight;
	if(Cmax < cti) Cmax = cti;
  }

  // Now, we define the latest start and criticality conditions for nodes
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = -1, imaxEFT = -1;
	for(i = 0; i < Alg->nOps; i++){
	  if(Nodes[i].estart == -1) continue;  // the operation is not in the network
	  if(Nodes[i].lstart != -1) continue;  // latest start is already defined
	  int eft = Nodes[i].estart + Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	  else if(eft == maxEFT)
		if(Nodes[i].rank > Nodes[imaxEFT].rank){
		  maxEFT = eft;
		  imaxEFT = i;
	  }
	}

	int mlft, jlft, lft;
	if(Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = Nodes[Nodes[imaxEFT].m_nxt].lstart;
	if(Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = Nodes[Nodes[imaxEFT].j_nxt].lstart;
	lft = ((mlft < jlft)? mlft : jlft);
	Nodes[imaxEFT].lstart = lft - Nodes[imaxEFT].weight;
	Nodes[imaxEFT].tail = Cmax - Nodes[imaxEFT].lstart;
	Nodes[imaxEFT].slack = Nodes[imaxEFT].lstart - Nodes[imaxEFT].estart;
	if(Nodes[imaxEFT].slack == 0) Nodes[imaxEFT].bCritical = true;
	else Nodes[imaxEFT].bCritical = false;
	rem--;
  }

	 #ifdef REPORT_NETWORK_RESULTS
	   Assert("Incomplete network representation");
	 #endif

  return true;
}

bool MOSP_Network::ConvertIncompleteNetToTasks()
{
  if(!InterpretIncompleteNetwork()) return false;

  for(int i=0; i<Alg->nOps; i++){
	Alg->tsk[i]->SelectedMC = (Nodes[i].mc == -1)? NULL : Alg->M[Nodes[i].mc];
	Alg->tsk[i]->StartTime = Nodes[i].estart;
	Alg->tsk[i]->EndTime = Nodes[i].estart + Nodes[i].weight;
  }

  return true;
}


bool MOSP_Network::InterpretVectors(MOSP_VectorSol *V, bool bckf)
{
  if(bckf)  // If we have to check the feasibility of the vector solution first
	if(!V->isFeasible()) return false;

  Reset();

  // interpret vectors into nodes and arcs only and assign node weights
  int i, k, j, m;

  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;

  for(m=0; m<nMCs; m++){
	SList::SNode *pind = V->headOfPi(m);
	while(pind){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)pind->Data();
	  i = op->i;
	  if(V->u[i] & 2){
		FirstmNode[m] = i;
		Nodes[i].mc = m;
		Nodes[i].m_prv = -1;
		Nodes[i].weight = Alg->pt[op->Job->i][m];
		while(true){
		  pind = pind->Next();
		  if(pind){
			op = (MOSP_Problem::SOperation *)pind->Data();
			k = op->i;
			if(V->u[k] & 2){
			  Nodes[i].m_nxt = k;
			  Nodes[k].mc = m;
			  Nodes[k].m_prv = i;
			  Nodes[k].weight = Alg->pt[op->Job->i][m];
			  i = k;
			}
		  }
		  else{
			Nodes[i].m_nxt = -1;
			LastmNode[m] = i;
			break;
		  }
		}
	  }
	  else{
		pind = pind->Next();
	  }
	}
  }

  for(j=0; j<nJobs; j++){
	SList::SNode *tund = V->headOfTau(j);
	while(tund){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)tund->Data();
	  i = op->i;
	  if(V->u[i] & 1){
		FirstjNode[j] = i;
		Nodes[i].j_prv = -1;
		while(true){
		  tund = tund->Next();
		  if(tund){
			op = (MOSP_Problem::SOperation *)tund->Data();
			k = op->i;
			if(V->u[k] & 1){
			  Nodes[i].j_nxt = k;
			  Nodes[k].j_prv = i;
			  i = k;
			}
		  }
		  else{
			Nodes[i].j_nxt = -1;
			LastjNode[j] = i;
			break;
		  }
		}
	  }
	  else tund = tund->Next();
	}
  }

	 #ifdef REPORT_NETWORK_RESULTS
		  V->Assert("Vector representation as passed to network");
		  Assert("Corresponding network representation based on the given vectors");
	 #endif

  return true;
}

void MOSP_Network::Reset()
{
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  memset(FirstjNode, -1, nJobs*sizeof(int));
  memset(LastjNode, -1, nJobs*sizeof(int));
  memset(FirstmNode, -1, nMCs*sizeof(int));
  memset(LastmNode, -1, nMCs*sizeof(int));

  Cmax = MAXINT;   // Just to notify that the network solution has not been
				   // constructed yet

  for(int i=0; i<nOps; i++) Nodes[i].Reset();
}

int MOSP_Network::sizeof_tau(int j)
{
  int n = 0;
  int nxt = FirstjNode[j];
  while(nxt != -1){
	 nxt = Nodes[nxt].j_nxt;
	 n++;
  }
  return n;
}

int MOSP_Network::sizeof_pi(int m)
{
  int n = 0;
  int nxt = FirstmNode[m];
  while(nxt != -1){
	nxt = Nodes[nxt].m_nxt;
	n++;
  }
  return n;
}

MOSP_Node *MOSP_Network::tauNode_at(int j, int x)
{
  int n = 1;
  int nxt = FirstjNode[j];
  while(n < x && nxt != -1){
	 nxt = Nodes[nxt].j_nxt;
	 n++;
  }
  if(nxt == -1) return NULL;
  return &Nodes[nxt];
}

MOSP_Node *MOSP_Network::piNode_at(int m, int x)
{
  int n = 1;
  int nxt = FirstmNode[m];
  while(n < x && nxt != -1){
	 nxt = Nodes[nxt].m_nxt;
	 n++;
  }
  if(nxt == -1) return NULL;
  return &Nodes[nxt];
}

int MOSP_Network::tau_pos(MOSP_Node *nd)
{
  MOSP_Node *v = nd;
  int p=1;
  while(v->j_prv != -1){
	v = &Nodes[v->j_prv];
	p++;
  }
  return p;
}

int MOSP_Network::pi_pos(MOSP_Node *nd)
{
  MOSP_Node *v = nd;
  int p=1;
  while(v->m_prv != -1){
	v = &Nodes[v->m_prv];
	p++;
  }
  return p;
}


bool MOSP_Network::isThereAPath(MOSP_Node *nd1, MOSP_Node *nd2)
{
  if(nd1->m_nxt == -1 && nd1->j_nxt == -1) return false;

  STree T((void *)nd1);
  MOSP_Node *cur = nd1;

  while(true){
	if(cur->j_nxt != -1){
	  if(&Nodes[cur->j_nxt] == nd2) return true;
	  T.AddChild((void *)&Nodes[cur->j_nxt]);
	  if(cur->m_nxt != -1){
		if(&Nodes[cur->m_nxt] == nd2) return true;
		T.AddLeftNode((void *)&Nodes[cur->m_nxt]);
	  }
	}
	else if(cur->m_nxt != -1){
	  if(&Nodes[cur->m_nxt] == nd2) return true;
	  T.AddChild((void *)&Nodes[cur->m_nxt]);
	}
	else{
	  STree::SNode *itrnd;
	  while(true){
		itrnd = T.MoveRight();
		if(!itrnd){
		  itrnd = T.MoveUp();
		  if(!itrnd) return false;
		}
		else break;
	  }
	}
	STree::SNode *curnd = T.GetCurrentNode();
	cur = (MOSP_Node *)curnd->Data();
  }
}

void MOSP_Network::alpha_omega_N1(MOSP_Problem::SOperation *v1,
					int &alpha, int &omega, int &apr_alpha, int &apr_omega)
{
  MOSP_Node *nd_v1 = &Nodes[v1->i];
  int l = sizeof_tau(v1->Job->i);

  if(nd_v1->m_prv == -1) alpha = apr_alpha = 1;
  else{
	alpha = tau_pos(nd_v1);
	apr_alpha = alpha;
	if(alpha != 1){
	  MOSP_Node *mprv = &Nodes[nd_v1->m_prv];
	  MOSP_Node *cur = nd_v1;

	  // Find exact value
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(isThereAPath(cur, mprv)) break;
		alpha--;
	  }

	  // Find approximate value
	  apr_alpha = l;
	  cur = &Nodes[LastjNode[v1->Job->i]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		apr_alpha--;
	  }
	}
  }

  if(nd_v1->m_nxt == -1) omega = apr_omega = l;
  else{
	omega = tau_pos(nd_v1);
	if(omega != l){
	  MOSP_Node *mnxt = &Nodes[nd_v1->m_nxt];
	  MOSP_Node *cur = nd_v1;

	  // Find exact value
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(isThereAPath(mnxt, cur)) break;
		omega++;
	  }

	  // Find approximate value
	  apr_omega = 1;
	  cur = &Nodes[FirstjNode[v1->Job->i]];
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		apr_omega++;
	  }
	}
	else apr_omega = l;
  }
}

int MOSP_Network::phi_N1(MOSP_Problem::SOperation *v1)
{
  int phi;

  MOSP_Node &v = Nodes[v1->i];
  phi =   ((v.m_prv == -1)? 0 : (Nodes[v.m_prv].estart + Nodes[v.m_prv].weight))
		+ v.weight + ((v.m_nxt == -1)? 0 : Nodes[v.m_nxt].tail);

  return phi;
}

void MOSP_Network::ApproxDeltas_N1(MOSP_Problem::SOperation *v1, int x1,
					   int &hat_delta, int &check_delta)
{
  MOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 == x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  MOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	MOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd) check_delta += jx1nd->tail;
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0;
	MOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd) hat_delta += (jx1nd->estart + jx1nd->weight);
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  MOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void MOSP_Network::EstDeltas_N1(MOSP_Problem::SOperation *v1, int x1,
					   int &hat_delta, int &check_delta)
{
  MOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 == x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  MOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	MOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate estimate of the nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  MOSP_Node *j_xnd = &Nodes[v.j_prv];
	  int gjv1nxt = (v.j_nxt != -1)? Nodes[v.j_nxt].tail : 0;
	  int gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
	  int g = j_xnd->weight + ((gjv1nxt > gmx1)? gjv1nxt : gmx1);
	  if(x1 < x - 1){
		for(int _x = x - 2; _x >= x1; _x--){
		  j_xnd = &Nodes[j_xnd->j_prv];
		  gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
		  g = j_xnd->weight + ((g > gmx1)? g : gmx1);
		}
	  }

	  check_delta += g;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0;
	MOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate estimated value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  MOSP_Node *j_xnd = &Nodes[v.j_nxt];
	  int etjv1prv = (v.j_prv != -1)? Nodes[v.j_prv].estart + Nodes[v.j_prv].weight : 0;
	  int etmx1 = (j_xnd->m_prv != -1)? Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight : 0;
	  int e = (etjv1prv > etmx1)? etjv1prv : etmx1;
	  if(x1 > x + 1){
		for(int _x = x + 2; _x <= x1; _x++){
		  int prvwt = j_xnd->weight;
		  j_xnd = &Nodes[j_xnd->j_nxt];
		  etmx1 = (j_xnd->m_prv != -1)? (Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight) : 0;
		  e = (e + prvwt > etmx1)? (e + prvwt) : etmx1;
		}
	  }
	  hat_delta += (e + jx1nd->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  MOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void MOSP_Network::ExactDeltas_N1(MOSP_Network *Net_, MOSP_Problem::SOperation *v1, int x1,
					   int &hat_delta, int &check_delta)
{
  MOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 <= x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	MOSP_Node *jx1nd = Net_->tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate  nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  check_delta += jx1nd->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0;
	MOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	MOSP_Node *jx1nd_ = Net_->tauNode_at(v1->Job->i, x1-1);
	if(jx1nd_){
	  // Get an accurate value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  hat_delta += (jx1nd_->estart + jx1nd->weight);
	}
	if(v.m_prv != -1){
	  MOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  MOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  MOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void MOSP_Network::alpha_omega_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2,
					int &alpha, int &omega, int &apr_alpha, int &apr_omega)
{
  MOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i == nd_v2->mc){ // if same machine
	int l = sizeof_pi(nd_v2->mc);

	if(nd_v2->j_prv == -1) alpha = apr_alpha = 1;
	else{
	  alpha = pi_pos(nd_v2);
	  apr_alpha = alpha;
	  if(alpha != 1){
		MOSP_Node *jprv = &Nodes[nd_v2->j_prv];
		MOSP_Node *cur = nd_v2;

		// Find exact value
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(isThereAPath(cur, jprv)) break;
		  alpha--;
		}

		// Find approximate value
		apr_alpha = l;
		cur = &Nodes[LastmNode[nd_v2->mc]];
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		}
	  }
	}

	if(nd_v2->j_nxt == -1) omega = apr_omega = l;
	else{
	  omega = pi_pos(nd_v2);
	  if(omega != l){
		MOSP_Node *jnxt = &Nodes[nd_v2->j_nxt];
		MOSP_Node *cur = nd_v2;

		// Find exact value
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(isThereAPath(jnxt, cur)) break;
		  omega++;
		}

		// Find approximate value
		apr_omega = 1;
		cur = &Nodes[FirstmNode[nd_v2->mc]];
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		}
	  }
	  else apr_omega = l;
	}
  }
  else{
	int l = sizeof_pi(mk2->i);

	if(nd_v2->j_prv == -1) alpha = apr_alpha = 1;
	else{
	  alpha = l + 1;
	  apr_alpha = alpha;
	  MOSP_Node *jprv = &Nodes[nd_v2->j_prv];
	  MOSP_Node *cur;

	  if(LastmNode[mk2->i] != -1){
	    // Find exact value
		cur = &Nodes[LastmNode[mk2->i]];
		do{
		  if(isThereAPath(cur, jprv)) break;
		  alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);

		// Find approximate value
		cur = &Nodes[LastmNode[mk2->i]];
		do{
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);
	  }
	}

	if(nd_v2->j_nxt == -1) omega = apr_omega = l + 1;
	else{
	  omega = apr_omega = 1;
	  MOSP_Node *jnxt = &Nodes[nd_v2->j_nxt];
	  MOSP_Node *cur;

	  if(FirstmNode[mk2->i] != -1){
		// Find exact value
		cur = &Nodes[FirstmNode[mk2->i]];
		do{
		  if(isThereAPath(jnxt, cur)) break;
		  omega++;
		  if(cur->m_nxt == -1) break;
		  cur = &Nodes[cur->m_nxt];
		}while(true);

		// Find approximate value
		cur = &Nodes[FirstmNode[mk2->i]];
		do{
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		  if(cur->m_nxt == -1) break;
		  cur = &Nodes[cur->m_nxt];
		}while(true);
	  }
	}
  }
}

int MOSP_Network::phi_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2)
{
  int phi;

  MOSP_Node &v = Nodes[v2->i];
  phi =   ((v.j_prv == -1)? 0 : (Nodes[v.j_prv].estart + Nodes[v.j_prv].weight))
		+ ((v.j_nxt == -1)? 0 : Nodes[v.j_nxt].tail);

  int d = Alg->pt[v2->Job->i][mk2->i];
  phi += d;

  return phi;
}

void MOSP_Network::ApproxDeltas_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2, int x2,
							int &hat_delta, int &check_delta)
{
  MOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0;
	check_delta = 0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  MOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

	int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  MOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	MOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 == x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < sizeof_pi(mk2->i)){
		MOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  MOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd) check_delta += mx2nd->tail;
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0;
	  MOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd) hat_delta += (mx2nd->estart + mx2nd->weight);
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < sizeof_pi(mk2->i)){
		MOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void MOSP_Network::EstDeltas_N2(MOSP_Problem::SOperation *v2, MOSP_Problem::SMC *mk2, int x2,
							int &hat_delta, int &check_delta)
{
  MOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0;
	check_delta = 0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  MOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

    int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  MOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	MOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 == x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < sizeof_pi(mk2->i)){
		MOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  MOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate estimate of the nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		MOSP_Node *m_xnd = &Nodes[v.m_prv];
		int gmv2nxt = (v.m_nxt != -1)? Nodes[v.m_nxt].tail : 0;
		int gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
		int g = m_xnd->weight + ((gmv2nxt > gjx2)? gmv2nxt : gjx2);
		if(x2 < x - 1){
		  for(int _x = x - 2; _x >= x2; _x--){
			m_xnd = &Nodes[m_xnd->m_prv];
			gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
			g = m_xnd->weight + ((g > gjx2)? g : gjx2);
		  }
		}

		check_delta += g;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0;
	  MOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate estimated value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
		MOSP_Node *m_xnd = &Nodes[v.m_nxt];
		int etmv2prv = (v.m_prv != -1)? Nodes[v.m_prv].estart + Nodes[v.m_prv].weight : 0;
		int etjx2 = (m_xnd->j_prv != -1)? Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight : 0;
		int e = (etmv2prv > etjx2)? etmv2prv : etjx2;
		if(x2 > x + 1){
		  for(int _x = x + 2; _x <= x2; _x++){
			int prvwt = m_xnd->weight;
			m_xnd = &Nodes[m_xnd->m_nxt];
			etjx2 = (m_xnd->j_prv != -1)? (Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight) : 0;
			e = (e + prvwt > etjx2)? (e + prvwt) : etjx2;
		  }
		}
		hat_delta += (e + mx2nd->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < sizeof_pi(mk2->i)){
		MOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void MOSP_Network::ExactDeltas_N2(MOSP_Network *Net_, MOSP_Problem::SOperation *v2,
					   MOSP_Problem::SMC *mk2, int x2, int &hat_delta, int &check_delta)
{
  MOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0;
	check_delta = 0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  MOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

	int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  MOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	MOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 <= x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  MOSP_Node *mx2nd = Net_->piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate  nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		check_delta += mx2nd->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0;
	  MOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  MOSP_Node *mx2nd_ = Net_->piNode_at(mk2->i, x2-1);
	  if(mx2nd_){
		// Get an accurate value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
		hat_delta += (mx2nd_->estart + mx2nd->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < sizeof_pi(mk2->i)){
		MOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void MOSP_Network::alpha_omega_B1(int j, int x1, int x2, int &alpha, int &omega,
								  int &apr_alpha, int &apr_omega)
{
  int l = sizeof_tau(j);

  alpha = apr_alpha = 1;
  omega = apr_omega = l - (x2 - x1);

  for(int x=x1; x <= x2; x++){  // of course x1 <= x2
	MOSP_Node *v = tauNode_at(j, x);
	int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	if(v->m_prv == -1 || x1 == 1) v_alpha = v_apr_alpha = 1;
	else{
	  v_alpha = v_apr_alpha = x1;
	  MOSP_Node *mprv = &Nodes[v->m_prv];
	  MOSP_Node *cur = tauNode_at(j, x1);

	  // Find exact value
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(isThereAPath(cur, mprv)) break;
		v_alpha--;
	  }

	  // Find approximate value
	  cur = tauNode_at(j, x1);  //&Nodes[LastjNode[j]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		v_apr_alpha--;
	  }
	}

	if(v->m_nxt == -1 || x2 == l) v_omega = v_apr_omega = l - (x2 - x1);
	else{
	  v_omega = v_apr_omega = x1;
	  MOSP_Node *mnxt = &Nodes[v->m_nxt];
	  MOSP_Node *cur = tauNode_at(j, x2);

	  // Find exact value
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(isThereAPath(mnxt, cur)) break;
		v_omega++;
	  }

	  // Find approximate value
	  cur = tauNode_at(j, x2);
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		v_apr_omega++;
	  }
	}

	if(v_alpha > alpha) alpha = v_alpha;
	if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	if(v_omega < omega) omega = v_omega;
	if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
  }
}

void MOSP_Network::alpha_omega_B2(int m1, int m2, int x1, int x2, int &alpha,
								  int &omega, int &apr_alpha, int &apr_omega)
{
  if(m1 == m2){ // if same machine
	int l = sizeof_pi(m1);

	alpha = apr_alpha = 1;
	omega = apr_omega = l - (x2 - x1);

	for(int x = x1; x <= x2; x++){
	  MOSP_Node *v = piNode_at(m1, x);
	  int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	  if(v->j_prv == -1 || x1 == 1) v_alpha = v_apr_alpha = 1;
	  else{
		v_alpha = v_apr_alpha = x1;
		MOSP_Node *jprv = &Nodes[v->j_prv];
		MOSP_Node *cur = piNode_at(m1, x1);

		// Find exact value
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(isThereAPath(cur, jprv)) break;
		  v_alpha--;
		}

		// Find approximate value
		cur = piNode_at(m1, x1);
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  v_apr_alpha--;
		}
	  }

	  if(v->j_nxt == -1 || x2 == l) v_omega = v_apr_omega = l - (x2 - x1);
	  else{
		v_omega = x1;
		MOSP_Node *jnxt = &Nodes[v->j_nxt];
		MOSP_Node *cur = piNode_at(m1, x2);

		// Find exact value
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(isThereAPath(jnxt, cur)) break;
		  v_omega++;
		}

		// Find approximate value
		v_apr_omega = x1;
		cur = piNode_at(m1, x2);
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  v_apr_omega++;
		}
	  }

	  if(v_alpha > alpha) alpha = v_alpha;
	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_omega < omega) omega = v_omega;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
  else{ // in case of different machines
	int l = sizeof_pi(m2);

	alpha = apr_alpha = 1;
	omega = apr_omega = l + 1;

	for(int x = x1; x <= x2; x++){
	  MOSP_Node *v = piNode_at(m1, x);
	  int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	  if(v->j_prv == -1) v_alpha = v_apr_alpha = 1;
	  else{
		v_alpha = v_apr_alpha = l + 1;
		MOSP_Node *jprv = &Nodes[v->j_prv];
		MOSP_Node *cur;

		if(LastmNode[m2] != -1){
		  // Find exact value
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(isThereAPath(cur, jprv)) break;
			v_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);

		  // Find approximate value
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(cur->estart + cur->weight < jprv->estart) break;
			v_apr_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);
		}
	  }

	  if(v->j_nxt == -1) v_omega = v_apr_omega = l + 1;
	  else{
		v_omega = v_apr_omega = 1;
		MOSP_Node *jnxt = &Nodes[v->j_nxt];
		MOSP_Node *cur;

		if(FirstmNode[m2] != -1){
		  // Find exact value
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(isThereAPath(jnxt, cur)) break;
			v_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);

		  // Find approximate value
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(jnxt->tail - jnxt->weight > cur->tail) break;
			v_apr_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);
		}
	  }

	  if(v_alpha > alpha) alpha = v_alpha;
	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_omega < omega) omega = v_omega;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
}

//------------------------------------------ Hill climbing neighborhood search

MOSP_NSTester::MOSP_NSTester(MOSP_Problem *P, char *N)
  : MOSP_Algorithm(P, N)
{
}

MOSP_NSTester::MOSP_NSTester(MOSP_Problem *P, MOSP_Solution *is)
  : MOSP_Algorithm(P, "Neighborhood Search Tester")
{
  iSol = is;
}

MOSP_NSTester::~MOSP_NSTester()
{
}

void MOSP_NSTester::AllocateMemory()
{
  MOSP_Algorithm::AllocateMemory();

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  dbgFile = fopen("Debug.txt", "a+");
	 #endif
}

void MOSP_NSTester::Initialize()
{
  if(iSol->Problem != Problem) return;

  // Copy tasks for the given initial solution to the current tasks
  for(int i=0; i<nOps; i++){
	MOSP_Solution::SchTask *gtsk = iSol->TaskAssociatedToOperation(O[i]);
	memcpy(tsk[i], gtsk, sizeof(MOSP_Solution::SchTask));
  }

	 #ifdef REPORT_NSTester_RESULTS
		  char buf[256];
		  sprintf(buf, "Initial solution as kept in NS Tester:\n\n");
		  fprintf(dbgFile, buf);
		  sprintf(buf, "\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fprintf(dbgFile, buf);
		  for(int i=0; i<nOps; i++){
			sprintf(buf, "\t%i\t%s\t%s\t%s\t%i\t%i\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), (int)(tsk[i]->StartTime),
						 (int)(tsk[i]->EndTime));
			fprintf(dbgFile, buf);
		  }
		  sprintf(buf, "\n\tCmax = %i\n\n", (int)Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN));
		  fprintf(dbgFile, buf);
		  fflush(dbgFile);
	 #endif
}

bool MOSP_NSTester::Stop()
{
  return true;
}

void MOSP_NSTester::Finalize()
{
  MOSP_Algorithm::Finalize();
}

void MOSP_NSTester::FreeMemory()
{
  MOSP_Algorithm::FreeMemory();

	 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fclose(dbgFile);
	 #endif
}

void MOSP_NSTester::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

bool MOSP_NSTester::GetN1Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int j,
							   int x, SList &N1E, SList &N1L, SList &N1S, int &_x, int &x_)
{
  if(Vec->Alg != this || Net->Alg != this) return false;

  N1E.Clear();
  N1L.Clear();
  N1S.Clear();

  SList &tauj = Vec->tau[j];
  SList tauj_rho(tauj);

  MOSP_Problem::SOperation *opx = (MOSP_Problem::SOperation *)tauj[x-1];
  if(Vec->u[opx->i] != 2) return false;

  MOSP_Problem::SOperation *op;
  SList::SNode *_nd = tauj_rho.head();
  for(SList::SNode *nd = tauj.head(); nd; nd = nd->Next()){
	op = (MOSP_Problem::SOperation *)nd->Data();
	if(Vec->u[op->i] == 3){
	  bool inE, inL;
	  inE = inL = false;
	  if(Net->Nodes[op->i].estart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].estart
		 && Net->Nodes[op->i].lstart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].lstart){
		   inE = true;
		   N1E.Add((void *)op);
	  }
	  if(Net->Nodes[opx->i].estart + Net->Nodes[opx->i].weight < Net->Nodes[op->i].estart
		 && Net->Nodes[opx->i].lstart + Net->Nodes[opx->i].weight < Net->Nodes[op->i].lstart){
		   inL = true;
		   N1L.Add((void *)op);
	  }
	  if(!inE & !inL) N1S.Add((void *)op);
	  _nd = _nd->Next();
	}
	else{
	  SList::SNode *rem_nd = _nd;
	  _nd = _nd->Next();
	  tauj_rho.Remove(rem_nd);
	}
  }

  if(N1E.Count() == 0) _x = 1;
  else _x = tauj_rho.IndexOf(N1E.tail()->Data()) + 2;

  if(N1L.Count() == 0) x_ = tauj_rho.Count() + 1;
  else x_ = tauj_rho.IndexOf(N1L.head()->Data()) + 1;

  return true;
}

bool MOSP_NSTester::GetN2Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int m,
							   int x, SList &N2E, SList &N2L, SList &N2S, int &_x, int &x_)
{
  if(Vec->Alg != this || Net->Alg != this) return false;

  N2E.Clear();
  N2L.Clear();
  N2S.Clear();

  SList &pim = Vec->pi[m];
  SList pim_rho(pim);

  MOSP_Problem::SOperation *opx = (MOSP_Problem::SOperation *)pim[x-1];
  if(Vec->u[opx->i] != 1) return false;

  MOSP_Problem::SOperation *op;
  SList::SNode *_nd = pim_rho.head();
  for(SList::SNode *nd = pim.head(); nd; nd = nd->Next()){
	op = (MOSP_Problem::SOperation *)nd->Data();
	if(Vec->u[op->i] == 3){
	  bool inE, inL;
	  inE = inL = false;
	  if(Net->Nodes[op->i].estart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].estart
		 && Net->Nodes[op->i].lstart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].lstart){
		   inE = true;
		   N2E.Add((void *)op);
	  }
	  if(Net->Nodes[opx->i].estart /*+ Net->Nodes[opx->i].weight*/ < Net->Nodes[op->i].estart
		 && Net->Nodes[opx->i].lstart /*+ Net->Nodes[opx->i].weight*/ < Net->Nodes[op->i].lstart){
		   inL = true;
		   N2L.Add((void *)op);
	  }
	  if(!inE & !inL) N2S.Add((void *)op);
	  _nd = _nd->Next();
	}
	else{
	  SList::SNode *rem_nd = _nd;
	  _nd = _nd->Next();
	  pim_rho.Remove(rem_nd);
	}
  }

  if(N2E.Count() == 0) _x = 1;
  else _x = pim_rho.IndexOf(N2E.tail()->Data()) + 2;

  if(N2L.Count() == 0) x_ = pim_rho.Count() + 1;
  else x_ = pim_rho.IndexOf(N2L.head()->Data()) + 1;

  return true;
}

bool MOSP_NSTester::GetN3Sets(MOSP_VectorSol *Vec, MOSP_Network *Net, int from_m, int to_m,
							   int x, SList &N3E, SList &N3L, SList &N3S, int &_x, int &x_)
{
  if(Vec->Alg != this || Net->Alg != this) return false;

  N3E.Clear();
  N3L.Clear();
  N3S.Clear();

  SList &pim = Vec->pi[from_m];
  SList &pitom = Vec->pi[to_m];

  MOSP_Problem::SOperation *opx = (MOSP_Problem::SOperation *)pim[x-1];
  if(Vec->u[opx->i] != 1) return false;

  MOSP_Problem::SOperation *op;
  for(SList::SNode *nd = pitom.head(); nd; nd = nd->Next()){
	op = (MOSP_Problem::SOperation *)nd->Data();
	bool inE, inL;
	inE = inL = false;
	if(Net->Nodes[op->i].estart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].estart
	   && Net->Nodes[op->i].lstart + Net->Nodes[op->i].weight < Net->Nodes[opx->i].lstart){
		inE = true;
		N3E.Add((void *)op);
	}
	if(Net->Nodes[opx->i].estart /*+ Net->Nodes[opx->i].weight*/ < Net->Nodes[op->i].estart
	   && Net->Nodes[opx->i].lstart /*+ Net->Nodes[opx->i].weight*/ < Net->Nodes[op->i].lstart){
		inL = true;
		N3L.Add((void *)op);
	}
	if(!inE & !inL) N3S.Add((void *)op);
  }

  if(N3E.Count() == 0) _x = 1;
  else _x = pitom.IndexOf(N3E.tail()->Data()) + 2;

  if(N3L.Count() == 0) x_ = pitom.Count() + 1;
  else x_ = pitom.IndexOf(N3L.head()->Data()) + 1;

  return true;
}

//-----------------------------------------------------------------------------
#pragma package(smart_init)
