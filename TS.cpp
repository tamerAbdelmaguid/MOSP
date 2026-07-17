//---------------------------------------------------------------------------


#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TS.h"
#include "Random.h"
#include "FormTSCalcProgress.h"
#include "FormTS.h"
#include "FileSystem.h"

//#define REPORT_INTERMEDIATE_RESULTS
//#define SAVE_INITIAL_SOLUTION
#define USE_FIRST_LAST_MOVES
#define USE_MCT_JOB_SELECTION
#define USE_APPROXIMATE_LB

#ifdef REPORT_INTERMEDIATE_RESULTS
FILE *dbgFile;
#endif

int max(int i1, int i2)
{
  if(i1 >= i2) return i1;
  return i2;
}

//---------------------------------------- Network representation of solutions

MOSP_TS_NetSolution::MOSP_TS_NetSolution(MOSP_TS *ts)
{
  TS = ts;
  int nJobs = ts->nJobs;
  int nMCs = ts->nMCs;
  int nOps = ts->nOps;

  Nodes = new MOSP_TS_Node[nOps];
  FirstjNode = new int[nJobs];    memset(FirstjNode, -1, nJobs*sizeof(int));
  LastjNode = new int[nJobs];     memset(LastjNode, -1, nJobs*sizeof(int));
  FirstmNode = new int[nMCs];     memset(FirstmNode, -1, nMCs*sizeof(int));
  LastmNode = new int[nMCs];      memset(LastmNode, -1, nMCs*sizeof(int));

  Cmax = MAXINT;   // Just to notify that the network solution has not been
				   // constructed yet
}

MOSP_TS_NetSolution::~MOSP_TS_NetSolution()
{
  delete [] Nodes;
  delete [] FirstjNode;
  delete [] LastjNode;
  delete [] FirstmNode;
  delete [] LastmNode;
}

void MOSP_TS_NetSolution::Copy(MOSP_TS_NetSolution *sol)
{     // Assuming no change in the problem (nJobs, nMCs, and nOps)
  if(TS != sol->TS) return;
  memcpy(Nodes, sol->Nodes, TS->nOps * sizeof(MOSP_TS_Node));
  memcpy(FirstjNode, sol->FirstjNode, TS->nJobs * sizeof(int));
  memcpy(LastjNode, sol->LastjNode, TS->nJobs * sizeof(int));
  memcpy(FirstmNode, sol->FirstmNode, TS->nMCs * sizeof(int));
  memcpy(LastmNode, sol->LastmNode, TS->nMCs * sizeof(int));
  Cmax = sol->Cmax;
}

bool MOSP_TS_NetSolution::InterpretNetwork()
{
  int i, j, m;

  for(i=0; i<TS->nOps; i++){
	Nodes[i].head = -1;
	Nodes[i].tail = -1;
  }

  int nIncompleteMCs = TS->nMCs;
  int nIncompleteJobs = TS->nJobs;
  for(m=0; m<TS->nMCs; m++){
	TS->MCrt[m] = TS->M[m]->ReadyTime;
	TS->mnxt[m] = FirstmNode[m];
	if(TS->mnxt[m] == -1) nIncompleteMCs--;
  }
  for(j=0; j<TS->nJobs; j++){
	TS->Jnft[j] = TS->J[j]->ReleaseTime;
	TS->jnxt[j] = FirstjNode[j];
	if(TS->jnxt[j] == -1) nIncompleteJobs--;
  }

  Cmax = 0;
  bool bFeasible;
  while(true){
	bFeasible = ((nIncompleteMCs == 0 && nIncompleteJobs !=0) ||
				 (nIncompleteMCs != 0 && nIncompleteJobs ==0))? false : true;

	if(!bFeasible) break;
	if(nIncompleteMCs == 0 && nIncompleteJobs ==0) break;

	for(m=0; m<TS->nMCs; m++){
	  i = TS->mnxt[m];
	  if(i==-1) continue;
	  j = TS->O[i]->Job->i;
	  if(TS->jnxt[j] == i){
		Nodes[i].head = (TS->MCrt[m] > TS->Jnft[j])? TS->MCrt[m] : TS->Jnft[j];
		TS->MCrt[m] = TS->Jnft[j] = Nodes[i].head + Nodes[i].weight;
		if(Cmax < TS->MCrt[m]) Cmax = TS->MCrt[m];
		TS->mnxt[m] = Nodes[i].m_nxt;
		TS->jnxt[j] = Nodes[i].j_nxt;
		if(TS->mnxt[m] == -1) nIncompleteMCs--;
		if(TS->jnxt[j] == -1) nIncompleteJobs--;
		break;
	  }
	}
  }

  if(!bFeasible){
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n\t------------ Infeasible slution is encountered!!!");
	 #endif

	Cmax = MAXINT;
	return false;
  }

  for(i=0; i<TS->nOps; i++){
	if(Nodes[i].head == -1){
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n\t------------ Incomplete scheduled is encountered!!!");
	 #endif

	  Cmax = MAXINT;
	  return false;   // incomplete schedule
	}
  }

  // Now, we define the tail and criticality conditions for nodes
  int rem = TS->nOps; // number of remaining operations which still need to defined their tails
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = 0, imaxEFT;
	for(i = 0; i < TS->nOps; i++){
	  if(Nodes[i].tail != -1) continue;  // tail is already defined
	  int eft = Nodes[i].head + Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	}

	int mlft, jlft;
	if(Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = Nodes[Nodes[imaxEFT].m_nxt].tail;
	if(Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = Nodes[Nodes[imaxEFT].j_nxt].tail;
	Nodes[imaxEFT].tail = ((mlft < jlft)? mlft : jlft) - Nodes[imaxEFT].weight;
	if(Nodes[imaxEFT].head == Nodes[imaxEFT].tail) Nodes[imaxEFT].bCritical = true;
	else Nodes[imaxEFT].bCritical = false;

	rem--;
  }

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  char buf[256];
		  fprintf(dbgFile, "\n\nNetwork representation of modified solution:\n\n");
		  fprintf(dbgFile, "\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\thead\ttail\tCritical\n");
		  for(i=0; i<TS->nOps; i++){
			sprintf(buf, "\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%c\n",
						 i, TS->O[i]->ID(), Nodes[i].j_prv, Nodes[i].j_nxt,
						 TS->M[Nodes[i].mc]->ID(), Nodes[i].m_prv,
						 Nodes[i].m_nxt, Nodes[i].head,
						 Nodes[i].tail, ((Nodes[i].bCritical)? '*' : ' '));
			fprintf(dbgFile, buf);
		  }
		  sprintf(buf, "\n\t\tCmax = %i\n\n", Cmax);
		  fprintf(dbgFile, buf);
	 #endif

  return bFeasible;
}

bool MOSP_TS_NetSolution::Swap_m(int m, int n1, int n2)
{
  // First check if n1 is in machine m's processing sequence or not
  int i = FirstmNode[m];
  while(i != -1 && i != n1) i = Nodes[i].m_nxt;   // i = n1;
  if(i == -1) return false;  // n1 is not in m's processing sequence

  if(Nodes[n1].m_nxt != n2 && Nodes[n1].m_prv != n2) return false;
						// the two nodes must be consecutive; otherwise, there is
						// no arc connecting them!!!

  if(Nodes[n1].m_prv == n2){
	int n = n1;
	n1 = n2;
	n2 = n;
  }

  //  here i = n1 and j = n2
  int PJi = Nodes[n1].j_prv;
  int SJi = Nodes[n1].j_nxt;
  int PJj = Nodes[n2].j_prv;
  int SJj = Nodes[n2].j_nxt;
  int PMi = Nodes[n1].m_prv;
  int SMj = Nodes[n2].m_nxt;

  int ej = max( (PJj != -1)? Nodes[PJj].head + Nodes[PJj].weight : 0,
				(PMi != -1)? Nodes[PMi].head + Nodes[PMi].weight : 0);
  int ei = max( (PJi != -1)? Nodes[PJi].head + Nodes[PJi].weight : 0,
				ej + Nodes[n2].weight);
  int li = max( (SJi != -1)? Cmax - Nodes[SJi].tail : 0,
				(SMj != -1)? Cmax - Nodes[SMj].tail : 0);
  int lj = max( (SJj != -1)? Cmax - Nodes[SJj].tail : 0, li + Nodes[n1].weight);
  LB = max(ei + Nodes[n1].weight + li, ej + Nodes[n2].weight + lj);

  int prv = Nodes[n1].m_prv;
  int nxt = Nodes[n2].m_nxt;
  if(prv == -1){
	Nodes[n2].m_prv = -1;
	Nodes[n1].m_nxt = Nodes[n2].m_nxt;
	Nodes[n2].m_nxt = n1;
	Nodes[n1].m_prv = n2;
	FirstmNode[m] = n2;
	if(Nodes[n1].m_nxt == -1) LastmNode[m] = n1;
	else Nodes[nxt].m_prv = n1;
  }
  else{
	Nodes[prv].m_nxt = n2;
	Nodes[n2].m_prv = prv;
	Nodes[n1].m_nxt = Nodes[n2].m_nxt;
	Nodes[n2].m_nxt = n1;
	Nodes[n1].m_prv = n2;
	if(Nodes[n1].m_nxt == -1) LastmNode[m] = n1;
	else Nodes[nxt].m_prv = n1;
  }

  return true;
}

bool MOSP_TS_NetSolution::Swap_j(int j, int n1, int n2)
{
  // First check if n1 is in job j's route or not
  int i = FirstjNode[j];
  while(i != -1 && i != n1) i = Nodes[i].j_nxt;   // i = n1;
  if(i == -1) return false;  // n1 is not in j's route

  if(Nodes[n1].j_nxt != n2 && Nodes[n1].j_prv != n2) return false;
						// the two nodes must be consecutive; otherwise, there is
						// no arc connecting them!!!

  if(Nodes[n1].j_prv == n2){
	int n = n1;
	n1 = n2;
	n2 = n;
  }

  //  here i = n1 and j = n2
  int PJi = Nodes[n1].m_prv;
  int SJi = Nodes[n1].m_nxt;
  int PJj = Nodes[n2].m_prv;
  int SJj = Nodes[n2].m_nxt;
  int PMi = Nodes[n1].j_prv;
  int SMj = Nodes[n2].j_nxt;

  int ej = max( (PJj != -1)? Nodes[PJj].head + Nodes[PJj].weight : 0,
				(PMi != -1)? Nodes[PMi].head + Nodes[PMi].weight : 0);
  int ei = max( (PJi != -1)? Nodes[PJi].head + Nodes[PJi].weight : 0,
				ej + Nodes[n2].weight);
  int li = max( (SJi != -1)? Cmax - Nodes[SJi].tail : 0,
				(SMj != -1)? Cmax - Nodes[SMj].tail : 0);
  int lj = max( (SJj != -1)? Cmax - Nodes[SJj].tail : 0, li + Nodes[n1].weight);
  LB = max(ei + Nodes[n1].weight + li, ej + Nodes[n2].weight + lj);

  int prv = Nodes[n1].j_prv;
  int nxt = Nodes[n2].j_nxt;
  if(prv == -1){
	Nodes[n2].j_prv = -1;
	Nodes[n1].j_nxt = Nodes[n2].j_nxt;
	Nodes[n2].j_nxt = n1;
	Nodes[n1].j_prv = n2;
	FirstjNode[j] = n2;
	if(Nodes[n1].j_nxt == -1) LastjNode[j] = n1;
	else Nodes[nxt].j_prv = n1;
  }
  else{
	Nodes[prv].j_nxt = n2;
	Nodes[n2].j_prv = prv;
	Nodes[n1].j_nxt = Nodes[n2].j_nxt;
	Nodes[n2].j_nxt = n1;
	Nodes[n1].j_prv = n2;
	if(Nodes[n1].j_nxt == -1) LastjNode[j] = n1;
	else Nodes[nxt].j_prv = n1;
  }

  return true;
}

int MOSP_TS_NetSolution::GetCPath(int *pth)
{
  int m, i, l=0;  // There should be at least on critical operation

  // Find a first critical node
  for(i=0; i<TS->nOps; i++){
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
		if(Nodes[jnxt].head <= Nodes[mnxt].head){
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

//------------------------------------------------------ Tabu Search Algorithm

MOSP_TS::MOSP_TS(MOSP_Problem *P, char *N)
  : MOSP_Algorithm(P, N)
{
}

MOSP_TS::MOSP_TS(MOSP_Problem *P, int ts, long nitr, long nimpitr, float maxt)
  : MOSP_Algorithm(P, "Tabu search"), TabuSize(ts), nIterations(nitr),
    MaxTime(maxt), nImpIterations(nimpitr)
{
}

MOSP_TS::~MOSP_TS()
{
}

void MOSP_TS::AllocateMemory()
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

  jnxt = new int[nJobs];
  mnxt = new int[nMCs];

  S0 = new MOSP_TS_NetSolution(this);
  S = new MOSP_TS_NetSolution(this);
  Sbest = new MOSP_TS_NetSolution(this);
  Stmp = new MOSP_TS_NetSolution(this);

  Tabus = new MOSP_Tabu[TabuSize];

  CPath = new int[nOps];

  bTerminate = false;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  dbgFile = fopen("Debug.txt", "a+");
	 #endif
}

void MOSP_TS::hPSOTS_AllocateMemory()
{
  if(Solution) delete Solution;
  Solution = new MOSP_Solution(Problem);
  AllocateArrays();

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

  jnxt = new int[nJobs];
  mnxt = new int[nMCs];

  S0 = new MOSP_TS_NetSolution(this);
  S = new MOSP_TS_NetSolution(this);
  Sbest = new MOSP_TS_NetSolution(this);
  Stmp = new MOSP_TS_NetSolution(this);

  Tabus = new MOSP_Tabu[TabuSize];

  CPath = new int[nOps];

  bTerminate = false;
}

void MOSP_TS::Initialize()
{
  int i, j, m;
  int t;   // Current scheduling time
  int Cmax = 0;

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

	// Define the set of ready machines at time t
	for(m=0; m<nMCs; m++){
	  bMCready[m] = (MCrt[m] == t && !bMCdone[m])? true : false;
	}

	// For each machine ready in time t, select a job
	//
	for(m=0; m<nMCs; m++){
		if(!bMCready[m]) continue;

		// Check to see if there is any job that needs to be scheduled on machine m
		//   preference is made first to jobs that has nearest finish time less than
		//   or equal t. If there isn't any job with that condition, all schedulable
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

		// Now select a job from all schedulable jobs based on the minimum completion time
		//
#ifdef USE_MCT_JOB_SELECTION
		int Mr;  // The measure based upon which a job will be selected
#endif
		for(j=0; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  SelJob[m] = j;
#ifdef USE_MCT_JOB_SELECTION
		  Mr = Jnft[j] + pt[j][m];
#endif
		  break;
		}
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
#ifdef USE_MCT_JOB_SELECTION
		  if(Jnft[j] + pt[j][m] < Mr){
			SelJob[m] = j;
			Mr = Jnft[j] + pt[j][m];
		  }
#else
		  if(flip(0.5)) SelJob[m] = j;   // Here, job selection is done randomly
#endif
		}
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections randomly
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
		if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  // Select the machine that results in lower completion time
		  //if(pt[j][m] <= pt[j][m2]) SelMC = m;
		  //else SelMC = m2;

		  SelMC = (flip(0.5))? m : m2;    // Select a machine randomly

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
	  if(Jnft[j] > Cmax) Cmax = Jnft[j];  // <--------------------- Update Cmax
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // define network parameters for the initial solution
	  S0->Nodes[i].i = i;   // index will be needed during the neighborhood search
	  S0->Nodes[i].mc = m;  // index of assigned machine to the operation with node i

	  if(S0->FirstjNode[j] == -1) S0->FirstjNode[j] = i;
	  if(S0->FirstmNode[m] == -1) S0->FirstmNode[m] = i;

	  int i2 = S0->LastmNode[m];
	  S0->Nodes[i].m_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].m_nxt = i;
	  S0->Nodes[i].m_nxt = -1;
	  S0->LastmNode[m] = i;

	  i2 = S0->LastjNode[j];
	  S0->Nodes[i].j_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].j_nxt = i;
	  S0->Nodes[i].j_nxt = -1;
	  S0->LastjNode[j] = i;

	  S0->Nodes[i].weight = pt[j][m];
	  S0->Nodes[i].head = tsk[i]->StartTime;
	  S0->Nodes[i].tail = -1;  // means undefined to be defined later

	  // Now job j does not need any machine in the processing center p
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
  S0->Cmax = Cmax;

  // Define tails for network nodes and if they are critical or not
  int rem = nOps; // number of remaining operations which still need to defined their tails
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = 0, imaxEFT;
	for(i = 0; i<nOps; i++){
	  if(S0->Nodes[i].tail != -1) continue;  // tail is already defined
	  int eft = S0->Nodes[i].head + S0->Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	}

	int mlft, jlft;
	if(S0->Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = S0->Nodes[S0->Nodes[imaxEFT].m_nxt].tail;
	if(S0->Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = S0->Nodes[S0->Nodes[imaxEFT].j_nxt].tail;
	S0->Nodes[imaxEFT].tail = ((mlft < jlft)? mlft : jlft) - S0->Nodes[imaxEFT].weight;
	if(S0->Nodes[imaxEFT].head == S0->Nodes[imaxEFT].tail) S0->Nodes[imaxEFT].bCritical = true;
	else S0->Nodes[imaxEFT].bCritical = false;

	rem--;
  }

   // -------------------------- Evaluate the lower bound for the given problem
   // ---- The first lower bound is based on Matta's work in which we use
   //      the minimum processing time on a given center on all its machines
   //      as the processing time in the case of proportionate MOSP
   //
   LB = 0;
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	 float Lk = pc->MCs.Count();
	 float N=0; // = number of jobs that need to be done on pc
	 for(j=0; j<nJobs; j++){
	   for(i=Ja[j]; i<=Jz[j]; i++){
		 if(O[i]->WC == pc){
		   N++;
		   break;
		 }
	   }
	 }
	 float lb = N / Lk;
	 int clb = lb;
	 if(clb < lb) clb++;

	 int pk = MAXINT; // Start with a large value for pk to find the minimum
	 int k = pc->i;   // processing time among machines that belong to the center k
	 for(j=0; j<nJobs; j++){
	   for(m=Wa[k]; m<=Wz[k]; m++){
		 if(pt[j][m]>0 && pk > pt[j][m]) pk = pt[j][m];
	   }
	 }

	 if(LB < clb * pk) LB = clb * pk;
	 pcnd = pcnd->Next();
   }

   // ---- The second lower bound is based on the maximum of each job's minimum
   //      completion time. For each job, the minimum time needed to complete
   //      it on its processing centers is evaluated. Then the maximum of these
   //      is the LB.
   //
   int jlb = 0;
   for(j=0; j<nJobs; j++){
	 int total_min_pt = 0;
	 for(i=Ja[j]; i<=Jz[j]; i++){
	   int k = O[i]->WC->i;
	   int minpt = MAXINT;
	   for(m=Wa[k]; m<=Wz[k]; m++){
		 if(minpt > pt[j][m]) minpt = pt[j][m];
	   }
	   total_min_pt += minpt;
	 }
	 if(total_min_pt > jlb) jlb = total_min_pt;
   }

   if(LB < jlb) LB = jlb;


  // Step 1: Start with an empty tabu list and let S = S_best = S_0, Cmax = makespan of S
  //         and itr = itr_imp = 0
  //
  nTabus = TabuPntr = 0;
  S->Copy(S0);
  Sbest->Copy(S0);
  itr = itr_imp = 0;

  ComputationalTime = 0;
  bInterrupt = false;

  // Step 2: If LB = Cmax then return S_best
  if(Cmax == LB){
	bTerminate = true;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n------------------------- Cmax = LB -----> Optimal solution found");
	 #endif
  }

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  char buf[256];
		  sprintf(buf, "Initial solution:\n\n");
		  fprintf(dbgFile, buf);
		  sprintf(buf, "\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fprintf(dbgFile, buf);
		  for(i=0; i<nOps; i++){
			sprintf(buf, "\t%i\t%s\t%s\t%s\t%i\t%i\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), (int)(tsk[i]->StartTime),
						 (int)(tsk[i]->EndTime));
			fprintf(dbgFile, buf);
		  }
		  sprintf(buf, "\n\tCmax = %i", Cmax);
		  fprintf(dbgFile, buf);
		  sprintf(buf, "\n\tLB = %i", LB);
		  fprintf(dbgFile, buf);

		  fprintf(dbgFile, "\n\nNetwork representation:\n\n");
		  fprintf(dbgFile, "\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\thead\ttail\tCritical\n");
		  for(i=0; i<nOps; i++){
			sprintf(buf, "\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%c\n",
						 i, O[i]->ID(), S->Nodes[i].j_prv, S->Nodes[i].j_nxt,
						 M[S->Nodes[i].mc]->ID(), S->Nodes[i].m_prv,
						 S->Nodes[i].m_nxt, S->Nodes[i].head,
						 S->Nodes[i].tail, ((S->Nodes[i].bCritical)? '*' : ' '));
			fprintf(dbgFile, buf);
		  }
		  fprintf(dbgFile, "\n\n");
	 #endif

	 #ifdef SAVE_INITIAL_SOLUTION
		  MOSP_FIO_RESULT Result;
	      SaveSolution(Problem, Solution, "TSSol0.mosp.sol", Result);
	 #endif

  frmTSCalcProgress->ProgressBar->Max = nIterations;
  frmTSCalcProgress->ProgressBar->Position = 0;
  frmTSCalcProgress->lblIter->Caption = "0";
  frmTSCalcProgress->lblLB->Caption = IntToStr(LB);
  frmTSCalcProgress->lblInitialCmax->Caption = IntToStr(Cmax);
  frmTSCalcProgress->lblCurrentCmax->Caption = IntToStr(Cmax);
  frmTSCalcProgress->lblBestCmax->Caption = IntToStr(Cmax);
}

void MOSP_TS::Improve()
{
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  char buf[256];
	 #endif

  // Step 3: based on the current network representation by Nodes, determine
  //         a critical path
  int cpl = S->GetCPath(CPath);
  if(cpl <= 0){   //<---- an error occured
	bTerminate = true;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\t\t------- An error encountered in determining a critical path ---------\n\n");
		  fprintf(dbgFile, buf);
	 #endif

	return;
  }
  if(cpl == 1){ // only one operation in the critical path, means that no improvement is possible
	bTerminate = true;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\t\t------- Only one operation in the critical path ---------\n\n");
		  fprintf(dbgFile, buf);
	 #endif

	return;
  }

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\nCritical path found: length = %i\n\t", cpl);
		  fprintf(dbgFile, buf);
		  for(int i=0; i<cpl; i++){
			sprintf(buf, "%s-", O[CPath[i]]->ID());
			fprintf(dbgFile, buf);
		  }
		  fprintf(dbgFile, "\n\n");
	 #endif


  // Step 4: Divide the critical path into machine and job blocks
  // Step 5: Based on the blocks, determine all possible moves and put them
  //         in set Moves
  // Step 6: For every move in the set Moves, determine the corresponding solution
  //         and its makespan
  // Step 7: If a move is found to be in the tabu list, then remove it from
  //         the set Moves, unless its makespan is found to be less than
  //         the current best makespan
  // Step 8: If the set of Moves is empty, stop and return Sbest
  // Step 9: Select the move with the best makespan form the set of Moves and
  //         update the current schedule accordingly.

  int bs, be;           // current block's start and end indexes in CPath
  int _u, _v;           // best move (u, v)
#ifdef USE_APPROXIMATE_LB
  int _LB = MAXINT;     // Start with a large value for the best lowerbound
#else
  int _Cmax = MAXINT;   // Start with a large value for Cmax as we want to get the minimum
#endif
  _u = _v = -1;
  int _j, _m;  // indexes of job and machine at which the best move is found
  _j = _m = -1;
  int cBestMove;  // = 0 if no best move found
				  // = 1 if best move found on machine arc reversal
				  // = 2 if best move found on job arc reversal
  cBestMove = 0;

  // First, loop through machine blocks in the critical path
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n- Machine blocks:\n\t");
		  fprintf(dbgFile, buf);
	 #endif

  int m;
  bs = 0;
  do{
	// find the next block end
	m = S->Nodes[CPath[bs]].mc;
	for(be=bs+1; be<cpl; be++){
	  if(m != S->Nodes[CPath[be]].mc) break;
	} // Now the block starts at bs and ends at be-1

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\n\tBlock: ");
		  fprintf(dbgFile, buf);
		  for(int i=bs; i<be; i++){
			sprintf(buf, "%s-", O[CPath[i]]->ID());
			fprintf(dbgFile, buf);
		  }
	 #endif

	if(be - bs == cpl){  // The critical path is composed of only one block
						 // --> a local optimal solution is found,
						 // i.e. no better solution can be obtained for the current
						 // machine selection decisions --> STOP!

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\t---- Critical path contains only one block -> STOP!\n");
		  fprintf(dbgFile, buf);
	 #endif

	  bTerminate = true;
	  return;
	}

    if(be - bs == 1){ // The block contains only one operation (node) --> do nothing
	  bs++;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\t---- Block contains only one operation -> do nothing\n");
		  fprintf(dbgFile, buf);
	 #endif

	}
	else{
#ifdef USE_FIRST_LAST_MOVES
	  if(bs == 0){   // the first block
		#ifdef REPORT_INTERMEDIATE_RESULTS
			  sprintf(buf, "\t\t- First block -\n\n");
			  fprintf(dbgFile, buf);
			  sprintf(buf, "\n\t\tExchange %s and %s\n\n", O[CPath[be-2]]->ID(), O[CPath[be-1]]->ID());
			  fprintf(dbgFile, buf);
		#endif

		// Exchange the last two nodes and evaluate the resultant makespan
		Stmp->Copy(S);
		if(Stmp->Swap_m(m, CPath[be-2], CPath[be-1])){
#ifdef USE_APPROXIMATE_LB
		  if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			if(Stmp->LB < _LB){
			  _u = CPath[be-2];
			  _v = CPath[be-1];
			  _LB = Stmp->LB;
			  _m = m;
			  cBestMove = 1;
			}
		  }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[be-2];
				_v = CPath[be-1];
				_Cmax = Stmp->Cmax;
				_m = m;
				cBestMove = 1;
			  }
			}
		  }
#endif
		}
	  }
	  else if(be != cpl){ // a middle block
#endif
		#ifdef REPORT_INTERMEDIATE_RESULTS
#ifdef USE_FIRST_LAST_MOVES
			  sprintf(buf, "\t\t- Middle block -\n\n");
			  fprintf(dbgFile, buf);
#endif
			  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[bs]]->ID(), O[CPath[bs+1]]->ID());
			  fprintf(dbgFile, buf);
		#endif

		// Exchange the first two nodes and evaluate the resultant makespan
		Stmp->Copy(S);

		if(Stmp->Swap_m(m, CPath[bs], CPath[bs+1])){
#ifdef USE_APPROXIMATE_LB
		  if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			if(Stmp->LB < _LB){
			  _u = CPath[bs];
			  _v = CPath[bs+1];
			  _LB = Stmp->LB;
			  _m = m;
			  cBestMove = 1;
			}
          }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[bs];
				_v = CPath[bs+1];
				_Cmax = Stmp->Cmax;
				_m = m;
				cBestMove = 1;
			  }
			}
		  }
#endif
		}
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		else {
		  sprintf(buf, "\t-- exchange failed");
		  fprintf(dbgFile, buf);
		}
	 #endif

		// Exchange the last two nodes and evaluate the resultant makespan
		if(be != bs+2){  // don't repeat the same exchange done previously!
		  // Exchange the last two nodes and evaluate the resultant makespan
		  Stmp->Copy(S);

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[be-2]]->ID(), O[CPath[be-1]]->ID());
		  fprintf(dbgFile, buf);
	 #endif

		  if(Stmp->Swap_m(m, CPath[be-2], CPath[be-1])){
#ifdef USE_APPROXIMATE_LB
			if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			  if(Stmp->LB < _LB){
				_u = CPath[be-2];
				_v = CPath[be-1];
				_LB = Stmp->LB;
				_m = m;
				cBestMove = 1;
			  }
			}
#else
			if(Stmp->InterpretNetwork()){
			  if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
				if(Stmp->Cmax < _Cmax){
				  _u = CPath[be-2];
				  _v = CPath[be-1];
				  _Cmax = Stmp->Cmax;
				  _m = m;
				  cBestMove = 1;
				}
			  }
			}
#endif
		  }
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  else {
			sprintf(buf, "\t-- exchange failed");
			fprintf(dbgFile, buf);
		  }
	 #endif
		}
#ifdef USE_FIRST_LAST_MOVES
	  }
	  else { // the last block
		#ifdef REPORT_INTERMEDIATE_RESULTS
			  sprintf(buf, "\t\t- Last block -\n\n");
			  fprintf(dbgFile, buf);
			  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[bs]]->ID(), O[CPath[bs+1]]->ID());
			  fprintf(dbgFile, buf);
		#endif

		// Exchange the first two nodes and evaluate the resultant makespan
		Stmp->Copy(S);
		if(Stmp->Swap_m(m, CPath[bs], CPath[bs+1])){
#ifdef USE_APPROXIMATE_LB
		  if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			if(Stmp->LB < _LB){
			  _u = CPath[bs];
			  _v = CPath[bs+1];
			  _LB = Stmp->LB;
			  _m = m;
			  cBestMove = 1;
			}
		  }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[bs];
				_v = CPath[bs+1];
				_Cmax = Stmp->Cmax;
				_m = m;
				cBestMove = 1;
			  }
			}
		  }
#endif
		}
	  }
#endif
	  bs = be;
	}
  }while(be < cpl);

  // Second, loop through job blocks in the critical path
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n- Job blocks:\n\t");
		  fprintf(dbgFile, buf);
	 #endif

  int j;
  bs = 0;
  do{
	// find the next block end
	j = O[CPath[bs]]->Job->i;
	for(be=bs+1; be<cpl; be++){
	  if(j != O[CPath[be]]->Job->i) break;
	} // Now the block starts at bs and ends at be-1

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\n\tBlock: ");
		  fprintf(dbgFile, buf);
		  for(int i=bs; i<be; i++){
			sprintf(buf, "%s-", O[CPath[i]]->ID());
			fprintf(dbgFile, buf);
		  }
	 #endif

	if(be - bs == cpl){  // The critical path is composed of only one block
						 // --> a local optimal solution is found,
						 // i.e. no better solution can be obtained for the current
						 // machine selection decisions --> STOP!

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\t---- Critical path contains only one block -> STOP!\n");
		  fprintf(dbgFile, buf);
	 #endif

	  bTerminate = true;
	  return;
	}

	if(be - bs == 1){ // The block contains only one operation (node) --> do nothing
	  bs++;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\t---- Block contains only one operation -> do nothing\n");
		  fprintf(dbgFile, buf);
	 #endif

	}
	else{
#ifdef USE_FIRST_LAST_MOVES
	  if(bs == 0){   // the first block
		#ifdef REPORT_INTERMEDIATE_RESULTS
			  sprintf(buf, "\t\t- First block -\n\n");
			  fprintf(dbgFile, buf);
			  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[be-2]]->ID(), O[CPath[be-1]]->ID());
			  fprintf(dbgFile, buf);
		#endif

		// Exchange the last two nodes and evaluate the resultant makespan
		Stmp->Copy(S);
		if(Stmp->Swap_j(j, CPath[be-2], CPath[be-1])){
#ifdef USE_APPROXIMATE_LB
		  if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			if(Stmp->LB < _LB){
			  _u = CPath[be-2];
			  _v = CPath[be-1];
			  _LB = Stmp->LB;
			  _j = j;
			  cBestMove = 2;
			}
		  }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[be-2];
				_v = CPath[be-1];
				_Cmax = Stmp->Cmax;
				_j = j;
				cBestMove = 2;
			  }
			}
		  }
#endif
		}
	  }
	  else if(be != cpl){ // a middle block
#endif
		#ifdef REPORT_INTERMEDIATE_RESULTS
#ifdef USE_FIRST_LAST_MOVES
			  sprintf(buf, "\t\t- Middle block -\n\n");
			  fprintf(dbgFile, buf);
#endif
		  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[bs]]->ID(), O[CPath[bs+1]]->ID());
		  fprintf(dbgFile, buf);
		#endif

		// Exchange the first two nodes and evaluate the resultant makespan
		Stmp->Copy(S);

		if(Stmp->Swap_j(j, CPath[bs], CPath[bs+1])){
#ifdef USE_APPROXIMATE_LB
		   if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			 if(Stmp->LB < _LB){
			   _u = CPath[bs];
			   _v = CPath[bs+1];
			   _LB = Stmp->LB;
			   _j = j;
			   cBestMove = 2;
			}
		  }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[bs];
				_v = CPath[bs+1];
				_Cmax = Stmp->Cmax;
				_j = j;
				cBestMove = 2;
			  }
			}
		  }
#endif
		}
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		else {
		  sprintf(buf, "\t-- exchange failed");
		  fprintf(dbgFile, buf);
		}
	 #endif

		// Exchange the last two nodes and evaluate the resultant makespan
		if(be != bs+2){  // don't repeat the same exchange done previously!
		  // Exchange the last two nodes and evaluate the resultant makespan
		  Stmp->Copy(S);

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[be-2]]->ID(), O[CPath[be-1]]->ID());
		  fprintf(dbgFile, buf);
	 #endif

		  if(Stmp->Swap_j(j, CPath[be-2], CPath[be-1])){
#ifdef USE_APPROXIMATE_LB
			if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
			  if(Stmp->LB < _LB){
				_u = CPath[be-2];
				_v = CPath[be-1];
				_LB = Stmp->LB;
				_j = j;
				cBestMove = 2;
			  }
			}
#else
			if(Stmp->InterpretNetwork()){
			  if((isTabu(CPath[be-2], CPath[be-1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[be-2], CPath[be-1])){
				if(Stmp->Cmax < _Cmax){
				  _u = CPath[be-2];
				  _v = CPath[be-1];
				  _Cmax = Stmp->Cmax;
				  _j = j;
				  cBestMove = 2;
				}
			  }
			}
#endif
		  }
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  else {
			sprintf(buf, "\t-- exchange failed");
			fprintf(dbgFile, buf);
		  }
	 #endif
		}
#ifdef USE_FIRST_LAST_MOVES
	  }
	  else { // the last block
		#ifdef REPORT_INTERMEDIATE_RESULTS
			  sprintf(buf, "\t\t- Last block -\n\n");
			  fprintf(dbgFile, buf);
			  sprintf(buf, "\n\t\tExchange %s and %s", O[CPath[bs]]->ID(), O[CPath[bs+1]]->ID());
			  fprintf(dbgFile, buf);
		#endif

		// Exchange the first two nodes and evaluate the resultant makespan
		Stmp->Copy(S);
		if(Stmp->Swap_j(j, CPath[bs], CPath[bs+1])){
#ifdef USE_APPROXIMATE_LB
		  if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->LB < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			if(Stmp->LB < _LB){
			  _u = CPath[bs];
			  _v = CPath[bs+1];
			  _LB = Stmp->LB;
			  _j = j;
			  cBestMove = 2;
			}
		  }
#else
		  if(Stmp->InterpretNetwork()){
			if((isTabu(CPath[bs], CPath[bs+1]) && Stmp->Cmax < Sbest->Cmax) || !isTabu(CPath[bs], CPath[bs+1])){
			  if(Stmp->Cmax < _Cmax){
				_u = CPath[bs];
				_v = CPath[bs+1];
				_Cmax = Stmp->Cmax;
				_j = j;
				cBestMove = 2;
			  }
			}
		  }
#endif
		}
	  }
#endif
	  bs = be;
	}
  }while(be < cpl);

  if(cBestMove != 0){
	// Step 10: Update the tabu list to maintain its size so that if it is currently
	//          having a size that equals its limit, remove the oldest move from it
	// Step 11: Add the inverse of the selected move to the tabu list
	AddTabu(_v, _u);

	// Step 12: If the new Cmax after applying the selected move is found to be
	//          less than the current Cmax, then update the best Cmax and the
	//          best solution and let itr_imp = 0; otherwise let itr_imp++
	Stmp->Copy(S);
	switch(cBestMove){
	  case 1: Stmp->Swap_m(_m, _u, _v);
			  break;
	  case 2: Stmp->Swap_j(_j, _u, _v);
			  break;
	}
	Stmp->InterpretNetwork();
	S->Copy(Stmp);
	if(S->Cmax < Sbest->Cmax){
	  itr_imp = 0;
	  Sbest->Copy(S);
	  if(Sbest->Cmax == LB){
		bTerminate = true;

		#ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n------------------------- Cmax = LB -----> Optimal solution found");
		#endif
	  }
	}
	else{
	  itr_imp++;
	}
  }
  else{
	bTerminate = true;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\n---------------- No non-tabued moves are found!  --> STOP\n");
		  fprintf(dbgFile, buf);
	 #endif
  }

  itr++;
}

bool MOSP_TS::Stop()
{
  frmTSCalcProgress->lblCurrentCmax->Caption = IntToStr(S->Cmax);
  frmTSCalcProgress->lblBestCmax->Caption = IntToStr(Sbest->Cmax);

  if(bTerminate){
	frmTSCalcProgress->btnStart->Enabled = true;
	frmTSCalcProgress->btnClose->Enabled = true;
	frmTSCalcProgress->btnTerminate->Enabled = false;
	frmTSCalcProgress->ProgressBar->Position = itr;
	frmTSCalcProgress->lblIter->Caption = IntToStr(itr);
	return true;
  }

  timeb curTime;
  ::ftime(&curTime);
  ComputationalTime = (curTime.time - start_time.time)*1000.0 +
					  (curTime.millitm - start_time.millitm);

  // Step 13: apply the stopping rules
  //if(itr % 100 == 0){
	frmTSCalcProgress->ProgressBar->Position = itr;
	frmTSCalcProgress->lblIter->Caption = IntToStr(itr);
  //}
  frmTSCalcProgress->lblImpIter->Caption = IntToStr(itr_imp);

  if(itr == nIterations || ComputationalTime >= MaxTime || itr_imp == nImpIterations){
	frmTSCalcProgress->btnStart->Enabled = true;
	frmTSCalcProgress->btnClose->Enabled = true;
	frmTSCalcProgress->btnTerminate->Enabled = false;
	frmTSCalcProgress->ProgressBar->Position = itr;
	frmTSCalcProgress->lblIter->Caption = IntToStr(itr);
	return true;
  }
  else return false;
}

void MOSP_TS::Finalize()
{
  // copy the Sbest results into tasks...
  for(int i=0; i<nOps; i++){
	tsk[i]->StartTime = Sbest->Nodes[i].head;
	tsk[i]->EndTime = Sbest->Nodes[i].head + Sbest->Nodes[i].weight;
  }

  MOSP_Algorithm::Finalize();

  SolStatus = FEASIBLE;
}

void MOSP_TS::FreeMemory()
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

  delete [] jnxt;
  delete [] mnxt;

  delete S0;
  delete S;
  delete Sbest;
  delete Stmp;

  delete [] Tabus;

  delete [] CPath;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fclose(dbgFile);
	 #endif
}

void MOSP_TS::hPSOTS_FreeMemory()
{
  FreeArrays();

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

  delete [] jnxt;
  delete [] mnxt;

  delete S0;
  delete S;
  delete Sbest;
  delete Stmp;

  delete [] Tabus;

  delete [] CPath;
}

void MOSP_TS::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

void MOSP_TS::AddTabu(int u, int v)
{
  if(TabuSize == 0) return;
  if(TabuPntr == TabuSize) TabuPntr = 0;

  Tabus[TabuPntr].u = u;
  Tabus[TabuPntr].v = v;

  TabuPntr++;
  if(nTabus < TabuSize) nTabus++;

	#ifdef REPORT_INTERMEDIATE_RESULTS
			char buf[128];
			sprintf(buf, "\n\nCurrent number of tabus = %i\n", nTabus);
			fprintf(dbgFile, buf);
			for(int i=0; i<nTabus; i++){
			  sprintf(buf, "\t(%i, %i) - (%s, %s)  %c\n",
							 Tabus[i].u, Tabus[i].v, O[Tabus[i].u]->ID(), O[Tabus[i].v]->ID(),
							 ((i==TabuPntr)? '*' : ' '));
			  fprintf(dbgFile, buf);
			}
	#endif
}

void MOSP_TS::ClearTabus()
{
  TabuPntr = nTabus = 0;
}

bool MOSP_TS::isTabu(int u, int v)
{
  if(TabuSize == 0) return false;
  int i;
  MOSP_Tabu *T = Tabus;
  MOSP_Tabu Comp;
  Comp.u = u;    Comp.v = v;

  for(i=0; i<nTabus; i++){
	if(memcmp(T, &Comp, sizeof(MOSP_Tabu)) == 0) return true;
	T++;
  }
  return false;
}

void MOSP_TS::DS_MWR(int **MA)
{
  int i, j, m;
  int t;   // Current scheduling time
  int Cmax = 0;

  for(int i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  for(int i=0; i<nJobs; i++){
	Jnft[i] = J[i]->ReleaseTime;
	bJcompleted[i] = false;
  }
  for (int i = 0; i < nJobs; i++){
	for(int m=0; m<nMCs; m++){
	  bJM[i][m] = (MA[Mw[m]][i]+Wa[Mw[m]] == m)? true : false; //(pt[i][m] > 0)? true : false;
	}
  }

   // Clear assignments made for first and last machine/job nodes
   memset(S0->FirstjNode, -1, nJobs*sizeof(int));
   memset(S0->LastjNode, -1, nJobs*sizeof(int));
   memset(S0->FirstmNode, -1, nMCs*sizeof(int));
   memset(S0->LastmNode, -1, nMCs*sizeof(int));

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

	// Define the set of ready machines at time t
	for(m=0; m<nMCs; m++){
	  bMCready[m] = (MCrt[m] == t && !bMCdone[m])? true : false;
	}

	// For each machine ready in time t, select a job
	//
	for(m=0; m<nMCs; m++){
		if(!bMCready[m]) continue;

		// Check to see if there is any job that needs to be scheduled on machine m
		//   preference is made first to jobs that has nearest finish time less than
		//   or equal t. If there isn't any job with that condition, all schedulable
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

		// Now select a job from all schedulable jobs based on the most work remaining
		//
		int Mr;  // The measure based upon which a job will be selected
		for(j=0; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  SelJob[m] = j;
		  Mr = 0;
		  for(i=Ja[j]; i<=Jz[j]; i++){
			if(bScheduled[i]) continue;
			Mr += O[i]->maxpt();
		  }
		  break;
		}
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  int wrk;
		  wrk = 0;
		  for(i=Ja[j]; i<=Jz[j]; i++){
			if(bScheduled[i]) continue;
			wrk += O[i]->maxpt();
		  }
		  if(wrk > Mr){
			Mr = wrk;
			SelJob[m] = j;
		  }
		}
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections randomly
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
		if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  SelMC = (flip(0.5))? m : m2;    // Select a machine randomly

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
	  if(Jnft[j] > Cmax) Cmax = Jnft[j];  // <--------------------- Update Cmax
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // define network parameters for the initial solution
	  S0->Nodes[i].i = i;   // index will be needed during the neighborhood search
	  S0->Nodes[i].mc = m;  // index of assigned machine to the operation with node i

	  if(S0->FirstjNode[j] == -1) S0->FirstjNode[j] = i;
	  if(S0->FirstmNode[m] == -1) S0->FirstmNode[m] = i;

	  int i2 = S0->LastmNode[m];
	  S0->Nodes[i].m_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].m_nxt = i;
	  S0->Nodes[i].m_nxt = -1;
	  S0->LastmNode[m] = i;

	  i2 = S0->LastjNode[j];
	  S0->Nodes[i].j_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].j_nxt = i;
	  S0->Nodes[i].j_nxt = -1;
	  S0->LastjNode[j] = i;

	  S0->Nodes[i].weight = pt[j][m];
	  S0->Nodes[i].head = tsk[i]->StartTime;
	  S0->Nodes[i].tail = -1;  // means undefined to be defined later

	  // Now job j does not need any machine in the processing center p
	  //int p = Mw[m];  // index of the WC to which machine m belongs
	  //for(int m2=Pa[p]; m2<=Pz[p]; m2++) bJM[j][m2] = false;
	  bJM[j][m] = false;

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
  S0->Cmax = Cmax;

  // Define tails for network nodes and if they are critical or not
  int rem = nOps; // number of remaining operations which still need to defined their tails
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = 0, imaxEFT;
	for(i = 0; i<nOps; i++){
	  if(S0->Nodes[i].tail != -1) continue;  // tail is already defined
	  int eft = S0->Nodes[i].head + S0->Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	}

	int mlft, jlft;
	if(S0->Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = S0->Nodes[S0->Nodes[imaxEFT].m_nxt].tail;
	if(S0->Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = S0->Nodes[S0->Nodes[imaxEFT].j_nxt].tail;
	S0->Nodes[imaxEFT].tail = ((mlft < jlft)? mlft : jlft) - S0->Nodes[imaxEFT].weight;
	if(S0->Nodes[imaxEFT].head == S0->Nodes[imaxEFT].tail) S0->Nodes[imaxEFT].bCritical = true;
	else S0->Nodes[imaxEFT].bCritical = false;

	rem--;
  }

  // let S = S_best = S_0, Cmax = makespan of S and itr = itr_imp = 0
  //
  S->Copy(S0);
  if(Sbest->Cmax > S->Cmax){
	Sbest->Copy(S0);
  }
  itr = itr_imp = 0;

  // Start with empty tabu list
  nTabus = TabuPntr = 0;
}

void MOSP_TS::DS_MCT(int **MA)
{
  int i, j, m;
  int t;   // Current scheduling time
  int Cmax = 0;

  for(int i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  for(int i=0; i<nJobs; i++){
	Jnft[i] = J[i]->ReleaseTime;
	bJcompleted[i] = false;
  }
  for (int i = 0; i < nJobs; i++){
	for(int m=0; m<nMCs; m++){
	  bJM[i][m] = (MA[Mw[m]][i]+Wa[Mw[m]] == m)? true : false; //(pt[i][m] > 0)? true : false;
	}
  }

   // Clear assignments made for first and last machine/job nodes
   memset(S0->FirstjNode, -1, nJobs*sizeof(int));
   memset(S0->LastjNode, -1, nJobs*sizeof(int));
   memset(S0->FirstmNode, -1, nMCs*sizeof(int));
   memset(S0->LastmNode, -1, nMCs*sizeof(int));

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

	// Define the set of ready machines at time t
	for(m=0; m<nMCs; m++){
	  bMCready[m] = (MCrt[m] == t && !bMCdone[m])? true : false;
	}

	// For each machine ready in time t, select a job
	//
	for(m=0; m<nMCs; m++){
		if(!bMCready[m]) continue;

		// Check to see if there is any job that needs to be scheduled on machine m
		//   preference is made first to job that has nearest finish time less than
		//   or equal t. If there isn't any job with that condition, all schedulable
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

		// Now select a job from all schedulable jobs based on the minimum
		//  completion time
		//
		int Mr;  // The measure based upon which a job will be selected
		for(j=0; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  SelJob[m] = j;
		  Mr = Jnft[j] + pt[j][m];
		  break;
		}
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  if(Jnft[j] + pt[j][m] < Mr){
			SelJob[m] = j;
			Mr = Jnft[j] + pt[j][m];
		  }
		}
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections randomly
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
		if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  SelMC = (flip(0.5))? m : m2;    // Select a machine randomly

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
	  if(Jnft[j] > Cmax) Cmax = Jnft[j];  // <--------------------- Update Cmax
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // define network parameters for the initial solution
	  S0->Nodes[i].i = i;   // index will be needed during the neighborhood search
	  S0->Nodes[i].mc = m;  // index of assigned machine to the operation with node i

	  if(S0->FirstjNode[j] == -1) S0->FirstjNode[j] = i;
	  if(S0->FirstmNode[m] == -1) S0->FirstmNode[m] = i;

	  int i2 = S0->LastmNode[m];
	  S0->Nodes[i].m_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].m_nxt = i;
	  S0->Nodes[i].m_nxt = -1;
	  S0->LastmNode[m] = i;

	  i2 = S0->LastjNode[j];
	  S0->Nodes[i].j_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].j_nxt = i;
	  S0->Nodes[i].j_nxt = -1;
	  S0->LastjNode[j] = i;

	  S0->Nodes[i].weight = pt[j][m];
	  S0->Nodes[i].head = tsk[i]->StartTime;
	  S0->Nodes[i].tail = -1;  // means undefined to be defined later

	  // Now job j does not need any machine in the processing center p
	  //int p = Mw[m];  // index of the WC to which machine m belongs
	  //for(int m2=Pa[p]; m2<=Pz[p]; m2++) bJM[j][m2] = false;
	  bJM[j][m] = false;

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
  S0->Cmax = Cmax;

  // Define tails for network nodes and if they are critical or not
  int rem = nOps; // number of remaining operations which still need to defined their tails
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = 0, imaxEFT;
	for(i = 0; i<nOps; i++){
	  if(S0->Nodes[i].tail != -1) continue;  // tail is already defined
	  int eft = S0->Nodes[i].head + S0->Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	}

	int mlft, jlft;
	if(S0->Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = S0->Nodes[S0->Nodes[imaxEFT].m_nxt].tail;
	if(S0->Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = S0->Nodes[S0->Nodes[imaxEFT].j_nxt].tail;
	S0->Nodes[imaxEFT].tail = ((mlft < jlft)? mlft : jlft) - S0->Nodes[imaxEFT].weight;
	if(S0->Nodes[imaxEFT].head == S0->Nodes[imaxEFT].tail) S0->Nodes[imaxEFT].bCritical = true;
	else S0->Nodes[imaxEFT].bCritical = false;

	rem--;
  }

  // let S = S_best = S_0, Cmax = makespan of S and itr = itr_imp = 0
  //
  S->Copy(S0);
  if(Sbest->Cmax > S->Cmax){
	Sbest->Copy(S0);
  }
  itr = itr_imp = 0;

  // Start with empty tabu list
  nTabus = TabuPntr = 0;
}

void MOSP_TS::Active_MCT(int **MA)
{
  int i, j, m;
  int t;   // Current scheduling time
  int Cmax = 0;

  for(int i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  for(int i=0; i<nJobs; i++){
	Jnft[i] = J[i]->ReleaseTime;
	bJcompleted[i] = false;
  }
  for (int i = 0; i < nJobs; i++){
	for(int m=0; m<nMCs; m++){
	  bJM[i][m] = (MA[Mw[m]][i]+Wa[Mw[m]] == m)? true : false; //(pt[i][m] > 0)? true : false;
	}
  }

   // Clear assignments made for first and last machine/job nodes
   memset(S0->FirstjNode, -1, nJobs*sizeof(int));
   memset(S0->LastjNode, -1, nJobs*sizeof(int));
   memset(S0->FirstmNode, -1, nMCs*sizeof(int));
   memset(S0->LastmNode, -1, nMCs*sizeof(int));

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

	// Find the machine _m on which the minimum completion time can be achieved
	// and store the minimum completion time in _ct
	//
	int _m = -1, _ct = MAXINT;
	for(m=0; m<nMCs; m++){
	  if(bMCdone[m]) continue;
	  for(j=0; j<nJobs; j++){
		if(bJM[j][m]){
		  int ct = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		  ct += pt[j][m];
		  if(ct < _ct){
			_m = m;
			_ct = ct;
		  }
		}
	  }
	}
	if(_m == -1) break;  // Shouldn't happen

	// Among all jobs that have ready times (nft) less than _ct, select
	// the job with the minimum completion time to be scheduled next
	//
	int _j = -1, _mct = MAXINT;
	for(j=0; j<nJobs; j++){
	  if(bJM[j][_m] && Jnft[j] < _ct){
		int ct = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		for(i=Ja[j]; i<=Jz[j]; i++){
		  if(bScheduled[i]) continue;
		  ct += O[i]->maxpt();
		}
		if(ct < _mct){
		  _j = j;
		  _mct = ct;
		}
	  }
	}
	if(_j == -1) break;  // Shouldn't happen

    // ---> Now schedule job _j on machine _m

	  j = _j;
	  m = _m;

	  // define which operation index
	  for(i=Ja[j]; i<=Jz[j]; i++) if(O[i]->WC == M[m]->WC) break;
	  if(i > Jz[j]) continue;  // <-- this should not happen!!

	  // Update machine and job times and set operation and task related variables
	  tsk[i]->SelectedMC = M[m];
	  tsk[i]->StartTime = (Jnft[j] > MCrt[m])? Jnft[j] : MCrt[m];
	  Jnft[j] = tsk[i]->StartTime + pt[j][m];
	  MCrt[m] = Jnft[j];
	  tsk[i]->EndTime = Jnft[j];
	  if(Jnft[j] > Cmax) Cmax = Jnft[j];  // <--------------------- Update Cmax
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // define network parameters for the initial solution
	  S0->Nodes[i].i = i;   // index will be needed during the neighborhood search
	  S0->Nodes[i].mc = m;  // index of assigned machine to the operation with node i

	  if(S0->FirstjNode[j] == -1) S0->FirstjNode[j] = i;
	  if(S0->FirstmNode[m] == -1) S0->FirstmNode[m] = i;

	  int i2 = S0->LastmNode[m];
	  S0->Nodes[i].m_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].m_nxt = i;
	  S0->Nodes[i].m_nxt = -1;
	  S0->LastmNode[m] = i;

	  i2 = S0->LastjNode[j];
	  S0->Nodes[i].j_prv = i2;
	  if(i2 != -1) S0->Nodes[i2].j_nxt = i;
	  S0->Nodes[i].j_nxt = -1;
	  S0->LastjNode[j] = i;

	  S0->Nodes[i].weight = pt[j][m];
	  S0->Nodes[i].head = tsk[i]->StartTime;
	  S0->Nodes[i].tail = -1;  // means undefined to be defined later

	  // Now job j does not need machine m
	  bJM[j][m] = false;

	  // Check if all operations of job j are completed
	  bAllDone = true;
	  for(int i2=Ja[j]; i2<=Jz[j]; i2++){
		if(!bScheduled[i2]){
		  bAllDone = false;
		  break;
		}
	  }
	  bJcompleted[j] = bAllDone;

  } // End of main loop
  S0->Cmax = Cmax;

  // Define tails for network nodes and if they are critical or not
  int rem = nOps; // number of remaining operations which still need to defined their tails
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	int maxEFT = 0, imaxEFT;
	for(i = 0; i<nOps; i++){
	  if(S0->Nodes[i].tail != -1) continue;  // tail is already defined
	  int eft = S0->Nodes[i].head + S0->Nodes[i].weight;
	  if(eft > maxEFT){
		maxEFT = eft;
		imaxEFT = i;
	  }
	}

	int mlft, jlft;
	if(S0->Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	else mlft = S0->Nodes[S0->Nodes[imaxEFT].m_nxt].tail;
	if(S0->Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	else jlft = S0->Nodes[S0->Nodes[imaxEFT].j_nxt].tail;
	S0->Nodes[imaxEFT].tail = ((mlft < jlft)? mlft : jlft) - S0->Nodes[imaxEFT].weight;
	if(S0->Nodes[imaxEFT].head == S0->Nodes[imaxEFT].tail) S0->Nodes[imaxEFT].bCritical = true;
	else S0->Nodes[imaxEFT].bCritical = false;

	rem--;
  }

  // let S = S_best = S_0, Cmax = makespan of S and itr = itr_imp = 0
  //
  S->Copy(S0);
  if(Sbest->Cmax > S->Cmax){
	Sbest->Copy(S0);
  }
  itr = itr_imp = 0;

  // Start with empty tabu list
  nTabus = TabuPntr = 0;
}

void MOSP_TS::ConvertNetSol()
{
  // copy the Sbest results into tasks...
  for(int i=0; i<nOps; i++){
	tsk[i]->StartTime = Sbest->Nodes[i].head;
	tsk[i]->EndTime = Sbest->Nodes[i].head + Sbest->Nodes[i].weight;
  }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
