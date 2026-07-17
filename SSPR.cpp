//---------------------------------------------------------------------------


#pragma hdrstop

#include <math.h>

#include "SSPR.h"
#include "SList.h"
#include "Main.h"
#include "random.h"
#include "FormSSPRCalcProgress.h"
#include "FormSSPRMultiRuns.h"

#ifdef PRNT_SSPR_DBG
FILE *ssprDbgFile;
#endif

TaguchiExperiment Taguchi[] = {
	2,	3,	10,	10,	10,	10,
	2,	3,	10,	10,	20,	20,
	2,	3,	10,	10,	40,	40,
	2,	5,	20,	20,	10,	10,
	2,	5,	20,	20,	20,	20,
	2,	5,	20,	20,	40,	40,
	2,	8,	40,	40,	10,	10,
	2,	8,	40,	40,	20,	20,
	2,	8,	40,	40,	40,	40,
	3,	3,	20,	40,	10,	20,
	3,	3,	20,	40,	20,	40,
	3,	3,	20,	40,	40,	10,
	3,	5,	40,	10,	10,	20,
	3,	5,	40,	10,	20,	40,
	3,	5,	40,	10,	40,	10,
	3,	8,	10,	20,	10,	20,
	3,	8,	10,	20,	20,	40,
	3,	8,	10,	20,	40,	10,
	4,	3,	40,	20,	10,	40,
	4,	3,	40,	20,	20,	10,
	4,	3,	40,	20,	40,	20,
	4,	5,	10,	40,	10,	40,
	4,	5,	10,	40,	20,	10,
	4,	5,	10,	40,	40,	20,
	4,	8,	20,	10,	10,	40,
	4,	8,	20,	10,	20,	10,
	4,	8,	20,	10,	40,	20
};

int MOSPSSPRSolComp(void * Item1, void * Item2)
{
  MOSP_SSPR_Solution *Sol1, *Sol2;
  Sol1 = (MOSP_SSPR_Solution *)Item1;
  Sol2 = (MOSP_SSPR_Solution *)Item2;
  return(Sol1->CmaxValue() - Sol2->CmaxValue());
}

//---------------------------------------------------------------------------

MOSP_SSPR_Solution::MOSP_SSPR_Solution(MOSP_SSPR *alg)
{
  AllocateMemory(alg);
}

void MOSP_SSPR_Solution::AllocateMemory(MOSP_SSPR *alg)
{
  sspr = alg;
  int nJobs = sspr->nJobs;
  int nWCs = sspr->nWCs;
  int nMCs = sspr->nMCs;
  int nOps = sspr->nOps;

  // Allocate all required memory for storing vector and network data

  tau = new int *[nJobs];
  for(int j=0; j<nJobs; j++){
	tau[j] = new int [nWCs+1];
  }

  pi = new int *[nMCs+1];
  for(int m=0; m<nMCs; m++){
	pi[m] = new int [nJobs+1];
  }

  jb = new int [nOps];
  mc = new int [nOps];
  tau_pos = new int [nOps];
  pi_pos = new int [nOps];

  Nodes = new MOSP_SSPR_Node[nOps];
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

  etgmx_1 = new int [nWCs+1];
  etgmx_2 = new int [nJobs+1];

  Reset();
}

MOSP_SSPR_Solution::~MOSP_SSPR_Solution()
{
  FreeMemory();
}

void MOSP_SSPR_Solution::FreeMemory()
{
  for(int j=0; j<sspr->nJobs; j++){
	delete [] tau[j];
  }
  delete [] tau;

  for(int m=0; m<sspr->nMCs; m++){
	delete [] pi[m];
  }
  delete [] pi;

  delete [] jb;
  delete [] mc;
  delete [] tau_pos;
  delete [] pi_pos;

  delete [] Nodes;
  delete [] FirstjNode;
  delete [] LastjNode;
  delete [] FirstmNode;
  delete [] LastmNode;

  delete [] MCrt;    // machine's ready time
  delete [] mnxt;    // next node index for the machine
  delete [] Jnft;    // job's nearest finish time
  delete [] jnxt;    // next node index for the job

  delete [] jcur;
  delete [] mcur;

  delete [] etgmx_1;
  delete [] etgmx_2;
}

void MOSP_SSPR_Solution::Reset()
{
  int nJobs = sspr->nJobs;
  int nWCs = sspr->nWCs;
  int nMCs = sspr->nMCs;
  int nOps = sspr->nOps;

  // reset all values so that no solution exists
  for(int j=0; j<nJobs; j++){
	memset((void *)tau[j], -1, sizeof(int) * (nWCs+1));
  }

  for(int m=0; m < nMCs; m++){
	memset((void *)pi[m], -1, sizeof(int) * (nJobs+1));
  }

  memset((void *)jb, -1, sizeof(int) * nOps);
  memset((void *)mc, -1, sizeof(int) * nOps);
  memset((void *)tau_pos, -1, sizeof(int) * nOps);
  memset((void *)pi_pos, -1, sizeof(int) * nOps);

  memset(FirstjNode, -1, nJobs*sizeof(int));
  memset(LastjNode, -1, nJobs*sizeof(int));
  memset(FirstmNode, -1, nMCs*sizeof(int));
  memset(LastmNode, -1, nMCs*sizeof(int));

  Cmax = -1;   // Just to notify that the network solution has not been
		   	   // constructed yet

  for(int i=0; i<nOps; i++) Nodes[i].Reset();
}

void MOSP_SSPR_Solution::Copy(MOSP_SSPR_Solution *S)
{
  if(S->sspr != sspr){
	FreeMemory();
	AllocateMemory(S->sspr);
  }

  int nJobs = sspr->nJobs;
  int nWCs = sspr->nWCs;
  int nMCs = sspr->nMCs;
  int nOps = sspr->nOps;

  for(int j=0; j<nJobs; j++){
	memcpy((void *)tau[j], (const void *)S->tau[j], sizeof(int) * (nWCs+1));
  }

  for(int m=0; m < nMCs; m++){
	memcpy((void *)pi[m], (const void *)S->pi[m], sizeof(int) * (nJobs+1));
  }

  memcpy((void *)jb, (const void *)S->jb, sizeof(int) * nOps);
  memcpy((void *)mc, (const void *)S->mc, sizeof(int) * nOps);
  memcpy((void *)tau_pos, (const void *)S->tau_pos, sizeof(int) * nOps);
  memcpy((void *)pi_pos, (const void *)S->pi_pos, sizeof(int) * nOps);

  memcpy((void *)Nodes, (const void *)S->Nodes, nOps * sizeof(MOSP_SSPR_Node));

  memcpy((void *)FirstjNode, (const void *)S->FirstjNode, nJobs*sizeof(int));
  memcpy((void *)LastjNode, (const void *)S->LastjNode, nJobs*sizeof(int));
  memcpy((void *)FirstmNode, (const void *)S->FirstmNode, nMCs*sizeof(int));
  memcpy((void *)LastmNode, (const void *)S->LastmNode, nMCs*sizeof(int));

  Cmax = S->Cmax;
}

void MOSP_SSPR_Solution::InterpretTaskList()
{
  Reset();

  for(int j=0; j < sspr->nJobs; j++) tau[j][0] = 0;
  for(int m=0; m < sspr->nMCs; m++) pi[m][0] = 0;

  SList &Tasks = sspr->Solution->ScheduledTasks;

	  #ifdef PRNT_VNT_DBG
		AssertTasks("Tasks before creating the vectors");
	  #endif

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	int j, m, o;
	j = tsk->Operation->Job->i;
	m = tsk->SelectedMC->i;
	o = tsk->Operation->i;

	tau_pos[o] = ++tau[j][0];
	pi_pos[o] = ++pi[m][0];

	tau[j][tau_pos[o]] = o;
	pi[m][pi_pos[o]] = o;

	jb[o] = j;
	mc[o] = m;
  }

	  #ifdef PRNT_VNT_DBG
		AssertVectors("Generated vectors based on the given task list");
	  #endif
}

void MOSP_SSPR_Solution::GenerateNetwork()
{
  // interpret vectors into nodes and arcs only and assign node weights
  int i, k, j, m, l;

  int nJobs = sspr->nJobs;
  int nMCs = sspr->nMCs;

  for(j=0; j<nJobs; j++){
	l = tau[j][0];
	if(l > 0){
	  i = tau[j][1];
	  FirstjNode[j] = i;
	  Nodes[i].j_prv = -1;
	  for(k = 2; k <= l; k++){
		m = tau[j][k];
		Nodes[i].j_nxt = m;
		Nodes[m].j_prv = i;
		i = m;
	  }
	  Nodes[i].j_nxt = -1;
	  LastjNode[j] = i;
	}
	else {
	  FirstjNode[j] = -1;
	  LastjNode[j] = -1;
	}
  }

  for(m=0; m<nMCs; m++){
	l = pi[m][0];
	if(l > 0){
	  i = pi[m][1];
	  FirstmNode[m] = i;
	  Nodes[i].m_prv = -1;
	  Nodes[i].weight = sspr->pt[jb[i]][m];
	  for(k = 2; k <= l; k++){
		j = pi[m][k];
		Nodes[i].m_nxt = j;
		Nodes[j].m_prv = i;
		Nodes[j].weight = sspr->pt[jb[j]][m];
		i = j;
	  }
	  Nodes[i].m_nxt = -1;
	  LastmNode[m] = i;
	}
	else{
	  FirstmNode[m] = -1;
	  LastmNode[m] = -1;
	}
  }
}

bool MOSP_SSPR_Solution::InterpretNetwork()
{
  int i, j, m;

  bool bFeasible = true;

  for(i=0; i<sspr->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  int nUsedMCs = sspr->nMCs;          // number of machines that have nodes
  int nIncludedJobs = sspr->nJobs;    // number of jobs with nodes included

  for(m=0; m<sspr->nMCs; m++){
	MCrt[m] = 0;
	mnxt[m] = FirstmNode[m];
	if(mnxt[m] == -1) nUsedMCs--;
  }

  for(j=0; j<sspr->nJobs; j++){
	Jnft[j] = 0;
	jnxt[j] = FirstjNode[j];
	if(jnxt[j] == -1) nIncludedJobs--;
  }

  // Determine the earliest start times for all operations, and determine Cmax
  Cmax = 0;
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<sspr->nMCs; m++){
	  i = mnxt[m];
	  if(i==-1) continue;
	  j = jb[i];
	  if(jnxt[j] == i){
		Nodes[i].estart = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		MCrt[m] = Jnft[j] = Nodes[i].estart + Nodes[i].weight;
		if(Cmax < MCrt[m]) Cmax = MCrt[m];
		mnxt[m] = Nodes[i].m_nxt;
		jnxt[j] = Nodes[i].j_nxt;

		MOSP_SSPR_Node &ndi = Nodes[i];
		if(ndi.m_prv != -1 && ndi.j_prv != -1) ndi.rank =
						   Nodes[ndi.m_prv].rank + Nodes[ndi.j_prv].rank +  1;
		else if(ndi.j_prv != -1) ndi.rank = Nodes[ndi.j_prv].rank + 1;
		else if(ndi.m_prv != -1) ndi.rank = Nodes[ndi.m_prv].rank + 1;
		else ndi.rank = 1;

		if(mnxt[m] == -1) nUsedMCs--;
		if(jnxt[j] == -1) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  if(!bFeasible) Cmax = -1;
  else{
    int rem = sspr->nOps; // number of remaining operations which need to define start/end times
	// Now, we define the latest start and criticality conditions for nodes
	while(rem > 0){
	  // Search for an operation with the most earliest finish time
	  int maxEFT = -1, imaxEFT = -1;
	  for(i = 0; i < sspr->nOps; i++){
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

	  #ifdef PRNT_VNT_DBG
		AssertNetwork("Generated network based on given vectors");
	  #endif

  return bFeasible;
}

bool MOSP_SSPR_Solution::ConvertNetToTasks()
{
  if(Cmax == -1){   // Network is not interpretted
	return false;
  }

  for(int i=0; i<sspr->nOps; i++){
	MOSP_Solution::SchTask *tsk = sspr->tsk[i];
	tsk->SelectedMC = sspr->M[mc[i]];
	tsk->StartTime = Nodes[i].estart;
	tsk->EndTime = Nodes[i].estart + Nodes[i].weight;
	tsk->bScheduled = true;
  }

	  #ifdef PRNT_VNT_DBG
		AssertTasks("Generated tasks based on the network");
	  #endif

  return true;
}

bool MOSP_SSPR_Solution::InterpretMOSPSolution(MOSP_Solution *sol)
{
  if(sol->Problem != sspr->GetProblem()) return false;

  Reset();

  for(int j=0; j < sspr->nJobs; j++) tau[j][0] = 0;
  for(int m=0; m < sspr->nMCs; m++) pi[m][0] = 0;

  SList &Tasks = sol->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	int j, m, o;
	j = tsk->Operation->Job->i;
	m = tsk->SelectedMC->i;
	o = tsk->Operation->i;

	tau_pos[o] = ++tau[j][0];
	pi_pos[o] = ++pi[m][0];

	tau[j][tau_pos[o]] = o;
	pi[m][pi_pos[o]] = o;

	jb[o] = j;
	mc[o] = m;
  }

	  #ifdef PRNT_VNT_DBG
		   AssertVectors("Vector solution after interpretting tasks list");
	  #endif

  return true;
}

bool MOSP_SSPR_Solution::isOperInAllCrtclPaths(int v)
{
  // Check if operation v1 is included in all critical paths (This is neglected
  // for now as it would take very long time to determine)
  //                    ---- Need to think about it ----
  //
  if(!Nodes[v].bCritical) return false;
  return true;
}

bool MOSP_SSPR_Solution::doesN1Improve(int v1)    // no proofs, needs revision
{
  if(!isOperInAllCrtclPaths(v1)) return false;

  // Check if the preceding operation of the job sequence is critical or
  // otherwise it is the source node.
  //
  int j = jb[v1];
  int x = tau_pos[v1];
  if(x != 1){
	if(!Nodes[tau[j][x-1]].bCritical) return false;
  }

  // Check if the succeeding operation of the job sequence is critical or
  // otherwise it is the terminal node.
  //
  if(x != tau[j][0]){
	if(!Nodes[tau[j][x+1]].bCritical) return false;
  }

  // Now check if the preceding and succeeding operations in the machine
  // sequence are critical. If so, the solution will not improve (Proposition 4)
  //
  int m = mc[v1];
  x = pi_pos[v1];
  bool bPrecCrtcl = false;
  bool bSucCrtcl = false;

  if(x == 1) bPrecCrtcl = true;
  else if(!Nodes[pi[m][x-1]].bCritical) bPrecCrtcl = true;

  if(x == pi[m][0]) bSucCrtcl = true;
  else if(!Nodes[pi[m][x+1]].bCritical) bSucCrtcl = true;

  if(bPrecCrtcl && bSucCrtcl) return false;

  return true;
}

bool MOSP_SSPR_Solution::doesN2Improve(int v2)    // no proofs, needs revision
{
  if(!isOperInAllCrtclPaths(v2)) return false;

  // Check if the preceding operation of the job sequence is critical or
  // otherwise it is the source node.
  //
  int m = mc[v2];
  int x = pi_pos[v2];
  if(x != 1){
	if(!Nodes[pi[m][x-1]].bCritical) return false;
  }

  // Check if the succeeding operation of the job sequence is critical or
  // otherwise it is the terminal node.
  //
  if(x != pi[m][0]){
	if(!Nodes[pi[m][x+1]].bCritical) return false;
  }

  // Now check if the preceding and succeeding operations in the job
  // sequence are critical. If so, the solution will not improve (Proposition 4)
  //
  int j = jb[v2];
  x = tau_pos[v2];
  bool bPrecCrtcl = false;
  bool bSucCrtcl = false;

  if(x == 1) bPrecCrtcl = true;
  else if(!Nodes[tau[j][x-1]].bCritical) bPrecCrtcl = true;

  if(x == tau[j][0]) bSucCrtcl = true;
  else if(!Nodes[tau[j][x+1]].bCritical) bSucCrtcl = true;

  if(bPrecCrtcl && bSucCrtcl) return false;

  return true;
}

void MOSP_SSPR_Solution::alpha_omega_N1(int v1,	int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  MOSP_SSPR_Node *nd_v1 = &Nodes[v1];
  int jv1 = jb[v1];
  int l = tau[jv1][0];

  if(nd_v1->m_prv == -1) apr_alpha = 1;
  else{
	apr_alpha = tau_pos[v1];
	if(apr_alpha != 1){
	  MOSP_SSPR_Node *mprv = &Nodes[nd_v1->m_prv];
	  MOSP_SSPR_Node *cur = nd_v1;
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		apr_alpha--;
	  }
	}
  }

  if(nd_v1->m_nxt == -1) apr_omega = l;
  else{
	apr_omega = tau_pos[v1];
	if(apr_omega != l){
	  MOSP_SSPR_Node *mnxt = &Nodes[nd_v1->m_nxt];
	  MOSP_SSPR_Node *cur = nd_v1;
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		apr_omega++;
	  }
	}
  }
}

int MOSP_SSPR_Solution::phi_N1(int v1)
{
  if(Cmax == -1){   // Network is not interpretted
	return -1;
  }

  int phi;

  MOSP_SSPR_Node &v = Nodes[v1];
  phi =   ((v.m_prv == -1)? 0 : (Nodes[v.m_prv].estart + Nodes[v.m_prv].weight))
		+ v.weight + ((v.m_nxt == -1)? 0 : Nodes[v.m_nxt].tail);

  return phi;
}

void MOSP_SSPR_Solution::Deltas_N1(int v1, int x1, int &hat_delta, int &check_delta)
{
  if(Cmax == -1){   // Network is not interpretted
	hat_delta = check_delta = -1;
	return;
  }

  MOSP_SSPR_Node &v = Nodes[v1];
  int x = tau_pos[v1];
  if(x1 == x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_SSPR_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
	  hat_delta += (jx1prv.estart + jx1prv.weight);
	}
	if(v.m_prv != -1){
	  MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0;
	if(x1 < tau[jb[v1]][0]){
	  MOSP_SSPR_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
	  check_delta += jx1nxt.tail;
	}
	if(v.m_nxt != -1){
	  MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0;
	if(x1 != 1){
	  MOSP_SSPR_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
	  hat_delta += (jx1prv.estart + jx1prv.weight);
	}
	if(v.m_prv != -1){
	  MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0;
	if(x1 > 0){
	  // Get an accurate estimate of the nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  MOSP_SSPR_Node *j_xnd = &Nodes[v.j_prv];
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
	  MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
  else{
	hat_delta = 0;
	if(x1 <= tau[jb[v1]][0]){
	  // Get an accurate estimated value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  MOSP_SSPR_Node &jx1nd = Nodes[tau[jb[v1]][x1]];
	  MOSP_SSPR_Node *j_xnd = &Nodes[v.j_nxt];
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
	  hat_delta += (e + jx1nd.weight);
	}
	if(v.m_prv != -1){
	  MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0;
	if(x1 < tau[jb[v1]][0]){
	  MOSP_SSPR_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
	  check_delta += jx1nxt.tail;
	}
	if(v.m_nxt != -1){
	  MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
}

void MOSP_SSPR_Solution::alpha_omega_N2(int v2, int mk2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  MOSP_SSPR_Node *nd_v2 = &Nodes[v2];

  int l = pi[mk2][0];

  if(mk2 == mc[v2]){ // if same machine
	if(nd_v2->j_prv == -1) apr_alpha = 1;
	else{
	  apr_alpha = pi_pos[v2];
	  if(apr_alpha != 1){
		MOSP_SSPR_Node *jprv = &Nodes[nd_v2->j_prv];
		MOSP_SSPR_Node *cur = nd_v2;
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		}
	  }
	}

	if(nd_v2->j_nxt == -1) apr_omega = l;
	else{
	  apr_omega = pi_pos[v2];
	  if(apr_omega != l){
		MOSP_SSPR_Node *jnxt = &Nodes[nd_v2->j_nxt];
		MOSP_SSPR_Node *cur = nd_v2;
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		}
	  }
	}
  }
  else{
	if(nd_v2->j_prv == -1) apr_alpha = 1;
	else{
	  apr_alpha = l + 1;
	  MOSP_SSPR_Node *jprv = &Nodes[nd_v2->j_prv];
	  MOSP_SSPR_Node *cur;

	  if(LastmNode[mk2] != -1){
		cur = &Nodes[LastmNode[mk2]];
		do{
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);
	  }
	}

	if(nd_v2->j_nxt == -1) apr_omega = l + 1;
	else{
	  apr_omega = 1;
	  MOSP_SSPR_Node *jnxt = &Nodes[nd_v2->j_nxt];
	  MOSP_SSPR_Node *cur;

	  if(FirstmNode[mk2] != -1){
		cur = &Nodes[FirstmNode[mk2]];
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

int MOSP_SSPR_Solution::phi_N2(int v2, int mk2)
{
  if(Cmax == -1){   // Network is not interpretted
	return -1;
  }

  int phi;

  MOSP_SSPR_Node &v = Nodes[v2];
  phi =   ((v.j_prv == -1)? 0 : (Nodes[v.j_prv].estart + Nodes[v.j_prv].weight))
		+ ((v.j_nxt == -1)? 0 : Nodes[v.j_nxt].tail);

  int d = sspr->pt[jb[v2]][mk2];
  phi += d;

  return phi;
}

void MOSP_SSPR_Solution::Deltas_N2(int v2, int mk2, int x2, int &hat_delta, int &check_delta)
{
  if(Cmax == -1){   // Network is not interpretted
	hat_delta = check_delta = -1;
	return;
  }

  MOSP_SSPR_Node &v = Nodes[v2];

  if(mk2 != mc[v2]){ // if different machine
	hat_delta = 0;
	check_delta = 0;

	if(v.j_prv != -1) hat_delta -= (Nodes[v.j_prv].estart +  Nodes[v.j_prv].weight);

	if(x2 != 1){
	  MOSP_SSPR_Node *ndx2prv = &Nodes[pi[mk2][x2-1]];
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(v.j_nxt != -1) check_delta -= Nodes[v.j_nxt].tail;

    int l = pi[mk2][0];
	if(x2 != l+1){
	  MOSP_SSPR_Node *ndx2nxt = &Nodes[pi[mk2][x2]];
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	int x = pi_pos[v2];
	if(x2 == x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_SSPR_Node *mx2prv = &Nodes[pi[mk2][x2-1]];
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < pi[mk2][0]){
		MOSP_SSPR_Node *mx2nxt = &Nodes[pi[mk2][x2+1]];
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0;
	  if(x2 != 1){
		MOSP_SSPR_Node *mx2prv = &Nodes[pi[mk2][x2-1]];
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 > 0){
		// Get an accurate estimate of the nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		MOSP_SSPR_Node *m_xnd = &Nodes[v.m_prv];
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
		MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0;
	  if(x2 <= pi[mk2][0]){
		// Get an accurate estimated value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
        MOSP_SSPR_Node *mx2nd = &Nodes[pi[mk2][x2]];
		MOSP_SSPR_Node *m_xnd = &Nodes[v.m_nxt];
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
		MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0;
	  if(x2 < pi[mk2][0]){
		MOSP_SSPR_Node *mx2nxt = &Nodes[pi[mk2][x2+1]];
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void MOSP_SSPR_Solution::alpha_omega_B1(int j, int x1, int x2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  int l = tau[j][0];

  apr_alpha = 1;
  apr_omega = l - (x2 - x1);

  for(int x=x1; x <= x2; x++){  // of course x1 <= x2
	MOSP_SSPR_Node *v = &Nodes[tau[j][x]];
	int v_apr_alpha, v_apr_omega;

	if(v->m_prv == -1 || x1 == 1) v_apr_alpha = 1;
	else{
	  v_apr_alpha = x1;
	  MOSP_SSPR_Node *mprv = &Nodes[v->m_prv];
	  MOSP_SSPR_Node *cur = &Nodes[tau[j][x1]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		v_apr_alpha--;
	  }
	}

	if(v->m_nxt == -1 || x2 == l) v_apr_omega = l - (x2 - x1);
	else{
	  v_apr_omega = x1;
	  MOSP_SSPR_Node *mnxt = &Nodes[v->m_nxt];
	  MOSP_SSPR_Node *cur = &Nodes[tau[j][x2]];
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		v_apr_omega++;
	  }
	}

	if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
  }
}

void MOSP_SSPR_Solution::alpha_omega_B2(int m1, int m2, int x1, int x2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  if(m1 == m2){ // if same machine
	int l = pi[m1][0];

	apr_alpha = 1;
	apr_omega = l - (x2 - x1);

	for(int x = x1; x <= x2; x++){
	  MOSP_SSPR_Node *v = &Nodes[pi[m1][x]];
	  int v_apr_alpha, v_apr_omega;

	  if(v->j_prv == -1 || x1 == 1) v_apr_alpha = 1;
	  else{
		v_apr_alpha = x1;
		MOSP_SSPR_Node *jprv = &Nodes[v->j_prv];
		MOSP_SSPR_Node *cur = &Nodes[pi[m1][x1]];
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  v_apr_alpha--;
		}
	  }

	  if(v->j_nxt == -1 || x2 == l) v_apr_omega = l - (x2 - x1);
	  else{
		MOSP_SSPR_Node *jnxt = &Nodes[v->j_nxt];
		MOSP_SSPR_Node *cur = &Nodes[pi[m1][x2]];
		v_apr_omega = x1;
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  v_apr_omega++;
		}
	  }

	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
  else{ // in case of different machines
	int l = pi[m2][0];

	apr_alpha = 1;
	apr_omega = l + 1;

	for(int x = x1; x <= x2; x++){
	  MOSP_SSPR_Node *v = &Nodes[pi[m1][x]];
	  int v_apr_alpha, v_apr_omega;

	  if(v->j_prv == -1) v_apr_alpha = 1;
	  else{
		v_apr_alpha = l + 1;
		MOSP_SSPR_Node *jprv = &Nodes[v->j_prv];
		MOSP_SSPR_Node *cur;

		if(LastmNode[m2] != -1){
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(cur->estart + cur->weight < jprv->estart) break;
			v_apr_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);
		}
	  }

	  if(v->j_nxt == -1) v_apr_omega = l + 1;
	  else{
		v_apr_omega = 1;
		MOSP_SSPR_Node *jnxt = &Nodes[v->j_nxt];
		MOSP_SSPR_Node *cur;

		if(FirstmNode[m2] != -1){
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(jnxt->tail - jnxt->weight > cur->tail) break;
			v_apr_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);
		}
	  }

	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
}

void MOSP_SSPR_Solution::N1Results(int v1, int &alpha, int &omega, int *&apr_Cmax, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N1 and evaluate the approximate Cmax
  // values at each insertion position between alpha and omega inclusive. The
  // approximate Cmax values are returned in Cmax array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of work centers of the given problem + 1. The values
  // of Cmax between alpha and omega are only assigned. Cmax is not a zero-based
  // array. The value of minX is the position that has the lowest value of
  // approximate Cmax other than the current position.
  //
  memset((void *)apr_Cmax, -1, sizeof(int) * sspr->nWCs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX =  -1;
	return;
  }

  alpha_omega_N1(v1, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  int phi = phi_N1(v1);

  MOSP_SSPR_Node &v = Nodes[v1];
  int x = tau_pos[v1];
  int minCmax = MAXINT;

  minX = 0;  // In case that there is only one operation in the job,
			 // the value of minX will be zero since there is only one position
			 // in the job which is the current position.

  // Get an accurate estimate of the nodes tail value in network Gamma-1
  // based on the values provided in Gamma network....
  //
  MOSP_SSPR_Node *j_xnd;
  if(v.j_prv != -1){
	j_xnd = &Nodes[v.j_prv];
	int gjv1nxt = (v.j_nxt != -1)? Nodes[v.j_nxt].tail : 0;
	int gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
	int g = j_xnd->weight + ((gjv1nxt > gmx1)? gjv1nxt : gmx1);
	etgmx_1[x-1] = g;
	for(int _x = x - 2; _x >= alpha; _x--){
	  j_xnd = &Nodes[j_xnd->j_prv];
	  gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
	  g = j_xnd->weight + ((g > gmx1)? g : gmx1);
	  etgmx_1[_x] = g;
	}
  }

  // Get an accurate estimated value of eta in the network Gamma-1
  // based on the values available in network Gamma...
  //
  if(v.j_nxt != -1){
	j_xnd = &Nodes[v.j_nxt];
	int etjv1prv = (v.j_prv != -1)? Nodes[v.j_prv].estart + Nodes[v.j_prv].weight : 0;
	int etmx1 = (j_xnd->m_prv != -1)? Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight : 0;
	int e = (etjv1prv > etmx1)? etjv1prv : etmx1;
	etgmx_1[x+1] = e;
	for(int _x = x + 2; _x <= omega; _x++){
	  int prvwt = j_xnd->weight;
	  j_xnd = &Nodes[j_xnd->j_nxt];
	  etmx1 = (j_xnd->m_prv != -1)? (Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight) : 0;
	  e = (e + prvwt > etmx1)? (e + prvwt) : etmx1;
	  etgmx_1[_x] = e;
	}
  }

  int hat_delta, check_delta;
  for(int x1=alpha; x1<=omega; x1++){
	if(x1 == x){
	  hat_delta = 0;
	  if(x1 != 1){
		MOSP_SSPR_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
		hat_delta += (jx1prv.estart + jx1prv.weight);
	  }
	  if(v.m_prv != -1){
		MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = 0;
	  if(x1 < tau[jb[v1]][0]){
		MOSP_SSPR_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
		check_delta += jx1nxt.tail;
	  }
	  if(v.m_nxt != -1){
		MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}
	else if(x1 < x){
	  hat_delta = 0;
	  if(x1 != 1){
		MOSP_SSPR_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
		hat_delta += (jx1prv.estart + jx1prv.weight);
	  }
	  if(v.m_prv != -1){
		MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = etgmx_1[x1];
	  if(v.m_nxt != -1){
		MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}
	else{
	  MOSP_SSPR_Node &jx1nd = Nodes[tau[jb[v1]][x1]];
	  hat_delta = (etgmx_1[x1] + jx1nd.weight);

	  if(v.m_prv != -1){
		MOSP_SSPR_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = 0;
	  if(x1 < tau[jb[v1]][0]){
		MOSP_SSPR_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
		check_delta += jx1nxt.tail;
	  }
	  if(v.m_nxt != -1){
		MOSP_SSPR_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}

	int delta = ((hat_delta > 0)? hat_delta : 0) + ((check_delta > 0 )? check_delta : 0);
	apr_Cmax[x1] = delta + phi;
	if(apr_Cmax[x1] < minCmax && x1 != x){
	  minCmax = apr_Cmax[x1];
	  minX = x1;
	}
  }
}

void MOSP_SSPR_Solution::N2Results(int v2, int k2, int &alpha, int &omega, int *&apr_Cmax, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N2 and evaluate the approximate Cmax
  // values at each insertion position between alpha and omega inclusive. The
  // approximate Cmax values are returned in Cmax array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of jobs of the given problem + 1. The values
  // of Cmax between alpha and omega are only assigned. Cmax is not a zero-based
  // array.
  //
  memset((void *)apr_Cmax, -1, sizeof(int) * sspr->nJobs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX = -1;
	return;
  }

  alpha_omega_N2(v2, k2, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  int phi = phi_N2(v2, k2);

  int hat_delta, check_delta;
  MOSP_SSPR_Node &v = Nodes[v2];
  int minCmax = MAXINT;

  if(k2 != mc[v2]){ // if different machine
	for(int x2=alpha; x2<=omega; x2++){
	  hat_delta = 0;
	  check_delta = 0;

	  if(v.j_prv != -1) hat_delta -= (Nodes[v.j_prv].estart +  Nodes[v.j_prv].weight);

	  if(x2 != 1){
		MOSP_SSPR_Node *ndx2prv = &Nodes[pi[k2][x2-1]];
		hat_delta += (ndx2prv->estart + ndx2prv->weight);
	  }

	  if(v.j_nxt != -1) check_delta -= Nodes[v.j_nxt].tail;

	  int l = pi[k2][0];
	  if(x2 != l+1){
		MOSP_SSPR_Node *ndx2nxt = &Nodes[pi[k2][x2]];
		check_delta += ndx2nxt->tail;
	  }

	  int delta = ((hat_delta > 0)? hat_delta : 0) + ((check_delta > 0 )? check_delta : 0);
	  apr_Cmax[x2] = delta + phi;
	  if(apr_Cmax[x2] < minCmax){
		minCmax = apr_Cmax[x2];
		minX = x2;
	  }
	}
  }
  else{
	minX = 0;  // In case that there is only one operation in the machine,
			   // the value of minX will be zero since there is only one position
			   // in the machine which is the current position.

	int x = pi_pos[v2];

	// Get an accurate estimate of the nodes tail value in network Gamma-1
	// based on the values provided in Gamma network....
	//
	MOSP_SSPR_Node *m_xnd;
	if(v.m_prv != -1){
	  m_xnd = &Nodes[v.m_prv];
	  int gmv2nxt = (v.m_nxt != -1)? Nodes[v.m_nxt].tail : 0;
	  int gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
	  int g = m_xnd->weight + ((gmv2nxt > gjx2)? gmv2nxt : gjx2);
	  etgmx_2[x-1] = g;
	  for(int _x = x - 2; _x >= alpha; _x--){
		m_xnd = &Nodes[m_xnd->m_prv];
		gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
		g = m_xnd->weight + ((g > gjx2)? g : gjx2);
		etgmx_2[_x] = g;
	  }
	}

	// Get an accurate estimated value of eta in the network Gamma-1
	// based on the values available in network Gamma...
	//
	if(v.m_nxt != -1){
	  m_xnd = &Nodes[v.m_nxt];
	  int etmv2prv = (v.m_prv != -1)? Nodes[v.m_prv].estart + Nodes[v.m_prv].weight : 0;
	  int etjx2 = (m_xnd->j_prv != -1)? Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight : 0;
	  int e = (etmv2prv > etjx2)? etmv2prv : etjx2;
	  etgmx_2[x+1] = e;
	  for(int _x = x + 2; _x <= omega; _x++){
		int prvwt = m_xnd->weight;
		m_xnd = &Nodes[m_xnd->m_nxt];
		etjx2 = (m_xnd->j_prv != -1)? (Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight) : 0;
		e = (e + prvwt > etjx2)? (e + prvwt) : etjx2;
		etgmx_2[_x] = e;
	  }
	}

	for(int x2=alpha; x2<=omega; x2++){
	  if(x2 == x){
		hat_delta = 0;
		if(x2 != 1){
		  MOSP_SSPR_Node *mx2prv = &Nodes[pi[k2][x2-1]];
		  hat_delta += (mx2prv->estart + mx2prv->weight);
		}
		if(v.j_prv != -1){
		  MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = 0;
		if(x2 < pi[k2][0]){
		  MOSP_SSPR_Node *mx2nxt = &Nodes[pi[k2][x2+1]];
		  check_delta += mx2nxt->tail;
		}
		if(v.j_nxt != -1){
		  MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }
	  else if(x2 < x){
		hat_delta = 0;
		if(x2 != 1){
		  MOSP_SSPR_Node *mx2prv = &Nodes[pi[k2][x2-1]];
		  hat_delta += (mx2prv->estart + mx2prv->weight);
		}
		if(v.j_prv != -1){
		  MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = etgmx_2[x2];
		if(v.j_nxt != -1){
		  MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }
	  else{
		MOSP_SSPR_Node *mx2nd = &Nodes[pi[k2][x2]];
		hat_delta = (etgmx_2[x2] + mx2nd->weight);
		if(v.j_prv != -1){
		  MOSP_SSPR_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = 0;
		if(x2 < pi[k2][0]){
		  MOSP_SSPR_Node *mx2nxt = &Nodes[pi[k2][x2+1]];
		  check_delta += mx2nxt->tail;
		}
		if(v.j_nxt != -1){
		  MOSP_SSPR_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }

	  int delta = ((hat_delta > 0)? hat_delta : 0) + ((check_delta > 0 )? check_delta : 0);
	  apr_Cmax[x2] = delta + phi;
	  if(apr_Cmax[x2] < minCmax && x2 != x){
		minCmax = apr_Cmax[x2];
		minX = x2;
	  }
	}
  }
}

#ifdef PRNT_SSPR_DBG
void MOSP_SSPR_Solution::AssertVectors(char *heading)
{
  int m, j;
  char buf[256];
  fprintf(ssprDbgFile, "\n\n%s:\n\n", heading);
  fprintf(ssprDbgFile, "\ta) pi\'s:\n");
  for(m=0; m<sspr->nMCs; m++){
	int l = pi[m][0];
	sprintf(buf, "\t\t%s [%i]: ", sspr->M[m]->ID(), l);
	fprintf(ssprDbgFile, buf);
	if(l <= 0) fprintf(ssprDbgFile, "\n");
	for(int i=1; i<=l; i++){
	  MOSP_Problem::SOperation *op = sspr->O[pi[m][i]];
	  sprintf(buf, " %s ", op->ID());
	  fprintf(ssprDbgFile, buf);
	  if(i<l) sprintf(buf, "->");
	  else sprintf(buf, "\n");
	  fprintf(ssprDbgFile, buf);
	}
  }
  fprintf(ssprDbgFile, "\n\tb) tau\'s:\n");
  for(j=0; j<sspr->nJobs; j++){
    int l = tau[j][0];
	sprintf(buf, "\t\t%s [%i]: ", sspr->J[j]->ID(), l);
	fprintf(ssprDbgFile, buf);
	if(l<=0) fprintf(ssprDbgFile, "\n");
	for(int i=1; i<=l; i++){
	  MOSP_Problem::SOperation *op = sspr->O[tau[j][i]];
	  sprintf(buf, " %s ", op->ID());
	  fprintf(ssprDbgFile, buf);
	  if(i<l) sprintf(buf, "->");
	  else sprintf(buf, "\n");
	  fprintf(ssprDbgFile, buf);
	}
  }
  fprintf(ssprDbgFile, "\n\n");
  fflush(ssprDbgFile);
}

void MOSP_SSPR_Solution::AssertNetwork(char *heading)
{
  char buf[256];
  fprintf(ssprDbgFile, "\n\n%s:\n\n", heading);
  fprintf(ssprDbgFile, "\ta) List of Nodes:\n\n");
  fprintf(ssprDbgFile, "\tNo.\tID\ttau_pos\tpi_pos\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\tweight\testart\tlstart\ttail\tslack\tRank\tCritical\n");
  for(int i=0; i<sspr->nOps; i++){
	sprintf(buf, "\t%i\t%s\t%i\t%i\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%c\n",
				 i, sspr->O[i]->ID(), tau_pos[i], pi_pos[i], Nodes[i].j_prv, Nodes[i].j_nxt,
				 sspr->M[mc[i]]->ID(), Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? '*' : ' '));
	fprintf(ssprDbgFile, buf);
  }
  fprintf(ssprDbgFile, "\n\n\tb) List of Jobs:\n\n");
  fprintf(ssprDbgFile, "\tID\tFirst\tLast\n");
  for(int j=0; j<sspr->nJobs; j++){
	sprintf(buf, "\t%s\t%i\t%i\n", sspr->J[j]->ID(), FirstjNode[j], LastjNode[j]);
	fprintf(ssprDbgFile, buf);
  }
  fprintf(ssprDbgFile, "\n\n\tc) List of Machines:\n\n");
  fprintf(ssprDbgFile, "\tID\tFirst\tLast\n");
  for(int m=0; m<sspr->nMCs; m++){
	sprintf(buf, "\t%s\t%i\t%i\n", sspr->M[m]->ID(), FirstmNode[m], LastmNode[m]);
	fprintf(ssprDbgFile, buf);
  }
  sprintf(buf, "\n\t\tCmax = %i\n\n", Cmax);
  fprintf(ssprDbgFile, buf);
  fflush(ssprDbgFile);
}

void MOSP_SSPR_Solution::AssertTasks(char *heading)
{
  char buf[256];
  sprintf(buf, "%s:\n\n", heading);
  fprintf(ssprDbgFile, buf);
  sprintf(buf, "\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
  fprintf(ssprDbgFile, buf);
  for(int i=0; i<sspr->nOps; i++){
	sprintf(buf, "\t%i\t%s\t%s\t%s\t%i\t%i\n",
				  i, sspr->tsk[i]->Operation->ID(), sspr->tsk[i]->Operation->Job->ID(),
				  sspr->tsk[i]->SelectedMC->ID(), (int)(sspr->tsk[i]->StartTime),
				  (int)(sspr->tsk[i]->EndTime));
	fprintf(ssprDbgFile, buf);
  }
  sprintf(buf, "\n\tCmax = %i\n\n", (int)sspr->Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN));
  fprintf(ssprDbgFile, buf);
  fflush(ssprDbgFile);
}

#endif

// ----------------------------------------------------- MOSP_SSPR ssprorithm

MOSP_SSPR::MOSP_SSPR(MOSP_Problem *P, int ps, int dth, int tsz, int ntsitr,
					 int ntsimpitr, int nbstmvs, int nrndmvs, int nitr, int Epsl)
	: MOSP_Algorithm(P, "Scatter Search with Path Relinking for MOSP")
{
  pSize = ps;
  deltaThreshold = dth;
  TabuSize = tsz;
  nTSiterations = ntsitr;
  nImpIterations = ntsimpitr;
  nBestMoves = nbstmvs;
  nRandMoves = nrndmvs;
  nIterations = nitr;
  Epsilon = Epsl;

  bestSol = NULL;
  bestLocalSol = NULL;

	  #ifdef DEBUG_SSPR_RESULTS
		#ifdef PRNT_SSPR_DBG
		  ssprDbgFile = fopen("ssprDebug.txt", "a+");
		#endif
		seedSol = NULL;
	  #endif
}

MOSP_SSPR::~MOSP_SSPR()
{
  if(bestSol) delete bestSol;
  if(bestLocalSol) delete bestLocalSol;

	  #ifdef DEBUG_SSPR_RESULTS
		#ifdef PRNT_SSPR_DBG
		  fclose(ssprDbgFile);
        #endif
		if(seedSol) delete seedSol;
	  #endif
}

void MOSP_SSPR::AllocateMemory()
{
  MOSP_Algorithm::AllocateMemory();

  // Allocate the memory needed by the random dense schedule generator
  MCrt = new int[nMCs];
  bMCready = new bool[nMCs];
  bMCdone = new bool[nMCs];
  Jnft = new int[nJobs];
  bJcompleted = new bool[nJobs];
  bJM = new bool *[nJobs];
  for (int i = 0; i < nJobs; i++) bJM[i] = new bool[nMCs];
  SchedulableJobs = new bool[nJobs];
  SelJob = new int[nMCs];
  bScheduled = new bool[nOps];
  SumSchedTime = new int[nMCs];
  //------------------------------------------------------------------

  // Allocated memory needed by the distance calculator
  int maxnMCs = 0;  // Maximum number of machines in a given work cetner
  for(int w=0; w<nWCs; w++)
	if(Wz[w] - Wa[w] + 1 > maxnMCs) maxnMCs = Wz[w] - Wa[w] + 1;
  mxsz = (nJobs+maxnMCs > nWCs)? (nJobs+maxnMCs) : nWCs;
  mxsz++;
  r = new MOSP_SubPerumutation[mxsz];
  a = new char *[mxsz];
  for(int i=0; i<mxsz; i++) a[i] = new char[mxsz];
  Suma_i = new int[mxsz];
  Suma_j = new int[mxsz];

  // Allocate the memory needed by Tabu Search
  TabuMoves = new NSMove[TabuSize];
  //------------------------------------------------------------------

  // Allocate the memory needed by Path Relinking
  jbDist = new int[nJobs];
  wcDist = new int[nWCs];
  bUseTau = new bool[nJobs];
  bUseVPi = new bool[nWCs];
  tmpTau = new int[nWCs+1];
  tmpPi = new int[nJobs+1];
  //------------------------------------------------------------------

  bestMoves = new NSMove [nBestMoves];  // Used locally in the iterations to
										// choose the best neighborhood move

  apr_Cmax = new int[((nWCs>nJobs)? nWCs : nJobs)+1];  // Used for local NB evaluations
}

void MOSP_SSPR::Initialize()
{
  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);

  bestSol = DSG(bestCmax, 1);    // Stores the best found solution throughout iterations
  bestSol->GenerateNetwork();
  bestSol->InterpretNetwork();

  bestLocalSol = new MOSP_SSPR_Solution(this);  // Stores best TS local solution
  bestLocalSol->Copy(bestSol);

  if(frmSSPRCalcProgress->bActive){
	frmSSPRCalcProgress->pbSSPRIterations->Min = 0;
	frmSSPRCalcProgress->pbSSPRIterations->Max = nIterations;
	frmSSPRCalcProgress->pbConstruction->Min = 0;
	frmSSPRCalcProgress->pbConstruction->Max = pSize;
	frmSSPRCalcProgress->grdInitialSolutions->RowCount = pSize + 1;
	frmSSPRCalcProgress->pbPRProgress->Min = 0;
	frmSSPRCalcProgress->sgPRIterations->RowCount = nIterations * pSize + 1;
	frmSSPRCalcProgress->ClearForm();
  }
  if(frmSSPRMultiRuns->bActive){
	frmSSPRMultiRuns->pbSSPRIterations->Min = 0;
	frmSSPRMultiRuns->pbSSPRIterations->Max = nIterations;
	frmSSPRMultiRuns->pbConstruction->Min = 0;
	frmSSPRMultiRuns->pbConstruction->Max = pSize;
  }

  CalculateLowerBound();

  if(frmSSPRCalcProgress->bActive){
	frmSSPRCalcProgress->lblLB->Caption = IntToStr(LB);
	frmSSPRCalcProgress->lblbestCmax->Caption = IntToStr(bestCmax);
  }
  if(frmSSPRMultiRuns->bActive){
	frmSSPRMultiRuns->lblLB->Caption = IntToStr(LB);
	frmSSPRMultiRuns->lblbestCmax->Caption = IntToStr(bestCmax);
  }

  if(bestCmax == LB) bOptSolFound = true;
  else{
	bOptSolFound = false;
	if(deltaThreshold == -1) SuggestDeltaThreshold();
	ConstructInitialPopulation();
  }

  sspritr = PRSolCounter = 1;
  bTerminate = false;
}

void MOSP_SSPR::Improve()
{
  // Clear all solutions in set U
  for(SList::SNode *nd = U.head(); nd; nd = nd->Next()){
	MOSP_SSPR_Solution *uS = (MOSP_SSPR_Solution *)nd->Data();
	delete uS;
  }
  U.Clear();

  int nPR = R.Count() / 4;
  if(nPR < 2) nPR = 2;

  if(frmSSPRCalcProgress->bActive){
	frmSSPRCalcProgress->pbPRProgress->Max = nPR;
	frmSSPRCalcProgress->pbPRProgress->Position = 0;
  }

  int OriginalCmax = bestCmax;

  while(nPR != 0){
	// Select two random solutions
	MOSP_SSPR_Solution *S1 = (MOSP_SSPR_Solution *)R[rnd(0, R.Count()-1)];
	MOSP_SSPR_Solution *S2;
	do{
	  S2 = (MOSP_SSPR_Solution *)R[rnd(0, R.Count()-1)];
	}while(S1 == S2);  // Make sure that the two solutions are different

	// Verfy that Cmax(S1) >= Cmax(S2); if not, swqp the two solutions
	if(S2->Cmax > S1->Cmax){
	  MOSP_SSPR_Solution *S_ = S1;
	  S1 = S2;
	  S2 = S_;
	}

	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->pbPRProgress->Position = frmSSPRCalcProgress->pbPRProgress->Position + 1;
	  frmSSPRCalcProgress->lblPRIter->Caption = IntToStr(frmSSPRCalcProgress->pbPRProgress->Position);
	}
	Application->ProcessMessages();

	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->sgPRIterations->Cells[0][PRSolCounter] = IntToStr(PRSolCounter);
	  frmSSPRCalcProgress->sgPRIterations->Cells[1][PRSolCounter] = IntToStr(S1->Cmax);
	  frmSSPRCalcProgress->sgPRIterations->Cells[2][PRSolCounter] = IntToStr(S2->Cmax);
	}

	MOSP_SSPR_Solution *S3 = PathRelinking(S1, S2);  // S3 is newly created solution

	// Show initial makespan value (before applying TS) in the list
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->sgPRIterations->Cells[3][PRSolCounter] = IntToStr(S3->Cmax);
    }
	Application->ProcessMessages();

	if(S3->Cmax != -1){    // This condition should not happen anywau!!!
	  TabuSearch(S3);
	  U.Add((void *)S3);
	}

	// Show final makespan value (after applying TS) in the list
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->sgPRIterations->Cells[4][PRSolCounter] = IntToStr(S3->Cmax);
    }
	Application->ProcessMessages();

	PRSolCounter++;

	nPR--;
  }

  // Examine updating R using solutions in U
  R.QSort(MOSPSSPRSolComp, ASCENDING);
  U.QSort(MOSPSSPRSolComp, ASCENDING);

  MOSP_SSPR_Solution *USol, *RSol;
  SList::SNode *Rnd;
  SList::SNode *Und = U.head();
  int udistjb, udistwc, udist;
  int rdistjb, rdistwc, rdist;
  if(Und != NULL){
	USol = (MOSP_SSPR_Solution *)Und->Data();
	udist = Distance(USol, bestSol, udistjb, udistwc);
	for(Rnd = R.head(); Rnd != NULL; Rnd = Rnd->Next()){
	  MOSP_SSPR_Solution *RSol = (MOSP_SSPR_Solution *)Rnd->Data();
	  if(USol->Cmax < RSol->Cmax){
		RSol->Copy(USol);
		Und = Und->Next();
		if(Und){
		  USol = (MOSP_SSPR_Solution *)Und->Data();
		  udist = Distance(USol, bestSol, udistjb, udistwc);
		}
		else break;
	  }
	  else if(USol->Cmax == RSol->Cmax){
		rdist = Distance(RSol, bestSol, rdistjb, rdistwc);
		if(udist >= rdist){
		  RSol->Copy(USol);
		  Und = Und->Next();
		  if(Und){
			USol = (MOSP_SSPR_Solution *)Und->Data();
			udist = Distance(USol, bestSol, udistjb, udistwc);
		  }
		  else break;
		}
	  }
	  else{
		Und = Und->Next();
		if(Und){
		  USol = (MOSP_SSPR_Solution *)Und->Data();
		  udist = Distance(USol, bestSol, udistjb, udistwc);
		}
		else break;
	  }
	  Application->ProcessMessages();
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
  }

  // If R does not change, diversify
  int ms;
  if(bestCmax == OriginalCmax){
	int nNewSol = 0;
	Rnd = R.tail();
	while(nNewSol < R.Count() / 2){
	  MOSP_SSPR_Solution *RSol = (MOSP_SSPR_Solution *)Rnd->Data();

	  //int newMakespan;
	  MOSP_SSPR_Solution *newSol = ConstructSolution();//DSG(newMakespan, 0);
	  newSol->GenerateNetwork();
	  if(newSol->InterpretNetwork()){
		Rnd->SetData((void *)newSol);
		delete RSol;
	  }
	  nNewSol++;

	  Rnd = Rnd->Prev();
	  if(!Rnd) break;

	  Application->ProcessMessages();
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
  }
}

bool MOSP_SSPR::Stop()
{
		   #ifdef PRNT_SSPR_DBG
			  char buf[256];
		   #endif

  bool bStop = false;

  if(bTerminate){
		   #ifdef PRNT_SSPR_DBG
			  sprintf(buf, "\n--> SSPR Algorithm is terminated! <--\n");
			  fprintf(ssprDbgFile, buf);
		   #endif
	bStop = true;
  }
  if(bOptSolFound){
		   #ifdef PRNT_SSPR_DBG
			  sprintf(buf, "\n--> Optimal solution is found! <--\n");
			  fprintf(ssprDbgFile, buf);
		   #endif
	bStop = true;
  }

  if(!bStop){
	sspritr++;
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->pbSSPRIterations->Position = sspritr;
	}
	if(frmSSPRMultiRuns->bActive){
	  frmSSPRMultiRuns->pbSSPRIterations->Position = sspritr;
	}

	if(sspritr > nIterations){
		   #ifdef PRNT_SSPR_DBG
			  sprintf(buf, "\n--> All iterations completed! <--\n");
			  fprintf(ssprDbgFile, buf);
		   #endif
	  bStop = true;
	}
  }

  if(bStop){
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->btnStart->Enabled = true;
	  frmSSPRCalcProgress->btnClose->Enabled = true;
	  frmSSPRCalcProgress->btnTerminate->Enabled = false;
    }
	return true;
  }
  return false;
}

void MOSP_SSPR::Finalize()
{
  bestSol->ConvertNetToTasks();
  MOSP_Algorithm::Finalize();
  SolStatus = FEASIBLE;
}

void MOSP_SSPR::FreeMemory()
{
  MOSP_Algorithm::FreeMemory();

  // Free the memory used for constructing initial solutions
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
  //--------------------------------------------------------

  // Free the memory used for calculating distances
  delete [] r;
  for(int i=0; i<mxsz; i++) delete [] a[i];
  delete [] a;
  delete [] Suma_i;
  delete [] Suma_j;
  //--------------------------------------------------------

  delete [] TabuMoves;

  delete [] bestMoves;

  // Free the memory used for path relinking
  delete [] jbDist;
  delete [] wcDist;
  delete [] bUseTau;
  delete [] bUseVPi;
  delete [] tmpTau;
  delete [] tmpPi;
  //--------------------------------------------------------

  // Clear all solutions in set R
  for(SList::SNode *nd = R.head(); nd != NULL; nd = nd->Next()){
	MOSP_SSPR_Solution *S = (MOSP_SSPR_Solution *)nd->Data();
	delete S;
  }
  R.Clear();

  // Clear all solutions in set U
  for(SList::SNode *nd = U.head(); nd; nd = nd->Next()){
	MOSP_SSPR_Solution *uS = (MOSP_SSPR_Solution *)nd->Data();
	delete uS;
  }
  U.Clear();

  if(bestSol) delete bestSol;             bestSol = NULL;
  if(bestLocalSol) delete bestLocalSol;   bestLocalSol = NULL;

  delete [] apr_Cmax;
}

void MOSP_SSPR::ComputeObjectiveValue()
{
  MOSP_Algorithm::ComputeObjectiveValue();
}

void MOSP_SSPR::CalculateLowerBound()
{
  int lb1, lb2, lb3;
  LB = CalculateLowerBound(lb1, lb2, lb3);
}

int MOSP_SSPR::CalculateLowerBound(int &lb1, int &lb2, int &lb3)
{
   // -------------------------- Evaluate the lower bound for the given problem
   // ---- The first lower bound is based on the maximum of each job's minimum
   //      completion time. For each job, the minimum time needed to complete
   //      it on its processing centers is evaluated. Then the maximum of these
   //      is the LB.
   //
   lb1 = 0;
   for(int j=0; j<nJobs; j++){
	 int sum_min_pt = 0;
	 for(int i=Ja[j]; i<=Jz[j]; i++){
	   int w = O[i]->WC->i;
	   int minpt = MAXINT;
	   for(int k=Wa[w]; k<=Wz[w]; k++){
		 if(minpt > pt[j][k]) minpt = pt[j][k];
	   }
	   sum_min_pt += minpt;
	 }
	 if(sum_min_pt > lb1) lb1 = sum_min_pt;
   }

   // ---- The second lower bound is based on parallel machine preemptive
   //      relaxation for each wrokcenter.
   //
   int min_pt;
   float sum_pt, mxWClb = 0;
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)pcnd->Data();
	 float qw = wc->MCs.Count();

	 sum_pt = 0;
	 min_pt = MAXINT;
	 int w = wc->i;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   int pk = MAXINT; // Start with a large value for pk to find the minimum
	   int j = O[o]->Job->i;
	   for(int m=Wa[w]; m<=Wz[w]; m++){
		 if(pk > pt[j][m]) pk = pt[j][m];
	   }
	   if(min_pt > pk) min_pt = pk;
	   sum_pt += pk;
	 }

	 if(sum_pt != 0){
	   sum_pt /= qw;
	   if(sum_pt > mxWClb) mxWClb = sum_pt;
	   if(mxWClb < min_pt) mxWClb = min_pt;
	 }

	 pcnd = pcnd->Next();
   }
   lb2 = ceil(mxWClb);

   // ---- The third lower bound is based on Matta's lower bound. It uses
   //      the minimum processing time on a given center on all its machines
   //      as the processing time in the case of proportionate MOSP
   //
   lb3 = 0;
   float Nj;
   pcnd = Problem->WCs.head();
   while(pcnd){
	 MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)pcnd->Data();
	 float qw = wc->MCs.Count();

	 Nj = 0;
	 min_pt = MAXINT;
	 int w = wc->i;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   Nj++;
	   int pk = MAXINT; // Start with a large value for pk to find the minimum
	   int j = O[o]->Job->i;
	   for(int m=Wa[w]; m<=Wz[w]; m++){
		 if(pk > pt[j][m]) pk = pt[j][m];
	   }
	   if(min_pt > pk) min_pt = pk;
	 }

	 int WClb = ceil(Nj / qw) * min_pt;
	 if(WClb > lb3) lb3 = WClb;

	 pcnd = pcnd->Next();
   }

   int LB = (lb1 < lb2)? lb2 : lb1;
   LB = (LB < lb3)? lb3 : LB;

   return LB;
}

int MOSP_SSPR::SuggestDeltaThreshold(double DivFactor)
{
   // This function is intented to suggest a delta threshold value for solutions
   // acceptance in the referecne set.
   float dTh = 0;

   // 1) For each workstation, count the number of jobs then divide that by 3
   // and then add the closest higher integer to the deltaTreshold value
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)pcnd->Data();
	 float Nj = 0;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   Nj++;
	 }
	 dTh += Nj/DivFactor;
	 pcnd = pcnd->Next();
   }

   // 2) For each job count the number of workstations that need to be visited
   // divide that by 3 and then add the closest higher integer to the
   // deltaThreshold value
   for(int j=0; j<nJobs; j++){
	 float nWC = Jz[j] - Jz[j] + 1;
	 dTh += nWC/DivFactor;
   }

   deltaThreshold = ceil(dTh);

   return deltaThreshold;
}

int MOSP_SSPR::ConstructInitialPopulation()
{
		   #ifdef PRNT_CONSTRUCTION_DBG
			  char buf[256];
		   #endif

  int nFails = 0;    // count the number of times the algorithm fails to add
					 // a constructed initial solution due to similarity

  while(R.Count() < pSize && nFails < pSize){
	MOSP_SSPR_Solution *newSol = ConstructSolution();

		   #ifdef PRNT_VNT_DBG
			  newSol->AssertVectors("Randomly generated solution using DSG:");
		   #endif

	TabuSearch(newSol);

		   #ifdef PRNT_VNT_DBG
			  newSol->AssertVectors("Modified solution after applying tabu search:");
		   #endif

	if(bOptSolFound) return R.Count();

	bool bAdd = true;
	for(SList::SNode *nd = R.head(); nd != NULL; nd = nd->Next()){
	  int jD, mD;
	  MOSP_SSPR_Solution *extantSol = (MOSP_SSPR_Solution *)nd->Data();
	  bool bDist = isDistanceGreaterThan(extantSol, newSol, deltaThreshold);

		   #ifdef PRNT_CONSTRUCTION_DBG
	          MOSP_SSPR_Solution *extantSol = (MOSP_SSPR_Solution *)nd->Data();
			  if(bDist) sprintf(buf, "\nComparison with Sol #%i: Distance > %i\n\n",
									R.IndexOf((void *)extantSol), deltaThreshold);
			  else sprintf(buf, "\nComparison with Sol #%i: Distance <= %i\n\n",
			                        R.IndexOf((void *)extantSol), deltaThreshold);
			  fprintf(ssprDbgFile, buf);
		   #endif

	  if(!bDist){
		bAdd = false;

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "Solution is not added!\n\n");
			  fprintf(ssprDbgFile, buf);
			  sprintf(buf, "-------------------------------------------------------------------------");
			  fprintf(ssprDbgFile, buf);
		   #endif

		break;
	  }
	}
	if(!bAdd){
	  delete newSol;
	  nFails++;
	  continue;
	}

	nFails = 0;
	R.Add((void *)newSol);

	int nConstructed = R.Count();
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->pbConstruction->Position = nConstructed;
	  frmSSPRCalcProgress->grdInitialSolutions->Cells[0][nConstructed] = IntToStr(nConstructed);
	  frmSSPRCalcProgress->grdInitialSolutions->Cells[1][nConstructed] = IntToStr(initialLocalCmax);
	  frmSSPRCalcProgress->grdInitialSolutions->Cells[2][nConstructed] = IntToStr(bestLocalCmax);
	}
	if(frmSSPRMultiRuns->bActive){
	  frmSSPRMultiRuns->pbConstruction->Position = nConstructed;
	}
	Application->ProcessMessages();

	if(bInterrupt){
	  bTerminate = true;
	  break;
	}

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "Solution is added  -> Cmax = %i\n\n", newSol->Cmax);
			  fprintf(ssprDbgFile, buf);
		   #endif

		   #ifdef PRNT_VNT_DBG
			  sprintf(buf, "Added solution #%i:", R.Count());
			  newSol->AssertVectors(buf);
		   #endif

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "-------------------------------------------------------------------------");
			  fprintf(ssprDbgFile, buf);
			  fflush(ssprDbgFile);
		   #endif
  }
		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "\n\nNumber of constructed solutions = %i", R.Count());
			  fprintf(ssprDbgFile, buf);
			  fflush(ssprDbgFile);
		   #endif

  return R.Count();
}

MOSP_SSPR_Solution *MOSP_SSPR::DSG(int &Makespan, int MSR)
{
  MOSP_SSPR_Solution *newSol = new MOSP_SSPR_Solution(this);

   // MSR = Machine selection rule:
   //      1 --> minimum work load
   //      2 --> minimum completion time
   //      3 --> random

  // Initialize scheduling parameters
  //
  Makespan = 0;
  int i, j, m;
  for(i=0; i<nMCs; i++) MCrt[i] = 0; //M[i]->ReadyTime;
  for(j=0; j<nJobs; j++){
	Jnft[j] = 0;  //J[j]->ReleaseTime;
	bJcompleted[j] = false;
  }
  for(m=0; m<nMCs; m++) SumSchedTime[m] = 0;
  for (j = 0; j < nJobs; j++)
	for(m=0; m<nMCs; m++)
	  bJM[j][m] = (pt[j][m] > 0)? true : false;

  // Steps of the DSG
  //
  int t;   // Current scheduling time

  // Start with all operations unscheduled
  for(i=0; i<nOps; i++) bScheduled[i] = false;

  // Initialize the bMCdone variables
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

  while(true){ // Main loop of the algorithm
	// Check for each machine if it is needed by any job or not
	bool bAllDone = true;
	for(m=0; m<nMCs; m++){
	  if(bMCdone[m]) continue;
	  bool b = true;
	  for(j=0; j<nJobs; j++){
		if(bJM[j][m]){
		  b = false;
		  bAllDone = false;
		  break;
		}
	  }
	  bMCdone[m] = b;
	}

	// If all machines are done, then stop the algorithm
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

	  // Now select a job from all schedulable jobs which has the minimum
	  // completion time. Tie-breaking is random.
	  //
	  int selj;
	  for(j=0; j<nJobs; j++){   // at least assign one arbitrary job first
		if(!SchedulableJobs[j]) continue;
		selj = j;
		break;
	  }
	  for(j++; j<nJobs; j++){  // Now select one
		if(!SchedulableJobs[j] || j == selj) continue;
		if(MSR != 0){
		  if(Jnft[j] + pt[j][m] < Jnft[selj] + pt[selj][m])	selj = j;
		  else if(Jnft[j] + pt[j][m] == Jnft[selj] + pt[selj][m]) selj = (flip(0.5))? j : selj;
		}
		else selj = (flip(0.5))? j : selj;
	  }
	  SelJob[m] = selj;
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections
	//
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
		if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  MSR = MSR % 3;
		  if(MSR == 1){
			// Select the machine with the minimum load
			if(SumSchedTime[m] <= SumSchedTime[m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else if(MSR == 2){
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
	  newSol->mc[i] = m;
	  newSol->jb[i] = j;
	  int jn = newSol->tau[j][0];
	  if(jn == -1) jn = 1;
	  else jn++;
	  newSol->tau[j][0] = jn;
	  newSol->tau[j][jn] = i;
	  newSol->tau_pos[i] = jn;
	  int mn = newSol->pi[m][0];
	  if(mn == -1) mn = 1;
	  else mn++;
	  newSol->pi[m][0] = mn;
	  newSol->pi[m][mn] = i;
	  newSol->pi_pos[i] = mn;

	  int StartTime = (Jnft[j] > MCrt[m])? Jnft[j] : MCrt[m];
	  Jnft[j] = StartTime + pt[j][m];
	  if(Jnft[j] > Makespan) Makespan = Jnft[j];  // <--------------------- Update Cmax
	  MCrt[m] = Jnft[j];
	  SumSchedTime[m] += pt[j][m];
	  bScheduled[i] = true;

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

  for(j=0; j<nJobs; j++) if(newSol->tau[j][0] == -1) newSol->tau[j][0] = 0;
  for(m=0; m<nMCs; m++)	if(newSol->pi[m][0] == -1) newSol->pi[m][0] = 0;

  return newSol;
}

MOSP_SSPR_Solution *MOSP_SSPR::ConstructSolution()
{
  int MSR = 0;
  int ms1, ms2;   /// Makespan of newly created solutions

  MOSP_SSPR_Solution *newSol = DSG(ms1, MSR);
  MOSP_SSPR_Solution *newSol2 = DSG(ms2, MSR);
  if(ms2 < ms1){
	newSol->Copy(newSol2);
    ms1 = ms2;
  }
  delete newSol2;

  for(MSR=1; MSR <= 6; MSR++){
	newSol2 = DSG(ms2, MSR);
	if(ms2 < ms1){
	  newSol->Copy(newSol2);
      ms1 = ms2;
	}
	delete newSol2;
  }

  return newSol;
}

void MOSP_SSPR::EnumerateMoves(MOSP_SSPR_Solution *Sol, bool bStrictImp)
{
  if(Sol->Cmax == -1) return;    // Network must be interpretted

  // Enumerate all possible moves and store the best ones in bestMoves.
  // Meanwhile, if debugging info is needed, output all possible moves
  // info and the best ones.
  //
  int alpha, omega, minX;

#ifdef PRNT_NSCB_DBG
  char buf[256];
  fprintf(ssprDbgFile, "\n\nList of all efficient neighborhood moves:\n\n");
  fprintf(ssprDbgFile, "\t\tOper.\tFunc.\talpha\tomega\tx\tk2\t~Cmax\n");
#endif

  nMoves = 0;
  for(int i=0; i<nOps; i++){
	MOSP_SSPR_Node &nd = Sol->Nodes[i];
	if(!nd.bCritical) continue;

	// Check for the conditions of propositions 2 and 4 before getting N1 parameters
	//
	if(bStrictImp){
	  minX = -1;
	  if(Sol->doesN1Improve(i)) Sol->N1Results(i, alpha, omega, apr_Cmax, minX);
	}
	else Sol->N1Results(i, alpha, omega, apr_Cmax, minX);

#ifdef PRNT_NSCB_DBG
	if(minX > 0){
	  sprintf(buf, "\t\t%s\tN1\t%i\t%i\t%i\t%s\t%i\n",
				    O[i]->ID(), alpha, omega, minX, "--", apr_Cmax[minX]);
	  fprintf(ssprDbgFile, buf);
	}
	else{
	  sprintf(buf, "\t\t%s\tN1\t%i\t%i\t%i\t--\t--\n",
					O[i]->ID(), alpha, omega, minX);
	  fprintf(ssprDbgFile, buf);
	}
#endif

	if(minX > 0){
	  if(nMoves == 0){
		bestMoves[0].F = MOSP_SSPR::N1;
		bestMoves[0].v = i;
		bestMoves[0].x = minX;
		bestMoves[0].aprCmax = apr_Cmax[minX];
		nMoves++;
	  }
	  else{
		bool bInserted = false;
		int m = 0;
		while(m < nMoves){
		  if(bestMoves[m].aprCmax > apr_Cmax[minX]){
			for(int n = nBestMoves-1; n > m; n--){
			  memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(MOSP_SSPR::NSMove));
			}
			bestMoves[m].F = MOSP_SSPR::N1;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].aprCmax = apr_Cmax[minX];

			bInserted = true;
			break;
		  }
		  m++;
		}
		if(!bInserted && nMoves < nBestMoves){
		  m = nMoves++;
		  bestMoves[m].F = MOSP_SSPR::N1;
		  bestMoves[m].v = i;
		  bestMoves[m].x = minX;
		  bestMoves[m].aprCmax = apr_Cmax[minX];
		}
	  }
	}

	SList::SNode *mcnd = O[i]->AltMCs.head();
	while(mcnd){
	  MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	  MOSP_Problem::SMC *mc = opmc->MC;

	  // Check for the conditions of propositions 2 and 4 before getting N1 parameters
	  //
	  if(bStrictImp){
		minX = -1;
		if(Sol->doesN2Improve(i)) Sol->N2Results(i, mc->i, alpha, omega, apr_Cmax, minX);
	  }
	  else Sol->N2Results(i, mc->i, alpha, omega, apr_Cmax, minX);

#ifdef PRNT_NSCB_DBG
	  if(minX > 0){
		sprintf(buf, "\t\t%s\tN2\t%i\t%i\t%i\t%s\t%i\n",
					 O[i]->ID(), alpha, omega, minX, mc->ID(), apr_Cmax[minX]);
		fprintf(ssprDbgFile, buf);
	  }
	  else{
		sprintf(buf, "\t\t%s\tN2\t%i\t%i\t%i\t%s\t--\n",
					 O[i]->ID(), alpha, omega, minX, mc->ID());
		fprintf(ssprDbgFile, buf);
	  }
#endif

	  if(minX > 0){
		if(nMoves == 0){
		  bestMoves[0].F = MOSP_SSPR::N2;
		  bestMoves[0].v = i;
		  bestMoves[0].x = minX;
		  bestMoves[0].m2 = mc->i;
		  bestMoves[0].aprCmax = apr_Cmax[minX];
		  nMoves++;
		}
		else{
		  bool bInserted = false;
		  int m = 0;
		  while(m < nMoves){
			if(bestMoves[m].aprCmax > apr_Cmax[minX]){

			  for(int n = nBestMoves-1; n > m; n--){
				memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(MOSP_SSPR::NSMove));
			  }
			  bestMoves[m].F = MOSP_SSPR::N2;
			  bestMoves[m].v = i;
			  bestMoves[m].x = minX;
			  bestMoves[m].m2 = mc->i;
			  bestMoves[m].aprCmax = apr_Cmax[minX];

			  bInserted = true;
			  break;
			}
			m++;
		  }
		  if(!bInserted && nMoves < nBestMoves){
			m = nMoves++;
			bestMoves[m].F = MOSP_SSPR::N2;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].m2 = mc->i;
			bestMoves[m].aprCmax = apr_Cmax[minX];
		  }
		}
	  }
	  mcnd = mcnd->Next();
	}
  }

#ifdef PRNT_NSCB_DBG
  fprintf(ssprDbgFile, "\n\nList of the best efficient neighborhood moves:\n\n");
  fprintf(ssprDbgFile, "\t\tOper.\tFunc.\tx\tk2\t~Cmax\n");

  for(int m=0; m<nMoves; m++){
	NSMove &mv = bestMoves[m];
	if(mv.F == N1)
	  sprintf(buf, "\t\t%s\tN1\t%i\t%s\t%i\n", O[mv.v]->ID(), mv.x, "--", mv.aprCmax);
	else
	  sprintf(buf, "\t\t%s\tN2\t%i\t%s\t%i\n", O[mv.v]->ID(), mv.x, M[mv.m2]->ID(), mv.aprCmax);
	fprintf(ssprDbgFile, buf);
  }
  fflush(ssprDbgFile);
#endif
}

int MOSP_SSPR::ApplyN(MOSP_SSPR_Solution *Sol, NSMove mv)
{
  // This function modivies the solution vectors by conducting the received move
  // mv, then the solution vectors are interpretted to network representation
  // and all start and finish times are calculated.
  // Note: It is assumed that the sent move is feasible.

  if(mv.F == N1){
	int j = Sol->jb[mv.v];
    int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->Cmax;  // There is nothing to do!
	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++){
		tau[x] = tau[x+1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--){
		tau[x] = tau[x-1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
  }
  else{
	int m1 = Sol->mc[mv.v];
	if(mv.m2 == m1){
	  int *&pi = Sol->pi[mv.m2];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->Cmax; // There is nothing to do
	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++){
		  pi[x] = pi[x+1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  else{
        int x;
		for(x=oldx; x>mv.x; x--){
		  pi[x] = pi[x-1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];
	  for(int x=curx; x<pi1[0]; x++){
		pi1[x] = pi1[x+1];
		Sol->pi_pos[pi1[x]] = x;
	  }
	  pi1[0]--;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0]){
		pi2[++pi2[0]] = mv.v;
		Sol->pi_pos[mv.v] = mv.x;
	  }
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--){
		  pi2[x] = pi2[x-1];
		  Sol->pi_pos[pi2[x]] = x;
		}
		pi2[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  Sol->mc[mv.v] = mv.m2;
	}
  }

	  #ifdef PRNT_VNT_DBG
		 Sol->AssertVectors("Vector representation after conducting selected move");
      #endif

  Sol->GenerateNetwork();
  if(Sol->InterpretNetwork()){
	if(Sol->Cmax < bestCmax){
	  bestCmax = Sol->Cmax;
	  bestSol->Copy(Sol);
	  if(bestCmax == LB){
		bOptSolFound = true;
	  }
	}
	return Sol->Cmax;
  }
  return -1;
}

int MOSP_SSPR::TauIntersectionSize(int j, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2)
{
  // This function returns the intersection set size for two solutions for the
  // perumtations of a given job
  if(S1->sspr != S2->sspr) return -1;

  int *&v1 = S1->tau[j];
  int *&v2 = S2->tau[j];
  int sz = v1[0];
  int z = 1;
  int x1 = 1;
  bool bDone = false;
  do{
	int i = v1[x1];
	int x2 = S2->tau_pos[i];
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > v1[0]){
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		z++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		z++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  return z;
}

int MOSP_SSPR::TauDistance(int j, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2)
{
  // This function returns the distance for two solutions for the
  // perumtations of a given job
  if(S1->sspr != S2->sspr) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(MOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  int *&v1 = S1->tau[j];
  int *&v2 = S2->tau[j];
  int sz = v1[0];

  nr = 0;
  int x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];
	int x2 = S2->tau_pos[i];
	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

		 #ifdef PRNT_DISTANCE_DBG
			 char buf[256];
			 sprintf(buf, "\n\nThe intersection set between permutations of job %s:\n", J[j]->ID());
			 fprintf(ssprDbgFile, buf);
			 if(nr==0){
			   sprintf(buf, "\tThe two permutations are found identical\n\n");
			   fprintf(ssprDbgFile, buf);
			 }
			 else{
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			   fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
		 #endif

  if(nr == 0) return 0;  // the two permutations are identical

  // Construct the initial subpermutation relative positions matrix
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\nThe subpermutations relative positions matrix:\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\tSum");
			 for(j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  // Now iterate on the "a" matrix to calculate the distance
  int Distance = 0;
  do{
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;

		 #ifdef PRNT_DISTANCE_DBG
			int toX;
		 #endif

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}

	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_Sa == 0) break;

	Distance++;
	if(Distance > nr) break;  // The distance cannot exceed the number of subpermutations

	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  Suma_i[max_ij] = 0;

	  // update the positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 >= min_Xj && r[i].x2 < r[max_ij].x2){
		  a[i][max_ij] = 1;
		  Suma_i[i]++;
		  Suma_j[max_ij]++;
		  r[i].x2 += r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 >= min_Xj && r[j].x2 < r[max_ij].x2){
		  a[max_ij][j] = 0;
		  Suma_j[j]--;
		  r[j].x2 += r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = min_Xj;

		 #ifdef PRNT_DISTANCE_DBG
			toX = min_Xj;
		 #endif
	}
	else{  // Forward move
	  int max_Xi = -1, max_i;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
			max_i = i;
		  }
		}
	  }
	  Suma_j[max_ij] = 0;

	  // update hte positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 > r[max_ij].x2 && r[i].x2 <= max_Xi){
		  a[i][max_ij] = 0;
		  Suma_i[i]--;
		  r[i].x2 -= r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 > r[max_ij].x2 && r[j].x2 <= max_Xi){
		  a[max_ij][j] = 1;
		  Suma_i[max_ij]++;
		  Suma_j[j]++;
		  r[j].x2 -= r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = r[max_i].x2 + r[max_i].l;

		 #ifdef PRNT_DISTANCE_DBG
			toX = max_Xi;
		 #endif
	}

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\n\tIteration #%i: ", Distance);
			 fprintf(ssprDbgFile, buf);
			 if(bInRow) fprintf(ssprDbgFile, "Backward ");
			 else fprintf(ssprDbgFile, "Forward ");
			 sprintf(buf, "move of subpermutation #%i to position %i\n", max_ij+1, toX);
			 fprintf(ssprDbgFile, buf);
			 fprintf(ssprDbgFile, "\tThe updated subpermutations:\n");
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\t\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			 sprintf(buf, "\n\n\tThe resultant subpermutations relative positions matrix:\n\t\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\t\tSum");
			 for(j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  }while(true);

  return Distance;
}

int MOSP_SSPR::WCIntersectionSize(int w, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2)
{
  // This function returns the intersection set size for two solutions for the
  // permutations among the machines of a given work center
  if(S1->sspr != S2->sspr) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(MOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  nr = 0;  // starting number of subpermutations is 0

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  // where rho_1 = pi^1_w,1 -> <d1> -> pi^1_w,2 -> <d2> ..... -> pi^1_w,q_w
  // and  rho_2 = pi^2_w,1 -> <d1> -> pi^2_w,2 -> <d2> ..... -> pi^2_w,q_w
  int nmcs = Wz[w] - Wa[w] + 1;    // number of machines in workcetner w
  int nops1 = 0, nops2 = 0;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	nops1 += S1->pi[m][0];
	nops2 += S2->pi[m][0];
  }
  if(nops1 != nops2) return -1; //  This should not happen anyway

  int sz = nops1 + nmcs - 1;
  int *v1 = new int[sz+1];
  int *v2 = new int[sz+1];
  v1[0] = v2[0] = sz;

  int x1=1, x2=1;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	int pisz1 = S1->pi[m][0];
	memcpy((void *)&v1[x1], &S1->pi[m][1], sizeof(int) * pisz1);
	if(m != Wz[w]){
	  x1 += pisz1;
	  v1[x1] = nOps + m;
	  x1++;
	}

	int pisz2 = S2->pi[m][0];
	memcpy((void *)&v2[x2], &S2->pi[m][1], sizeof(int) * pisz2);
	if(m != Wz[w]){
	  x2 += pisz2;
	  v2[x2] = nOps + m;
	  x2++;
	}
  }

  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];

	x2 = 0;
	if(i < nOps){
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		if(m2 != S2->mc[i]) x2 += (S2->pi[m2][0] + 1);
		else{
		  x2 += S2->pi_pos[i];
		  break;
		}
	  }
	}
	else{
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		x2 += (S2->pi[m2][0] + 1);
		if(m2 == i-nOps) break;
	  }
	}

	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  delete [] v1;
  delete [] v2;

  return nr;
}

int MOSP_SSPR::WCDistance(int w, MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2)
{
  // This function returns the distance between two solutions for the
  // permutations among the machines of a given work center
  if(S1->sspr != S2->sspr) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(MOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  nr = 0;  // starting number of subpermutations is 0

		 #ifdef PRNT_DISTANCE_DBG
			 char buf[256];
		 #endif

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  // where rho_1 = pi^1_w,1 -> <d1> -> pi^1_w,2 -> <d2> ..... -> pi^1_w,q_w
  // and  rho_2 = pi^2_w,1 -> <d1> -> pi^2_w,2 -> <d2> ..... -> pi^2_w,q_w
  int nmcs = Wz[w] - Wa[w] + 1;    // number of machines in workcetner w
  int nops1 = 0, nops2 = 0;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	nops1 += S1->pi[m][0];
	nops2 += S2->pi[m][0];
  }
  if(nops1 != nops2){  //  This should not happen anyway
		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\n\n-------> Unequal number of operations in workstation %s!!!! <-------\n", W[w]->ID());
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif
	return -1;
  }

  int sz = nops1 + nmcs - 1;
  int *v1 = new int[sz+1];
  int *v2 = new int[sz+1];
  v1[0] = v2[0] = sz;

  int x1=1, x2=1;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	int pisz1 = S1->pi[m][0];
	memcpy((void *)&v1[x1], &S1->pi[m][1], sizeof(int) * pisz1);
	if(m != Wz[w]){
	  x1 += pisz1;
	  v1[x1] = nOps + m;
	  x1++;
	}

	int pisz2 = S2->pi[m][0];
	memcpy((void *)&v2[x2], &S2->pi[m][1], sizeof(int) * pisz2);
	if(m != Wz[w]){
	  x2 += pisz2;
	  v2[x2] = nOps + m;
	  x2++;
	}
  }

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\n\nThe collective permutations of workstation %s are:\n", W[w]->ID());
			 fprintf(ssprDbgFile, buf);
			 sprintf(buf, "\n\tFor the first solution\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=1; i<=v1[0]; i++){
			   if(v1[i] < nOps) sprintf(buf, "%s ", O[v1[i]]->ID());
			   else sprintf(buf, "d%i ", v1[i] - nOps);
			   fprintf(ssprDbgFile, buf);
			   if(i != v1[0]) fprintf(ssprDbgFile, " -> ");
			   else fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
			 sprintf(buf, "\n\tFor the second solution\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=1; i<=v2[0]; i++){
			   if(v2[i] < nOps) sprintf(buf, "%s ", O[v2[i]]->ID());
			   else sprintf(buf, "d%i ", v2[i] - nOps);
			   fprintf(ssprDbgFile, buf);
			   if(i != v2[0]) fprintf(ssprDbgFile, " -> ");
			   else fprintf(ssprDbgFile, "\n");
			 }
			 fprintf(ssprDbgFile, "\n\n");
			 fflush(ssprDbgFile);
		 #endif

  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];

	x2 = 0;
	if(i < nOps){
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		if(m2 != S2->mc[i]) x2 += (S2->pi[m2][0] + 1);
		else{
		  x2 += S2->pi_pos[i];
		  break;
		}
	  }
	}
	else{
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
        x2 += (S2->pi[m2][0] + 1);
		if(m2 == i-nOps) break;
	  }
	}

	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "The intersection set between permutations of workstations %s:\n", W[w]->ID());
			 fprintf(ssprDbgFile, buf);
			 if(nr==0){
			   sprintf(buf, "\tThe two permutations are found identical\n\n");
			   fprintf(ssprDbgFile, buf);
			 }
			 else{
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			   fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
		 #endif

  if(nr == 0){   // the two permutations are identical
	delete [] v1;
	delete [] v2;
	return 0;
  }

  // Construct the initial subpermutation relative positions matrix
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\nThe subpermutations relative positions matrix:\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\tSum");
			 for(int j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  // Now iterate on the "a" matrix to calculate the distance
  int Distance = 0;
  do{
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;

		 #ifdef PRNT_DISTANCE_DBG
			int toX;
		 #endif

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}

	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_Sa == 0) break;

	Distance++;
	if(Distance > nr) break;  // The distance cannot exceed the number of subpermutations

	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  Suma_i[max_ij] = 0;

	  // update the positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 >= min_Xj && r[i].x2 < r[max_ij].x2){
		  a[i][max_ij] = 1;
		  Suma_i[i]++;
		  Suma_j[max_ij]++;
		  r[i].x2 += r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 >= min_Xj && r[j].x2 < r[max_ij].x2){
		  a[max_ij][j] = 0;
		  Suma_j[j]--;
		  r[j].x2 += r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = min_Xj;

		 #ifdef PRNT_DISTANCE_DBG
			toX = min_Xj;
		 #endif
	}
	else{  // Forward move
	  int max_Xi = -1, max_i;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
			max_i = i;
		  }
		}
	  }
	  Suma_j[max_ij] = 0;

	  // update hte positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 > r[max_ij].x2 && r[i].x2 <= max_Xi){
		  a[i][max_ij] = 0;
		  Suma_i[i]--;
		  r[i].x2 -= r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 > r[max_ij].x2 && r[j].x2 <= max_Xi){
		  a[max_ij][j] = 1;
		  Suma_i[max_ij]++;
		  Suma_j[j]++;
		  r[j].x2 -= r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = r[max_i].x2 + r[max_i].l;

		 #ifdef PRNT_DISTANCE_DBG
			toX = max_Xi;
		 #endif
	}

		 #ifdef PRNT_DISTANCE_DBG
			 sprintf(buf, "\n\tIteration #%i: ", Distance);
			 fprintf(ssprDbgFile, buf);
			 if(bInRow) fprintf(ssprDbgFile, "Backward ");
			 else fprintf(ssprDbgFile, "Forward ");
			 sprintf(buf, "move of subpermutation #%i to position %i\n", max_ij+1, toX);
			 fprintf(ssprDbgFile, buf);
			 fprintf(ssprDbgFile, "\tThe updated subpermutations:\n");
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\t\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			 sprintf(buf, "\n\n\tThe resultant subpermutations relative positions matrix:\n\t\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\t\tSum");
			 for(int j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  }while(true);

  delete [] v1;
  delete [] v2;

  return Distance;
}

int MOSP_SSPR::Distance(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2, int &jD, int &wcD)
{
  jD = wcD = 0;

  for(int j=0; j<nJobs; j++){
	jD += TauDistance(j, S1, S2);
  }
  for(int w=0; w<nWCs; w++){
	wcD += WCDistance(w, S1, S2);
  }

  return (jD + wcD);
}

bool MOSP_SSPR::isDistanceGreaterThan(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2, int DistLimit)
{
  int D = 0;

  for(int j=0; j<nJobs; j++){
	if(D > DistLimit) break;
	D += TauDistance(j, S1, S2);
  }

  for(int w=0; w<nWCs; w++){
	if(D > DistLimit) break;
	D += WCDistance(w, S1, S2);
  }

  if(D > DistLimit) return true;
  return false;
}

int MOSP_SSPR::SuggestTabuSize()
{
   // This function is intented to suggest a maximum tabu size value
   float max_tsz = 0;

   // 1) For each workstation, count the number of jobs then divide that by 3
   // and then add the closest higher integer to the maximum tabu size
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)pcnd->Data();
	 float Nj = 0;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   Nj++;
	 }
	 max_tsz += Nj/3.0;
	 pcnd = pcnd->Next();
   }

   // 2) For each job count the number of workstations that need to be visited
   // then divide that by 3 and then add the closest higher integer to the
   // maximum tabu size value
   for(int j=0; j<nJobs; j++){
	 float nWC = Jz[j] - Jz[j] + 1;
	 max_tsz += nWC/3.0;
   }

   max_tsz = ceil(max_tsz);

   return max_tsz;
}

void MOSP_SSPR::TabuSearch(MOSP_SSPR_Solution *S)
{
		   #ifdef PRNT_CONSTRUCTION_DBG
			  char buf[256];
		   #elif defined PRNT_TS_DBG
			  char buf[256];
		   #endif

  // Apply Tabu Search on the given solution to improve it
  S->GenerateNetwork();
  if(!S->InterpretNetwork()) return;

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "\nNew randomly generated solution #%i with Cmax = %i, and best found Cmax = %i\n\n", R.Count()+1, S->Cmax, bestCmax);
			  fprintf(ssprDbgFile, buf);
		   #endif

		   #ifdef PRNT_TS_DBG
			  sprintf(buf, "\nTS started with a solution having Cmax = %i, and best found Cmax so far = %i\n\n", S->Cmax, bestCmax);
			  fprintf(ssprDbgFile, buf);
		   #endif


  ClearTabus();
  bestLocalSol->Copy(S);
  initialLocalCmax = bestLocalCmax = S->Cmax;

  if(frmSSPRCalcProgress->bActive){
	frmSSPRCalcProgress->lblTSInitialCmax->Caption = IntToStr(initialLocalCmax);
	frmSSPRCalcProgress->lblTSCurrentCmax->Caption = IntToStr(initialLocalCmax);
	frmSSPRCalcProgress->lblTSBestCmax->Caption = IntToStr(bestLocalCmax);
	frmSSPRCalcProgress->lblTSIter->Caption = "0";
	frmSSPRCalcProgress->lblTSImpIter->Caption = "0";
	frmSSPRCalcProgress->pbTabuSearch->Min = 0;
	frmSSPRCalcProgress->pbTabuSearch->Max = nTSiterations;
	frmSSPRCalcProgress->pbTabuSearch->Position = 0;
  }

  int itr;             // current iteration number
  int itr_noimp;       // Number of iterations since last found best solution
  int itr_random;      // Number of random iterations
  itr = itr_noimp = itr_random = 0;

  while(itr < nTSiterations){
	if(bestLocalCmax < bestCmax){
	  bestCmax = bestLocalCmax;
	  bestSol->Copy(bestLocalSol);
	  if(frmSSPRCalcProgress->bActive){
		frmSSPRCalcProgress->lblbestCmax->Caption = IntToStr(bestCmax);
      }
	  if(frmSSPRMultiRuns->bActive){
		frmSSPRMultiRuns->lblbestCmax->Caption = IntToStr(bestCmax);
      }
	  if(bestCmax == LB){
		bOptSolFound = true;
		break;
	  }
	}

	EnumerateNonTabuMoves(S);

	if(nMoves == 0) break;  // This should not happen unless the tabu size is very large

	if(itr_noimp == nImpIterations){
	  itr_random = nRandMoves;
	  itr_noimp = 0;
	}

	int iselNSMove;       // index of selected move
	if(itr_random != 0){
	  iselNSMove = rnd(0, nMoves-1);
	  itr_random--;
	}
	else iselNSMove = 0;

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "itr %i: Selected move #%i\t", itr+1, iselNSMove);
			  fprintf(ssprDbgFile, buf);
		   #endif

	if(ApplyNandUpdateTabuList(S, bestMoves[iselNSMove]) == -1){   // Something went wrong
		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "-------- Wrong application of a neighborhood move !!!!!!\n");
			  fprintf(ssprDbgFile, buf);
		   #endif
	  break;
	};

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "--> Resultant Cmax = %i\n", S->Cmax);
			  fprintf(ssprDbgFile, buf);
		   #endif

	if(bestLocalCmax <= S->Cmax) itr_noimp++;
	else{
	  bestLocalSol->Copy(S);
	  bestLocalCmax = S->Cmax;
	  itr = itr_noimp = 0;

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "--> new best local Cmax = %i\n", bestLocalCmax);
			  fprintf(ssprDbgFile, buf);
		   #endif
	}
	itr++;

	Application->ProcessMessages();
	if(frmSSPRCalcProgress->bActive){
	  frmSSPRCalcProgress->lblTSCurrentCmax->Caption = IntToStr(S->Cmax);
	  frmSSPRCalcProgress->lblTSBestCmax->Caption = IntToStr(bestLocalCmax);
	  frmSSPRCalcProgress->lblTSIter->Caption = IntToStr(itr);
	  frmSSPRCalcProgress->lblTSImpIter->Caption = IntToStr(itr_noimp);
	  frmSSPRCalcProgress->pbTabuSearch->Position = itr;
	}

	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  S->Copy(bestLocalSol);

		   #ifdef PRNT_CONSTRUCTION_DBG
			  sprintf(buf, "\nSolution #%i after TS has Cmax = %i\n\n", R.Count()+1, S->Cmax);
			  fprintf(ssprDbgFile, buf);
			  fflush(ssprDbgFile);
		   #endif
}

void MOSP_SSPR::AddTabu(NSFunction F, int v, int x, int m)
{
  if(TabuSize == 0) return;
  if(TabuPntr == TabuSize) TabuPntr = 0;

  TabuMoves[TabuPntr].F = F;
  TabuMoves[TabuPntr].v = v;
  TabuMoves[TabuPntr].x = x;
  TabuMoves[TabuPntr].m2 = (F==N1)? -1 : m;

  TabuPntr++;
  if(nTabus < TabuSize) nTabus++;

	#ifdef PRNT_TS_DBG
			char buf[128];
			sprintf(buf, "\n\nCurrent number of tabus = %i\n", nTabus);
			fprintf(ssprDbgFile, buf);
			for(int i=0; i<nTabus; i++){
			  sprintf(buf, "\t(%s, %s, x = %i, m2 = %s)  %c\n",
							 ((TabuMoves[i].F == N1)? "N1" : ((TabuMoves[i].F == N2)? "N2" : "--")),
							 O[TabuMoves[i].v]->ID(), TabuMoves[i].x,
							 ((TabuMoves[i].m2 != -1)? M[TabuMoves[i].m2]->ID() : "--"),
							 ((i==TabuPntr-1)? '*' : ' '));
			  fprintf(ssprDbgFile, buf);
			}
	#endif
}

void MOSP_SSPR::ClearTabus()
{
  memset((void *)TabuMoves, -1, sizeof(NSMove)*TabuSize);
  TabuPntr = nTabus = 0;
}

bool MOSP_SSPR::isTabu(NSFunction F, int v, int x, int m)
{
  if(TabuSize == 0) return false;
  bool ret = false;
  for(int i=0; i<nTabus; i++){
	if(TabuMoves[i].F != F) continue;
	if(TabuMoves[i].v != v) continue;
	if(TabuMoves[i].x != x) continue;
	if(F == N1){
	  ret = true;
	  break;
	}
	if(TabuMoves[i].m2 != m) continue;
	ret = true;
	break;
  }
  return ret;
}

void MOSP_SSPR::EnumerateNonTabuMoves(MOSP_SSPR_Solution *Sol, bool bStrictImp)
{
  if(Sol->Cmax == -1) return;    // Network must be interpretted

  // Enumerate all possible non-tabu moves and store the best ones in bestMoves.
  // Meanwhile, if debugging info is needed, output all possible moves
  // info and the best ones.
  //
  int alpha, omega, minX;

#ifdef PRNT_NSCB_DBG
  char buf[256];
  fprintf(ssprDbgFile, "\n\nList of all efficient neighborhood moves:\n\n");
  fprintf(ssprDbgFile, "\t\tOper.\tFunc.\talpha\tomega\tx\tk2\t~Cmax\n");
#endif

  nMoves = 0;
  for(int i=0; i<nOps; i++){
	MOSP_SSPR_Node &nd = Sol->Nodes[i];
	if(!nd.bCritical) continue;

	// Check for the conditions of propositions 2 and 4 before getting N1 parameters
	//
	if(bStrictImp){
	  minX = -1;
	  if(Sol->doesN1Improve(i)) Sol->N1Results(i, alpha, omega, apr_Cmax, minX);
	}
	else Sol->N1Results(i, alpha, omega, apr_Cmax, minX);

#ifdef PRNT_NSCB_DBG
	if(minX > 0){
	  sprintf(buf, "\t\t%s\tN1\t%i\t%i\t%i\t%s\t%i\n",
					O[i]->ID(), alpha, omega, minX, "--", apr_Cmax[minX]);
	  fprintf(ssprDbgFile, buf);
	}
	else{
	  sprintf(buf, "\t\t%s\tN1\t%i\t%i\t%i\t--\t--\n",
					O[i]->ID(), alpha, omega, minX);
	  fprintf(ssprDbgFile, buf);
	}
#endif

	if(minX > 0){
	  // Check if this best move is tabu and if this is the case try to choose
	  // a different insertion postion if possible.
	  // If unable to find a non-tabu move within the given range, let minX = -1
	  // so that no new move will be added.
	  if(isTabu(N1, i, minX) && apr_Cmax[minX] >= 3*bestCmax/4){
		int xr, xl;    // right and left positions of best non-tabu Cmax
		int rCmax, lCmax;  // best right and left makspan values

		minX = -1;
		rCmax = lCmax = MAXINT;
		for(xr = minX + 1; xr <= omega; xr++){
		  if(!isTabu(N1, i, xr)  || apr_Cmax[xr] < 3*bestCmax/4){
			rCmax = apr_Cmax[xr];
			break;
		  }
		}
		for(xl = minX - 1; xl >= alpha; xl--){
		  if(!isTabu(N1, i, xl) || apr_Cmax[xl] < 3*bestCmax/4){
			lCmax = apr_Cmax[xl];
			break;
		  }
		}
		if(rCmax != MAXINT || lCmax != MAXINT){
		  if(rCmax < lCmax) minX = xr;
		  else minX = xl;
		}
	  }
	}
	if(minX > 0){
	  if(nMoves == 0){
		bestMoves[0].F = MOSP_SSPR::N1;
		bestMoves[0].v = i;
		bestMoves[0].x = minX;
		bestMoves[0].aprCmax = apr_Cmax[minX];
		nMoves++;
	  }
	  else{
		bool bInserted = false;
		int m = 0;
		while(m < nMoves){
		  if(bestMoves[m].aprCmax > apr_Cmax[minX]){
			for(int n = nBestMoves-1; n > m; n--){
			  memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(MOSP_SSPR::NSMove));
			}
			bestMoves[m].F = MOSP_SSPR::N1;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].aprCmax = apr_Cmax[minX];

			bInserted = true;
			break;
		  }
		  m++;
		}
		if(!bInserted && nMoves < nBestMoves){
		  m = nMoves++;
		  bestMoves[m].F = MOSP_SSPR::N1;
		  bestMoves[m].v = i;
		  bestMoves[m].x = minX;
		  bestMoves[m].aprCmax = apr_Cmax[minX];
		}
	  }
	}

	SList::SNode *mcnd = O[i]->AltMCs.head();
	while(mcnd){
	  MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	  MOSP_Problem::SMC *mc = opmc->MC;

	  // Check for the conditions of propositions 2 and 4 before getting N1 parameters
	  //
	  if(bStrictImp){
		minX = -1;
		if(Sol->doesN2Improve(i)) Sol->N2Results(i, mc->i, alpha, omega, apr_Cmax, minX);
	  }
      else Sol->N2Results(i, mc->i, alpha, omega, apr_Cmax, minX);

#ifdef PRNT_NSCB_DBG
	  if(minX > 0){
		sprintf(buf, "\t\t%s\tN2\t%i\t%i\t%i\t%s\t%i\n",
					 O[i]->ID(), alpha, omega, minX, mc->ID(), apr_Cmax[minX]);
		fprintf(ssprDbgFile, buf);
	  }
	  else{
		sprintf(buf, "\t\t%s\tN2\t%i\t%i\t%i\t%s\t--\n",
					 O[i]->ID(), alpha, omega, minX, mc->ID());
		fprintf(ssprDbgFile, buf);
	  }
#endif

	  if(minX > 0){
		// Check if this best move is tabu and if this is the case try to choose
		// a different insertion postion if possible.
		// If unable to find a non-tabu move within the given range, let minX = -1
		// so that no new move will be added.
		if(isTabu(N2, i, minX, mc->i) && apr_Cmax[minX] >= 3*bestCmax/4){
		  int xr, xl;    // right and left positions of best non-tabu Cmax
		  int rCmax, lCmax;  // best right and left makspan values

		  minX = -1;
		  rCmax = lCmax = MAXINT;
		  for(xr = minX + 1; xr <= omega; xr++){
			if(!isTabu(N2, i, xr, mc->i)  || apr_Cmax[xr] < 3*bestCmax/4){
			  rCmax = apr_Cmax[xr];
			  break;
			}
		  }
		  for(xl = minX - 1; xl >= alpha; xl--){
			if(!isTabu(N2, i, xl, mc->i) || apr_Cmax[xl] < 3*bestCmax/4){
			  lCmax = apr_Cmax[xl];
			  break;
			}
		  }
		  if(rCmax != MAXINT || lCmax != MAXINT){
			if(rCmax < lCmax) minX = xr;
			else minX = xl;
		  }
		}
	  }
	  if(minX > 0){
		if(nMoves == 0){
		  bestMoves[0].F = MOSP_SSPR::N2;
		  bestMoves[0].v = i;
		  bestMoves[0].x = minX;
		  bestMoves[0].m2 = mc->i;
		  bestMoves[0].aprCmax = apr_Cmax[minX];
		  nMoves++;
		}
		else{
		  bool bInserted = false;
		  int m = 0;
		  while(m < nMoves){
			if(bestMoves[m].aprCmax > apr_Cmax[minX]){

			  for(int n = nBestMoves-1; n > m; n--){
				memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(MOSP_SSPR::NSMove));
			  }
			  bestMoves[m].F = MOSP_SSPR::N2;
			  bestMoves[m].v = i;
			  bestMoves[m].x = minX;
			  bestMoves[m].m2 = mc->i;
			  bestMoves[m].aprCmax = apr_Cmax[minX];

			  bInserted = true;
			  break;
			}
			m++;
		  }
		  if(!bInserted && nMoves < nBestMoves){
			m = nMoves++;
			bestMoves[m].F = MOSP_SSPR::N2;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].m2 = mc->i;
			bestMoves[m].aprCmax = apr_Cmax[minX];
		  }
		}
	  }
	  mcnd = mcnd->Next();
	}
  }

#ifdef PRNT_NSCB_DBG
  fprintf(ssprDbgFile, "\n\nList of the best efficient neighborhood moves:\n\n");
  fprintf(ssprDbgFile, "\t\tOper.\tFunc.\tx\tk2\t~Cmax\n");

  for(int m=0; m<nMoves; m++){
	NSMove &mv = bestMoves[m];
	if(mv.F == N1)
	  sprintf(buf, "\t\t%s\tN1\t%i\t%s\t%i\n", O[mv.v]->ID(), mv.x, "--", mv.aprCmax);
	else
	  sprintf(buf, "\t\t%s\tN2\t%i\t%s\t%i\n", O[mv.v]->ID(), mv.x, M[mv.m2]->ID(), mv.aprCmax);
	fprintf(ssprDbgFile, buf);
  }
  fflush(ssprDbgFile);
#endif
}

int MOSP_SSPR::ApplyNandUpdateTabuList(MOSP_SSPR_Solution *Sol, NSMove mv)
{
  // This function modivies the solution vectors by conducting the received move
  // mv, then the solution vectors are interpretted to network representation
  // and all start and finish times are calculated.
  // Note: It is assumed that the sent move is feasible.

  NSMove Inverse;  // inverse move to the one being applied
  Inverse.F = mv.F;
  Inverse.v = mv.v;

  if(mv.F == N1){
	int j = Sol->jb[mv.v];
    int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->Cmax;  // There is nothing to do!

    Inverse.m2 = -1;
	Inverse.x = oldx;

	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++){
		tau[x] = tau[x+1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--){
		tau[x] = tau[x-1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
  }
  else{
	int m1 = Sol->mc[mv.v];

	Inverse.m2 = m1;

	if(mv.m2 == m1){
	  int *&pi = Sol->pi[mv.m2];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->Cmax; // There is nothing to do

	  Inverse.x = oldx;

	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++){
		  pi[x] = pi[x+1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  else{
        int x;
		for(x=oldx; x>mv.x; x--){
		  pi[x] = pi[x-1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];

      Inverse.x = curx;

	  for(int x=curx; x<pi1[0]; x++){
		pi1[x] = pi1[x+1];
		Sol->pi_pos[pi1[x]] = x;
	  }
	  pi1[0]--;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0]){
		pi2[++pi2[0]] = mv.v;
		Sol->pi_pos[mv.v] = mv.x;
	  }
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--){
		  pi2[x] = pi2[x-1];
		  Sol->pi_pos[pi2[x]] = x;
		}
		pi2[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  Sol->mc[mv.v] = mv.m2;
	}
  }

	  #ifdef PRNT_VNT_DBG
		 Sol->AssertVectors("Vector representation after conducting selected move");
      #endif

  Sol->GenerateNetwork();
  if(Sol->InterpretNetwork()){
	if(isTabu(Inverse.F, Inverse.v, Inverse.x, Inverse.m2))
	  // If the inverse move is currently tabu, add the applied move to the tabu list instead
	  AddTabu(mv.F, mv.v, mv.x, mv.m2);
	else
	  // Now add the inverse of the applied move to the tabu list
	  AddTabu(Inverse.F, Inverse.v, Inverse.x, Inverse.m2);

	return Sol->Cmax;
  }
  return -1;
}

//------------------------------------------------- Path Relinking functions

int MOSP_SSPR::DetailedDistances(MOSP_SSPR_Solution *S1, MOSP_SSPR_Solution *S2)
{
  TotaljbDist = TotalwcDist = 0;

  for(int j=0; j<nJobs; j++){
	jbDist[j] = TauDistance(j, S1, S2);
	if(jbDist[j] > 0) bUseTau[j] = true;
	else bUseTau[j] = false;
	TotaljbDist += jbDist[j];
  }
  for(int w=0; w<nWCs; w++){
	wcDist[w] = WCDistance(w, S1, S2);
	if(wcDist[w] > 0) bUseVPi[w] = true;
	else bUseVPi[w] = false;
	TotalwcDist += wcDist[w];
  }

  return (TotaljbDist + TotalwcDist);
}

MOSP_SSPR_Solution *MOSP_SSPR::PathRelinking(MOSP_SSPR_Solution *Si,
											 MOSP_SSPR_Solution *Sg)
{
#ifdef PRNT_PR_DBG
		   char buf[256];
		   Si->AssertVectors("Initial Solution for path relinking");
		   sprintf(buf, "\n\t\t--> Cmax = %i\n", Si->Cmax);
		   fprintf(ssprDbgFile, buf);
		   Sg->AssertVectors("Guiding Solution for path relinking");
		   sprintf(buf, "\n\t\t--> Cmax = %i\n", Sg->Cmax);
		   fprintf(ssprDbgFile, buf);
#endif

  MOSP_SSPR_Solution *S3 = new MOSP_SSPR_Solution(this);
  S3->Copy(Si);

  // Apply the steps of path relinking

  // 1. Calculate the current distance between Si and Sg
  int w, j;
  int D = DetailedDistances(Si, Sg);
  int initialDist = D;

#ifdef PRNT_PR_DBG
		   sprintf(buf, "\n\t\t------------ Initial Distance = %i --------------\n", initialDist);
		   fprintf(ssprDbgFile, buf);

		   fprintf(ssprDbgFile, "\nDetailed distances:\n\n");
		   fprintf(ssprDbgFile, "\ta) Workstations:\n");
		   for(w=0; w<nWCs; w++){
			 sprintf(buf, "\t\t%s: %i\n", W[w]->ID(), wcDist[w]);
			 fprintf(ssprDbgFile, buf);
		   }
		   fprintf(ssprDbgFile, "\n\tb) Jobs:\n");
		   for(j=0; j<nJobs; j++){
			 sprintf(buf, "\t\t%s: %i\n", J[j]->ID(), jbDist[j]);
			 fprintf(ssprDbgFile, buf);
		   }
		   fprintf(ssprDbgFile, "\n\n");
		   fflush(ssprDbgFile);
#endif

  int nApplications = 0;
  do{
	// Select either a job permutation or a workstation vector of permutations
	// randomly to conduct the solution combination function
	//
	bool bSelB1 = flip(0.5);
	bool bAppliedB1, bAppliedB2;

	if(bSelB1){ // Use B1
	  // Find a feasible move of subpermutation that can result in the largest
	  // reduction in the distance for the selected job
	  for(int B1itr=0; B1itr < TotaljbDist / nJobs + 1; B1itr++){  // A number of iterations
													   // are needed to find a feasible move
		Application->ProcessMessages();
		if(bInterrupt){
		  bTerminate = true;
		  break;
	    }

		// First select Tau associated with max. distance (random tie-breaking)
		int sj = -1;
		int maxTauDist = 0;
		bAppliedB1 = false;

		for(j=0; j<nJobs; j++){
		  if(!bUseTau[j]) continue;
		  if(jbDist[j] > maxTauDist){
			maxTauDist = jbDist[j];
			sj = j;
		  }
		  else if(jbDist[j] == maxTauDist && flip(0.5)){
			maxTauDist = jbDist[j];
			sj = j;
		  }
		}

			#ifdef PRNT_PR_DBG
			   sprintf(buf, "\nSelected job index = %i\n", sj);
			   fprintf(ssprDbgFile, buf);
			   fflush(ssprDbgFile);
			#endif

		// Now based on the intersection set find the best feasible move
		if(sj != -1){
		  int x1, x2, x;

		  BestTauSubpermutationMove(S3, Sg, sj, x1, x2, x);

		  if(x != -1){
			// Apply the selected subpermutation move and update the solution S
				#ifdef PRNT_PR_DBG
					sprintf(buf, "\nSelected Tau subpermutation:\n\t Job %s, x1=%i, x2=%i, x=%i\n",
								J[sj]->ID(), x1, x2, x);
					fprintf(ssprDbgFile, buf);
					fflush(ssprDbgFile);
				#endif

			ApplyB1(S3, sj, x1, x2, x);
			nApplications++;

			// Update the distances information (do not recalculate the whole distance,
			// just the distance change associated with the applied subpermutation move)
			D -= jbDist[sj];
			jbDist[sj] = TauDistance(sj, S3, Sg);
			D += jbDist[sj];

			if(jbDist[sj] == 0) bUseTau[sj] = false;

				 #ifdef PRNT_PR_DBG
					  sprintf(buf, "\n\t\t New distance for job %s is %i\n", J[sj]->ID(), jbDist[sj]);
					  fprintf(ssprDbgFile, buf);
					  sprintf(buf, "\n\t\t------------ New Distance = %i --------------\n", D);
					  fprintf(ssprDbgFile, buf);
					  fflush(ssprDbgFile);
				 #endif

			bAppliedB1 = true;
			break;
		  }
		  else{
				 #ifdef PRNT_PR_DBG
					  sprintf(buf, "\n---> For Tau of job %s, no best feasible subpermutation move is found \n\n", J[sj]->ID());
					  fprintf(ssprDbgFile, buf);
					  fflush(ssprDbgFile);
				 #endif

			bUseTau[sj] = false;     // To avoid selecting the same job again
		  }
		}
		else{
		  bAppliedB1 = false;
		  break;
		}
	  }
	}
	if(!bSelB1 || !bAppliedB1){ // Use B2
	  // Find a feasible move of subpermutation that can result in the largest
	  // reduction in the distance for the selected workstation
	  for(int B2itr=0; B2itr < TotalwcDist / nWCs + 1; B2itr++){  // A number of iterations
													   // are needed to find a feasible move
		Application->ProcessMessages();
		if(bInterrupt){
		  bTerminate = true;
		  break;
		}

		// First select a workstation (Vector of Pi's) with max. distance (random tie-breaking)
		int sw = -1;
		int maxPiDist = 0;
		bAppliedB2 = false;

		for(w=0; w<nWCs; w++){
		  if(!bUseVPi[w]) continue;
		  if(wcDist[w] > maxPiDist){
			maxPiDist = wcDist[w];
			sw = w;
		  }
		  else if(wcDist[w] == maxPiDist && flip(0.5)){
			maxPiDist = wcDist[w];
			sw = w;
		  }
		}

				 #ifdef PRNT_PR_DBG
					sprintf(buf, "\nSelected workstation index = %i\n", sw);
					fprintf(ssprDbgFile, buf);
					fflush(ssprDbgFile);
				 #endif

		if(sw != -1){
		  int x1, x2, x, fromMC, toMC;

		  BestVPiSubpermutationMove(S3, Sg, sw, x1, x2, x, fromMC, toMC);

		  if(x != -1){
			// Apply the selected subpermutation move and update the solution S
				#ifdef PRNT_PR_DBG
					sprintf(buf, "\nSelected Pi subpermutation:\n\t Workstation %s, x1=%i, x2=%i, x=%i, from %s, to %s\n",
								W[sw]->ID(), x1, x2, x, M[fromMC]->ID(), M[toMC]->ID());
					fprintf(ssprDbgFile, buf);
					fflush(ssprDbgFile);
				#endif

			ApplyB2(S3, x1, x2, x, fromMC, toMC);
			nApplications++;

			// Update the distances information (do not recalculate the whole distance,
			// just the distance change associated with the applied subpermutation move)
			D -= wcDist[sw];
			wcDist[sw] = WCDistance(sw, S3, Sg);
			D += wcDist[sw];

			if(wcDist[sw] == 0) bUseVPi[sw] = false;

				 #ifdef PRNT_PR_DBG
					 sprintf(buf, "\n\t\t------------ New Distance = %i --------------\n", D);
					 fprintf(ssprDbgFile, buf);
					 fflush(ssprDbgFile);
				 #endif

			bAppliedB2 = true;
		  }
		  else{
				 #ifdef PRNT_PR_DBG
					  sprintf(buf, "\n---> For Pis of workstation %s, no best feasible subpermutation move is found \n\n", W[sw]->ID());
					  fprintf(ssprDbgFile, buf);
					  fflush(ssprDbgFile);
				 #endif

			bUseVPi[sw] = false;     // To avoid selecting the same workstation again
		  }
		}
		else{
		  bAppliedB2 = false;
		  break;
		}
	  }
	}

	if(!bAppliedB1 && !bAppliedB2) break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}

	// If the distance reached is at least one quarter of the initial distance,
	// stop. Otherwise, repeat the previous iterations
  }while(initialDist - D < Epsilon && nApplications < 2*Epsilon);

  return S3;
}

void MOSP_SSPR::BestTauSubpermutationMove(MOSP_SSPR_Solution *S2, MOSP_SSPR_Solution *S1,
								   int jb, int &x1, int &x2, int &x)
{
  // Notice that S1 is the initial solution and S2 is the target solution
  // therefore, S1 is supposed to be modified to be the same as S2, that's why
  // they are exchanged in the function argument list.

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(MOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  int *&v1 = S1->tau[jb];
  int *&v2 = S2->tau[jb];
  int sz = v1[0];

  nr = 0;
  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];
	int x2 = S2->tau_pos[i];
	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

		 #ifdef PRNT_PR_DBG
			 char buf[256];
			 sprintf(buf, "\n\nThe intersection set between permutations of job %s:\n", J[jb]->ID());
			 fprintf(ssprDbgFile, buf);
			 if(nr==0){
			   sprintf(buf, "\tThe two permutations are found identical\n\n");
			   fprintf(ssprDbgFile, buf);
			 }
			 else{
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			   fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
		 #endif

  if(nr == 0){  // The distance is already zero!!!!
	x1 = x2 = x = -1;
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Cardinality of the intersection set is 1 -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	return;
  }

  // Construct the initial subpermutation relative positions matrix
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\nThe subpermutations relative positions matrix:\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\tSum");
			 for(int j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  // Now iterate on the "a" matrix to deterine the next best subpermutation move
  for(int subitr=0; subitr<3; subitr++){
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;
	int toX;

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] == 0) continue;
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	  else if(Suma_i[i] == max_Sa && flip(0.5)){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}
	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] == 0) continue;
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	  else if(Suma_j[j] == max_Sa && flip(0.5)){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_ij == -1){  // No move is found. Something went wrong !!!!
	  x1 = x2 = x = -1;
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- No move found !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	  break;
	}

	// Determine the parameters of the B1 move
	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  toX = min_Xj;
	}
	else{  // Forward move
	  int max_Xi = -1;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
		  }
		}
	  }
	  toX = max_Xi;
	}

	x1 = r[max_ij].x2;
	x2 = r[max_ij].x2 + r[max_ij].l - 1;
	x = toX;

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\tBest found move is: ");
			 fprintf(ssprDbgFile, buf);
			 if(bInRow) fprintf(ssprDbgFile, "Backward ");
			 else fprintf(ssprDbgFile, "Forward ");
			 sprintf(buf, "move of subpermutation #%i (from %i to %i) to position %i\n",
						  max_ij+1, x1, x2, toX);
			 fprintf(ssprDbgFile, buf);
		 #endif

	int alpha, omega;
	S2->alpha_omega_B1(jb, x1, x2, alpha, omega);   // This has to be S2 since we target
													// changing S2 which is the first
													// argument to this function

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\tSubpermutation insertion positions: alpha = %i, omega = %i \n", alpha, omega);
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif


	if(x < alpha || x > omega){  // Insertion position can result in infeasible solution
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Probably an infeasible move !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	  if(bInRow) Suma_i[max_ij] = 0;
	  else Suma_j[max_ij] = 0;
	  x1 = x2 = x = -1;
	}
	else break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
}

void MOSP_SSPR::ApplyB1(MOSP_SSPR_Solution *Sol, int jb, int x1, int x2, int tox)
{
  // Move the subpermutation in x1,...,x2 to position x and construct the new
  // network with complete schedule information

  if(x1 == tox) return;   // No change will be made in this case!

  int *&tau = Sol->tau[jb];
  int l = tau[0];
  int x, dx;
  if(x1 < tox){
	for(x=1; x < x1; x++) tmpTau[x] = tau[x];
	dx = x2-x1+1;
	while(x < tox){  // Notice that x = x1 at the beginning of this loop
	  tmpTau[x] = tau[x + dx];
	  x++;
	}  // At the end of this loop, x = tox
	for(dx=0; dx <= x2-x1; dx++) tmpTau[x+dx] = tau[x1+dx];
	x += dx;
	while(x <= l){
	  tmpTau[x] = tau[x];
	  x++;
	}
  }
  else{
	for(x=1; x < tox; x++)  tmpTau[x] = tau[x];
	for(dx=0; dx <= x2-x1; dx++) tmpTau[x+dx] = tau[x1+dx];
	x += dx;
	while(x - dx < x1){
	  tmpTau[x] = tau[x - dx];
	  x++;
	}
	while(x <= l){
	  tmpTau[x] = tau[x];
	  x++;
	}
  }

  for(x = 1; x <= l; x++){
	int v = tmpTau[x];
	tau[x] = v;
	Sol->tau_pos[v] = x;
  }

		 #ifdef PRNT_PR_DBG
			 Sol->AssertVectors("Solution vectors after applying B1");
			 fflush(ssprDbgFile);
		 #endif

  Sol->GenerateNetwork();
  Sol->InterpretNetwork();

		 #ifdef PRNT_PR_DBG
			 Sol->AssertNetwork("Corresponding network after applying B1");
			 fflush(ssprDbgFile);
		 #endif
}

void MOSP_SSPR::BestVPiSubpermutationMove(MOSP_SSPR_Solution *S2, MOSP_SSPR_Solution *S1,
								   int sw, int &x1, int &x2, int &x, int &fromMC, int &toMC)
{
  // Notice that S1 is the initial solution and S2 is the target solution
  // therefore, S1 is supposed to be modified to be the same as S2, that's why
  // they are exchanged in the function argument list.
  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(MOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  nr = 0;  // starting number of subpermutations is 0

		 #ifdef PRNT_PR_DBG
			 char buf[256];
		 #endif

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  // where rho_1 = pi^1_w,1 -> <d1> -> pi^1_w,2 -> <d2> ..... -> pi^1_w,q_w
  // and  rho_2 = pi^2_w,1 -> <d1> -> pi^2_w,2 -> <d2> ..... -> pi^2_w,q_w
  int nmcs = Wz[sw] - Wa[sw] + 1;    // number of machines in workcetner w
  int nops1 = 0, nops2 = 0;
  for(int m=Wa[sw]; m<=Wz[sw]; m++){
	nops1 += S1->pi[m][0];
	nops2 += S2->pi[m][0];
  }
  if(nops1 != nops2){  //  This should not happen anyway
	x1 = x2 = x = -1;

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\n-------> Unequal number of operations in workstation %s!!!! <-------\n", W[sw]->ID());
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif
	return;
  }

  int sz = nops1 + nmcs - 1;
  int *v1 = new int[sz+1];
  int *v2 = new int[sz+1];
  v1[0] = v2[0] = sz;

  x1 = x2 = 1;
  for(int m=Wa[sw]; m<=Wz[sw]; m++){
	int pisz1 = S1->pi[m][0];
	memcpy((void *)&v1[x1], &S1->pi[m][1], sizeof(int) * pisz1);
	if(m != Wz[sw]){
	  x1 += pisz1;
	  v1[x1] = nOps + m;
	  x1++;
	}

	int pisz2 = S2->pi[m][0];
	memcpy((void *)&v2[x2], &S2->pi[m][1], sizeof(int) * pisz2);
	if(m != Wz[sw]){
	  x2 += pisz2;
	  v2[x2] = nOps + m;
	  x2++;
	}
  }

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\nThe collective permutations of workstation %s are:\n", W[sw]->ID());
			 fprintf(ssprDbgFile, buf);
			 sprintf(buf, "\n\tFor the first solution\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=1; i<=v1[0]; i++){
			   if(v1[i] < nOps) sprintf(buf, "%s ", O[v1[i]]->ID());
			   else sprintf(buf, "d%i ", v1[i] - nOps);
			   fprintf(ssprDbgFile, buf);
			   if(i != v1[0]) fprintf(ssprDbgFile, " -> ");
			   else fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
			 sprintf(buf, "\n\tFor the second solution\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=1; i<=v2[0]; i++){
			   if(v2[i] < nOps) sprintf(buf, "%s ", O[v2[i]]->ID());
			   else sprintf(buf, "d%i ", v2[i] - nOps);
			   fprintf(ssprDbgFile, buf);
			   if(i != v2[0]) fprintf(ssprDbgFile, " -> ");
			   else fprintf(ssprDbgFile, "\n");
			 }
			 fprintf(ssprDbgFile, "\n\n");
			 fflush(ssprDbgFile);
		 #endif

  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];

	x2 = 0;
	if(i < nOps){
	  for(int m2=Wa[sw]; m2<=Wz[sw]; m2++){
		if(m2 != S2->mc[i]) x2 += (S2->pi[m2][0] + 1);
		else{
		  x2 += S2->pi_pos[i];
		  break;
		}
	  }
	}
	else{
	  for(int m2=Wa[sw]; m2<=Wz[sw]; m2++){
		x2 += (S2->pi[m2][0] + 1);
		if(m2 == i-nOps) break;
	  }
	}

	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "The intersection set between permutations of workstations %s:\n", W[sw]->ID());
			 fprintf(ssprDbgFile, buf);
			 if(nr==0){
			   sprintf(buf, "\tThe two permutations are found identical\n\n");
			   fprintf(ssprDbgFile, buf);
			 }
			 else{
			   for(int i=0; i<=nr; i++){
				 sprintf(buf, "\tsubpermutation #%i: x1 = %i, x2 = %i, l = %i\n",
							  i+1, r[i].x1, r[i].x2, r[i].l);
				 fprintf(ssprDbgFile, buf);
			   }
			   fprintf(ssprDbgFile, "\n");
			 }
			 fflush(ssprDbgFile);
		 #endif

  if(nr == 0){   // the two permutations are identical
	x1 = x2 = x = -1;

		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Cardinality of the intersection set is 1 -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif

	delete [] v1;
	delete [] v2;
	return;
  }

  // Construct the subpermutations relative positions matrix [a]
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\nThe subpermutations relative positions matrix:\n\t\t");
			 fprintf(ssprDbgFile, buf);
			 for(int i=0; i<=nr; i++){
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "Sum");
			 for(int i=0; i<=nr; i++){
			   fprintf(ssprDbgFile, "\n\t");
			   sprintf(buf, "%i\t", i+1);
			   fprintf(ssprDbgFile, buf);
			   int j;
			   for(j=0; j<=i; j++) fprintf(ssprDbgFile, "-\t");
			   for(; j<=nr; j++){
				 sprintf(buf, "%i\t", a[i][j]);
				 fprintf(ssprDbgFile, buf);
			   }
			   sprintf(buf, "%i", Suma_i[i]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n\tSum");
			 for(int j=0; j<=nr; j++){
			   sprintf(buf, "\t%i", Suma_j[j]);
			   fprintf(ssprDbgFile, buf);
			 }
			 fprintf(ssprDbgFile, "\n");
			 fflush(ssprDbgFile);
		 #endif

  // Now iterate on the "a" matrix to deterine the next best subpermutation move
  for(int subitr=0; subitr<3; subitr++){
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;
	int toX;

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] == 0) continue;
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	  else if(Suma_i[i] == max_Sa && flip(0.5)){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}
	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] == 0) continue;
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	  else if(Suma_j[j] == max_Sa && flip(0.5)){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_ij == -1){  // No move is found. Something went wrong !!!!
	  x1 = x2 = x = -1;

		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- No move found !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif

	  delete [] v1;
	  delete [] v2;
	  return;
	}

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\nSelected subpermutation #%i (from position %i to position %i) \n\n", max_ij+1, r[max_ij].x2, r[max_ij].x2+r[max_ij].l-1);
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif

	// If the subpermutation contains a dummy operation, we will exclude that
	// from the best found moves
	bool bDummyIn = false;
	for(int x_ = r[max_ij].x2; x_ < r[max_ij].x2+r[max_ij].l; x_++){
	  if(v2[x_] >= nOps){
		bDummyIn = true;
		break;
	  }
	}
	if(bDummyIn){
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Dummy operation in subpermutation !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	  if(bInRow) Suma_i[max_ij] = 0;
	  else Suma_j[max_ij] = 0;
	  x1 = x2 = x = -1;
	  continue;
	}

	// Determine the move-to position
	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  toX = min_Xj;
	}
	else{  // Forward move
	  int max_Xi = -1;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
		  }
		}
	  }
	  toX = max_Xi;
	}

	// Interpret the move paramters defined in terms of the collective permutations
	// to parameters that can be used by B2 function
	// Note: since we know that there is no dummy operations in the subpermutation
	// range, the start and end positions belong to the same machine.
	x1 = r[max_ij].x2;
	x2 = r[max_ij].x2 + r[max_ij].l - 1;

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\tBest found move is: ");
			 fprintf(ssprDbgFile, buf);
			 if(bInRow) fprintf(ssprDbgFile, "Backward ");
			 else fprintf(ssprDbgFile, "Forward ");
			 sprintf(buf, "move of subpermutation #%i (from %i to %i) to position %i\n",
						  max_ij+1, x1, x2, toX);
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif

	// find the starting position for the permutation of the corresponding machine
	int cumX = 0, lastCumX = 0;
	for(fromMC = Wa[sw]; fromMC <= Wz[sw]; fromMC++){
	  cumX += S2->pi[fromMC][0];
	  if(x1 <= cumX) break;
	  lastCumX = ++cumX;
	}
	x1 -= lastCumX;
	x2 -= lastCumX;

	x = toX;
	cumX = lastCumX = 0;
	for(toMC = Wa[sw]; toMC <= Wz[sw]; toMC++){
	  cumX += S2->pi[toMC][0];
	  if(x <= cumX) break;
	  lastCumX = ++cumX;
	}
	x -= lastCumX;
	if(!bInRow && fromMC != toMC) x++;  // In case of forward move to another machine

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\t--> This means moving the subpermutation (%i to %i) from machine %s",
						  x1, x2, M[fromMC]->ID());
			 fprintf(ssprDbgFile, buf);
			 sprintf(buf, "\n\t\tTo machine %s in position %i\n", M[toMC]->ID(), x);
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif

	/*if(x1 == 0 || x2 == 0 || x == 0){   // Another check for the dummy operation
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Dummy operation in subpermutation !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	  if(bInRow) Suma_i[max_ij] = 0;
	  else Suma_j[max_ij] = 0;
	  x1 = x2 = x = -1;
	  continue;
	}*/

	int alpha, omega;
	S2->alpha_omega_B2(fromMC, toMC, x1, x2, alpha, omega);   // This has to be S2 since we target
															  // changing S2 which is the first
													          // argument to this function

		 #ifdef PRNT_PR_DBG
			 sprintf(buf, "\n\tSubpermutation insertion positions: alpha = %i, omega = %i \n", alpha, omega);
			 fprintf(ssprDbgFile, buf);
			 fflush(ssprDbgFile);
		 #endif


	if(x < alpha || x > omega){  // Insertion position can result in infeasible solution
		 #ifdef PRNT_PR_DBG
			 fprintf(ssprDbgFile, "\n----------------- Probably an infeasible move !!!  -------------\n\n");
			 fflush(ssprDbgFile);
		 #endif
	  if(bInRow) Suma_i[max_ij] = 0;
	  else Suma_j[max_ij] = 0;
	  x1 = x2 = x = -1;
	}
	else break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  delete [] v1;
  delete [] v2;
}

void MOSP_SSPR::ApplyB2(MOSP_SSPR_Solution *Sol, int x1, int x2, int tox, int fromMC, int toMC)
{
  // Move the subpermutation in x1,...,x2 in fromMC to position x in toMC
  // and construct the new network with complete schedule information

  if(fromMC == toMC){
	if(x1 == tox) return;   // No change will be made in this case!

	int *&pi = Sol->pi[fromMC];
	int l = pi[0];
	int x, dx;
	if(x1 < tox){
	  for(x=1; x < x1; x++) tmpPi[x] = pi[x];
	  dx = x2-x1+1;
	  while(x < tox){  // Notice that x = x1 at the beginning of this loop
		tmpPi[x] = pi[x + dx];
		x++;
	  }  // At the end of this loop, x = tox
	  for(dx=0; dx <= x2-x1; dx++) tmpPi[x+dx] = pi[x1+dx];
	  x += dx;
	  while(x <= l){
	    tmpPi[x] = pi[x];
		x++;
	  }
    }
	else{
	  for(x=1; x < tox; x++)  tmpPi[x] = pi[x];
	  for(dx=0; dx <= x2-x1; dx++) tmpPi[x+dx] = pi[x1+dx];
	  x += dx;
	  while(x - dx < x1){
		tmpPi[x] = pi[x - dx];
		x++;
	  }
	  while(x <= l){
		tmpPi[x] = pi[x];
		x++;
	  }
	}

	for(x = 1; x <= l; x++){
	  int v = tmpPi[x];
	  pi[x] = v;
	  Sol->pi_pos[v] = x;
    }
  }
  else{
	// Change the vector of the toMC machine
	int *&pi1 = Sol->pi[fromMC];
	int *&pi2 = Sol->pi[toMC];
	int l = pi2[0], x;
	if(l != 0){
	  for(x=1; x<tox; x++){
		tmpPi[x] = pi2[x];
	  }
	  int dx = 0;
	  for(; x <= tox + x2 - x1; x++){
		tmpPi[x] = pi1[x1+dx];
		dx++;
	  }
	  for(; x <= l + dx; x++){
		tmpPi[x] = pi2[x-dx];
	  }
	  for(x = 1; x <= l + dx; x++){
		int v = tmpPi[x];
		pi2[x] = v;
		Sol->pi_pos[v] = x;
		Sol->mc[v] = toMC;
	  }
	  pi2[0] = l + dx;
	}
	else{
	  int dx = 0;
	  for(x = 1; x <= x2 - x1 + 1; x++){
		int v = pi1[x1+dx];
		pi2[x] = v;
		Sol->pi_pos[v] = x;
		Sol->mc[v] = toMC;
		dx++;
	  }
      pi2[0] = dx;
	}

	// Change the vector of the fromMC machine
	l = pi1[0];
	for(x=x2+1; x<=l; x++){
	  int v = pi1[x];
	  int xv = x-x2+x1-1;
	  pi1[xv] = v;
	  Sol->pi_pos[v] = xv;
	}
	pi1[0] = l - (x2 - x1 + 1);
  }

		 #ifdef PRNT_PR_DBG
			 Sol->AssertVectors("Solution vectors after applying B2");
			 fflush(ssprDbgFile);
		 #endif

  Sol->GenerateNetwork();
  Sol->InterpretNetwork();

		 #ifdef PRNT_PR_DBG
			 Sol->AssertNetwork("Corresponding network after applying B2");
			 fflush(ssprDbgFile);
		 #endif
}

#ifdef DEBUG_SSPR_RESULTS

bool MOSP_SSPR::SetSeedSolution(MOSP_Solution *Sol)
{
  AllocateMemory();
  // Copy tasks from the given initial solution to the sspr->Solution tasks
  for(int i=0; i<nOps; i++){
	MOSP_Solution::SchTask *gtsk = Sol->TaskAssociatedToOperation(O[i]);
	memcpy(tsk[i], gtsk, sizeof(MOSP_Solution::SchTask));
  }

  if(seedSol) delete seedSol;
  seedSol = new MOSP_SSPR_Solution(this);
  seedSol->InterpretTaskList();
  seedSol->GenerateNetwork();
  if(seedSol->InterpretNetwork()){
	bestCmax = seedSol->Cmax;
	if(bestSol) delete bestSol;
	bestSol = new MOSP_SSPR_Solution(this);
	bestSol->Copy(seedSol);
	return seedSol->ConvertNetToTasks();
  }
  return false;
}

int MOSP_SSPR::nOpsByMCinSeedSol(int mc)
{
  if(!seedSol) return -1;
  return seedSol->pi[mc][0];
}

void MOSP_SSPR::EnumerateMoves(bool bStrictImp)
{
  EnumerateMoves(seedSol, bStrictImp);
}

int MOSP_SSPR::ApplyN(int mv)
{
  if(ApplyN(seedSol, bestMoves[mv]) != -1){
	if(seedSol->ConvertNetToTasks()){
	  SList &Tasks = Solution->ScheduledTasks;
	  // Sort tasks in increasing order of their earliest start times
	  Tasks.QSort(tskComp, ASCENDING);
	  return seedSol->Cmax;
	}
  }
  return -1;
}

int MOSP_SSPR::ApplyNandUpdateTabuList(int mv)
{
  if(ApplyNandUpdateTabuList(seedSol, bestMoves[mv]) != -1){
	if(seedSol->ConvertNetToTasks()){
	  SList &Tasks = Solution->ScheduledTasks;
	  // Sort tasks in increasing order of their earliest start times
	  Tasks.QSort(tskComp, ASCENDING);
	  return seedSol->Cmax;
	}
  }
  return -1;
}

#ifdef PRNT_NSCB_DBG
void MOSP_SSPR::GenerateNSParameters(MOSP_Problem::SOperation *v)
{
  char buf[256];

  sprintf(buf, "\n\nIndividual calculations for N1 parameters for operation %s:\n\n", v->ID());
  fprintf(ssprDbgFile, buf);

  int alpha, omega, phi, x, check_delta, hat_delta, delta, aprx_Cmax;
  seedSol->alpha_omega_N1(v->i, alpha, omega);

  sprintf(buf, "\t\talpha = %i\n\t\tomega = %i\n", alpha, omega);
  fprintf(ssprDbgFile, buf);

  phi = seedSol->phi_N1(v->i);

  sprintf(buf, "\t\tphi = %i\n", phi);
  fprintf(ssprDbgFile, buf);
  sprintf(buf, "\n\tEstimates at different insertion positions for N1:\n");
  fprintf(ssprDbgFile, buf);
  sprintf(buf, "\t\t\tx\t/\\delta\t\\/delta\tdelta\t~Cmax\n");
  fprintf(ssprDbgFile, buf);

  for(int x=alpha; x<=omega; x++){
	seedSol->Deltas_N1(v->i, x, hat_delta, check_delta);
	delta = ((hat_delta > 0)? hat_delta : 0) + ((check_delta > 0 )? check_delta : 0);
	aprx_Cmax = delta + phi;
	sprintf(buf, "\t\t\t%i\t%i\t%i\t%i\t%i\n", x, hat_delta, check_delta, delta, aprx_Cmax);
	fprintf(ssprDbgFile, buf);
  }

  sprintf(buf, "\n\nIndividual calculations for N2 parameters for operation %s:\n\n", v->ID());
  fprintf(ssprDbgFile, buf);

  SList::SNode *mcnd = v->AltMCs.head();
  while(mcnd){
	MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	MOSP_Problem::SMC *mc = opmc->MC;
	sprintf(buf, "\tFor machine %s:\n", mc->ID());
	fprintf(ssprDbgFile, buf);
	seedSol->alpha_omega_N2(v->i, mc->i, alpha, omega);
	sprintf(buf, "\t\talpha = %i\n\t\tomega = %i\n", alpha, omega);
	fprintf(ssprDbgFile, buf);
	phi = seedSol->phi_N2(v->i, mc->i);
	sprintf(buf, "\t\tphi = %i\n", phi);
	fprintf(ssprDbgFile, buf);

	sprintf(buf, "\n\t\tEstimates at different insertion positions:\n");
	fprintf(ssprDbgFile, buf);
	sprintf(buf, "\t\t\t\tx\t/\\delta\t\\/delta\tdelta\t~Cmax\n");
	fprintf(ssprDbgFile, buf);

	for(int x=alpha; x<=omega; x++){
	  seedSol->Deltas_N2(v->i, mc->i, x, hat_delta, check_delta);
	  delta = ((hat_delta > 0)? hat_delta : 0) + ((check_delta > 0 )? check_delta : 0);
	  aprx_Cmax = delta + phi;

	  sprintf(buf, "\t\t\t\t%i\t%i\t%i\t%i\t%i\n", x, hat_delta, check_delta, delta, aprx_Cmax);
	  fprintf(ssprDbgFile, buf);
	}
	fprintf(ssprDbgFile, "\n");

	mcnd = mcnd->Next();
  }

  int minX;
  int *N1Cmax = new int [nWCs+1];
  int *N2Cmax = new int [nJobs+1];

  seedSol->N1Results(v->i, alpha, omega, N1Cmax, minX);

  sprintf(buf, "\n\nIntegrated calculations for N1 parameters for operation %s:\n\n", v->ID());
  fprintf(ssprDbgFile, buf);
  sprintf(buf, "\t\talpha = %i\n\t\tomega = %i\n", alpha, omega);
  fprintf(ssprDbgFile, buf);
  sprintf(buf, "\t\t\t\tx\t~Cmax\n");
  fprintf(ssprDbgFile, buf);
  for(int x=alpha; x<=omega; x++){
	sprintf(buf, "\t\t\t\t%i\t%i\n", x, N1Cmax[x]);
	fprintf(ssprDbgFile, buf);
  }
  sprintf(buf, "\n\t\t--> min Cmax at x = %i", minX);
  fprintf(ssprDbgFile, buf);

  sprintf(buf, "\n\nIntegrated calculations for N2 parameters for operation %s:", v->ID());
  fprintf(ssprDbgFile, buf);

  mcnd = v->AltMCs.head();
  while(mcnd){
	MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	MOSP_Problem::SMC *mc = opmc->MC;
	sprintf(buf, "\n\n\tFor machine %s:\n", mc->ID());
	fprintf(ssprDbgFile, buf);
	seedSol->N2Results(v->i, mc->i, alpha, omega, N2Cmax, minX);

	sprintf(buf, "\t\talpha = %i\n\t\tomega = %i\n", alpha, omega);
	fprintf(ssprDbgFile, buf);

	sprintf(buf, "\t\t\t\tx\t~Cmax\n");
	fprintf(ssprDbgFile, buf);
	for(int x=alpha; x<=omega; x++){
	  sprintf(buf, "\t\t\t\t%i\t%i\n", x, N2Cmax[x]);
	  fprintf(ssprDbgFile, buf);
	}
	sprintf(buf, "\n\t\t--> min Cmax at x = %i", minX);
	fprintf(ssprDbgFile, buf);

	mcnd = mcnd->Next();
  }

  delete [] N1Cmax;
  delete [] N2Cmax;

  fflush(ssprDbgFile);
}

void MOSP_SSPR::GenerateB1Parameters(MOSP_Problem::SJob *jb, int x1, int x2)
{
  char buf[256];

  sprintf(buf, "B1 parameters for operation subpermutation <");
  fprintf(ssprDbgFile, buf);
  for(int x=x1; x<=x2; x++){
	sprintf(buf, "%s", O[seedSol->tau[jb->i][x]]->ID());
	fprintf(ssprDbgFile, buf);
	if(x < x2){
	  sprintf(buf, " -> ");
	  fprintf(ssprDbgFile, buf);
	}
	else{
	  sprintf(buf, "> in job %s:\n", jb->ID());
	  fprintf(ssprDbgFile, buf);
	}
  }

  int alpha, omega;
  seedSol->alpha_omega_B1(jb->i, x1, x2, alpha, omega);

  sprintf(buf, "\talpha = %i\n\tomega = %i\n\n", alpha, omega);
  fprintf(ssprDbgFile, buf);
  fflush(ssprDbgFile);
}

void MOSP_SSPR::GenerateB2Parameters(MOSP_Problem::SMC *mc, int x1, int x2)
{
  char buf[256];

  sprintf(buf, "B2 parameters for operation subpermutation <");
  fprintf(ssprDbgFile, buf);
  for(int x=x1; x<=x2; x++){
	sprintf(buf, "%s", O[seedSol->pi[mc->i][x]]->ID());
	fprintf(ssprDbgFile, buf);
	if(x < x2){
	  sprintf(buf, " -> ");
	  fprintf(ssprDbgFile, buf);
	}
  }
  sprintf(buf, "> existing in machine %s:\n", mc->ID());
  fprintf(ssprDbgFile, buf);

  SList::SNode *mcnd = O[seedSol->pi[mc->i][x1]]->AltMCs.head();
  while(mcnd){
	MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	MOSP_Problem::SMC *m = opmc->MC;
	int alpha, omega;

	sprintf(buf, "\t in machine %s:\n", m->ID());
	fprintf(ssprDbgFile, buf);

	seedSol->alpha_omega_B2(mc->i, m->i, x1, x2, alpha, omega);

	sprintf(buf, "\t\talpha = %i\n\t\tomega = %i\n\n", alpha, omega);
	fprintf(ssprDbgFile, buf);

	mcnd = mcnd->Next();
  }

  fflush(ssprDbgFile);
}

#endif

void MOSP_SSPR::SetTabuSize(int ts)
{
  if(ts != TabuSize){
	delete [] TabuMoves;
	TabuSize = ts;
	TabuMoves = new NSMove[TabuSize];
  }
  ClearTabus();
}

#endif

#pragma package(smart_init)
