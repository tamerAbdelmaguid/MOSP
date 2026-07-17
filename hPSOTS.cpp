//---------------------------------------------------------------------------


#pragma hdrstop

#include "hPSOTS.h"
#include "Random.h"
#include "FormhPSOTS.h"
#include "FormhPSOTSCalcProgress.h"
#include "FileSystem.h"

//#define PSO_INTERMEDIATE_RESULTS
//#define SAVE_FIRST_PARTICLE_SOL

#ifdef PSO_INTERMEDIATE_RESULTS
FILE *PSOdbgFile;
#endif

//--------------------------------------- MOSP_Particle functions definitions

MOSP_Particle::MOSP_Particle(MOSP_Problem *P, int ts, float delta0)
{
  int npcs = P->WCs.Count();
  int njbs = P->Jobs.Count();

  X = new float*[npcs];
  delta = new float*[npcs];
  MA = new int*[npcs];
  Xbest = new float*[npcs];

  for(int c=0; c<npcs; c++){
	X[c] = new float[njbs];
	delta[c] = new float[njbs];
	for(int j=0; j<njbs; j++) delta[c][j] = delta0;
	MA[c] = new int[njbs];
	Xbest[c] = new float[njbs];
  }

  Y = new float[njbs];
  O = new int[njbs];

  TS = new MOSP_TS(P, ts);
  TS->hPSOTS_AllocateMemory();
}

MOSP_Particle::~MOSP_Particle()
{
  int npcs = TS->nWCs;
  for(int c=0; c<npcs; c++){
	delete [] X[c];
	delete [] delta[c];
	delete [] MA[c];
	delete [] Xbest[c];
  }
  delete [] X;
  delete [] delta;
  delete [] MA;
  delete [] Xbest;

  delete [] Y;
  delete [] O;

  TS->hPSOTS_FreeMemory();
  delete TS;
}

void MOSP_Particle::RandomizeX()
{
  // Generate random coordinate values each within the range 0 to 10
  int npcs = TS->nWCs;
  int njbs = TS->nJobs;

  for(int c=0; c<npcs; c++){
	for(int j=0; j<njbs; j++){
	  X[c][j] = rndreal(0, 10);
	}
  }
}

void MOSP_Particle::XtoM()
{
  int npcs = TS->nWCs;
  int njbs = TS->nJobs;

  for(int c=0; c<npcs; c++){
	memcpy(Y, X[c], njbs * sizeof(float));

	for(int j=0; j<njbs; j++) O[j] = j;

	qSort(O, Y, 0, njbs-1);

	int nMCs = TS->W[c]->MCs.Count();
	int r = rnd(1, nMCs-1);
	for(int j=0; j<njbs; j++) MA[c][O[j]] = (j+r) % nMCs;
  }
}

void MOSP_Particle::DS_MWR()
{
  TS->DS_MWR(MA);
}

void MOSP_Particle::DS_MCT()
{
  TS->DS_MCT(MA);
}

void MOSP_Particle::Active_MCT()
{
  TS->Active_MCT(MA);
}

void MOSP_Particle::Construct()
{
  int r = rnd(1, 2);
  switch(r){
	 case 1:   DS_MWR(); break;
	 case 2:   DS_MCT(); break;
	 default:  Active_MCT();
  }
  TS->itr_imp = 0;
  TS->itr = 0;
}

int MOSP_Particle::Cmax()
{
  return TS->S->Cmax;
}

int MOSP_Particle::bestCmax()
{
  return TS->Sbest->Cmax;
}

int MOSP_Particle::itr_imp()
{
  return TS->itr_imp;
}

void MOSP_Particle::ClearTabu()
{
  TS->ClearTabus();
}

bool MOSP_Particle::Terminated()
{
  return TS->bTerminate;
}

void MOSP_Particle::UpdateBestPosition()
{
  if(bestCmax() < Cmax()){
	int npcs = TS->nWCs;
	int njbs = TS->nJobs;

	for(int c=0; c<npcs; c++){
	  memcpy(Xbest[c], X[c], njbs * sizeof(float));
	}
  }
}

//---------------------------------------------- hPSOTS functions definitions

MOSP_hPSOTS::MOSP_hPSOTS(MOSP_Problem *P, char *N)
  : MOSP_Algorithm(P, N)
{
}

MOSP_hPSOTS::MOSP_hPSOTS(MOSP_Problem *P, int np, long nitr_pso, int ts, long nitr_ts,
						 long nitr_imp, float tl, float omeg, float d1, float const1, float const2)
  : MOSP_Algorithm(P, "hPSO-TS"), nParticles(np), itr_max_PSO(nitr_pso), TabuSize(ts), itr_max_TS(nitr_ts),
	itr_max_imp(nitr_imp), MaxTime(tl), omega(omeg), delta0(d1), c1(const1), c2(const2)
{
}

MOSP_hPSOTS::~MOSP_hPSOTS()
{
}

void MOSP_hPSOTS::AllocateMemory()
{
  if(Solution) delete Solution;
  Solution = new MOSP_Solution(Problem);
  AllocateArrays();

  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);

  // Allocate memory for all particles
  for(int p=0; p<nParticles; p++){
	MOSP_Particle *prtcl = new MOSP_Particle(Problem, TabuSize, delta0);
	Particles.Add((void *)prtcl);
  }

  Xbest = new float*[nWCs];
  for(int c=0; c<nWCs; c++){
	Xbest[c] = new float[nJobs];
  }

  bTerminate = false;

	 #ifdef PSO_INTERMEDIATE_RESULTS
		  PSOdbgFile = fopen("PSODebug.txt", "a+");
	 #endif
}

void MOSP_hPSOTS::EvaluateLB()
{
  // -------------------------- Evaluate the lower bound for the given problem
  // ---- The first lower bound is based on Matta's work in which we use
  //      the minimum processing time on a given center on all its machines
  //      as the processing time in the case of proportionate MOSP
  //
  int i, j, m;
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

	int pk = MAXINT;   // Start with a large value for pk to find the minimum
	int k = pc->i;     // processing time among machines that belong to the center k
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
}

void MOSP_hPSOTS::Initialize()
{
  ComputationalTime = 0;
  bInterrupt = false;

  EvaluateLB();
  
  // Randomly initialzie all particles' position vectors
  //
  SList::SNode *pnd = Particles.head();
  while(pnd){
	MOSP_Particle *p = (MOSP_Particle *)pnd->Data();
	p->RandomizeX();
	pnd = pnd->Next();
  }

  // Generate an initial random solution and assign it to "Solution"
  //
  MOSP_TS *tsa = new MOSP_TS(Problem, 1, 1, 1);
  tsa->Run();
  Solution->Copy(tsa->Solution);
  delete tsa;

  bestCmax = Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN);

	 #ifdef PSO_INTERMEDIATE_RESULTS
		  char buf[256];
		  sprintf(buf, "Initial randomly generated solution:\n\n");
		  fprintf(PSOdbgFile, buf);
		  sprintf(buf, "\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fprintf(PSOdbgFile, buf);
		  for(int i=0; i<nOps; i++){
			sprintf(buf, "\t%i\t%s\t%s\t%s\t%i\t%i\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), (int)(tsk[i]->StartTime),
						 (int)(tsk[i]->EndTime));
			fprintf(PSOdbgFile, buf);
		  }
		  sprintf(buf, "\n\tCmax = %i", bestCmax);
		  fprintf(PSOdbgFile, buf);
		  sprintf(buf, "\n\tLB = %i", LB);
		  fprintf(PSOdbgFile, buf);
		  fprintf(PSOdbgFile, "\n\n");
	 #endif

  itr_PSO = itr_TS = 1;

  // For all particles, convert X to MA and use DS/MWR algorithm with machine
  // assignments in MA to generate initial solutions
  //
	 #ifdef PSO_INTERMEDIATE_RESULTS
		  int pi = 1;
	 #endif

  pnd = Particles.head();
  while(pnd){
	MOSP_Particle *p = (MOSP_Particle *)pnd->Data();
	p->XtoM();
	p->Construct();

	 #ifdef PSO_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\tParticle #%i:\n\n\t\t\tX = ", pi);
		  fprintf(PSOdbgFile, buf);
		  for(int c=0; c<nWCs; c++){
			for(int j=0; j<nJobs; j++){
			  sprintf(buf, "\t%2.2f", p->X[c][j]);
			  fprintf(PSOdbgFile, buf);
			}
			sprintf(buf, "\n\t\t\t");
			fprintf(PSOdbgFile, buf);
		  }
		  sprintf(buf, "\n\n\t\t\tM = ");
		  fprintf(PSOdbgFile, buf);
		  for(int c=0; c<nWCs; c++){
			for(int j=0; j<nJobs; j++){
			  sprintf(buf, "\t%i", p->MA[c][j]);
			  fprintf(PSOdbgFile, buf);
			}
			sprintf(buf, "\n\t\t\t");
			fprintf(PSOdbgFile, buf);
		  }
		  sprintf(buf, "\n\n\t\t\tNetwork solution: Cmax = %i\n", p->Cmax());
		  fprintf(PSOdbgFile, buf);
		  fprintf(PSOdbgFile, "\t\t\t\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\thead\ttail\tCritical\n");
		  for(int i=0; i<nOps; i++){
			sprintf(buf, "\t\t\t\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%c\n",
						 i, p->TS->O[i]->ID(), p->TS->S->Nodes[i].j_prv, p->TS->S->Nodes[i].j_nxt,
						 p->TS->M[p->TS->S->Nodes[i].mc]->ID(), p->TS->S->Nodes[i].m_prv,
						 p->TS->S->Nodes[i].m_nxt, p->TS->S->Nodes[i].head,
						 p->TS->S->Nodes[i].tail, ((p->TS->S->Nodes[i].bCritical)? '*' : ' '));
			fprintf(PSOdbgFile, buf);
		  }
		  fprintf(PSOdbgFile, "\n\n");
		  pi++;
	 #endif

	 #ifdef SAVE_FIRST_PARTICLE_SOL
		  if(pnd == Particles.head()){
			MOSP_FIO_RESULT Result;
		    SaveSolution(Problem, p->TS->Solution, "hPSOTSSol0.mosp.sol", Result);
		  }
	 #endif

	pnd = pnd->Next();
  }              // Note: calling DS_MWR sets itr_imp = 0 and clears tabu lists
				 //       for all particles

  frmhPSOTSCalcProgress->ProgressBar->Max = itr_max_PSO;
  frmhPSOTSCalcProgress->lblIterTS->Caption = "0";
  frmhPSOTSCalcProgress->ProgressBar->Position = 0;
  frmhPSOTSCalcProgress->lblIter->Caption = "0";
  frmhPSOTSCalcProgress->lblLB->Caption = IntToStr(LB);
  frmhPSOTSCalcProgress->lblInitialCmax->Caption = IntToStr(bestCmax);
  frmhPSOTSCalcProgress->lblBestCmax->Caption = IntToStr(bestCmax);  
}

void MOSP_hPSOTS::Improve()
{
  SList::SNode *pnd = Particles.head();
  while(pnd){
	MOSP_Particle *p = (MOSP_Particle *)pnd->Data();
	if(!p->Terminated()){
	  p->TS->Improve();
	  p->UpdateBestPosition();
	  if(p->bestCmax() < bestCmax){ // Update global best positions
		p->TS->ConvertNetSol();
		Solution->Copy(p->TS->Solution);
		bestCmax = p->bestCmax();

		for(int c=0; c<nWCs; c++){
		  memcpy(Xbest[c], p->X[c], nJobs * sizeof(float));
		}
	  }
	}
	pnd = pnd->Next();
  }

  itr_TS++;
  if(itr_TS == itr_max_TS){
	// Update all particles' positions
	UpdateParticlesPositions();

	// For each particle, use DS/MWR algorithm to move to the new position
	UpdateParticlesSolutions();

	itr_TS = 1;
	itr_PSO++;
  }
}

bool MOSP_hPSOTS::Stop()
{
  frmhPSOTSCalcProgress->lblBestCmax->Caption = IntToStr(bestCmax);

  if(bestCmax == LB) bTerminate = true;

  if(bTerminate){
	frmhPSOTSCalcProgress->btnStart->Enabled = true;
	frmhPSOTSCalcProgress->btnClose->Enabled = true;
	frmhPSOTSCalcProgress->btnTerminate->Enabled = false;
	frmhPSOTSCalcProgress->ProgressBar->Position = itr_PSO;
	frmhPSOTSCalcProgress->lblIterTS->Caption = IntToStr(itr_TS);
	frmhPSOTSCalcProgress->lblIter->Caption = IntToStr(itr_PSO);
	return true;
  }

  timeb curTime;
  ::ftime(&curTime);
  ComputationalTime = (curTime.time - start_time.time)*1000.0 +
					  (curTime.millitm - start_time.millitm);

  if(itr_PSO > itr_max_PSO || ComputationalTime >= MaxTime){
	frmhPSOTSCalcProgress->btnStart->Enabled = true;
	frmhPSOTSCalcProgress->btnClose->Enabled = true;
	frmhPSOTSCalcProgress->btnTerminate->Enabled = false;
	frmhPSOTSCalcProgress->ProgressBar->Position = itr_PSO;
	frmhPSOTSCalcProgress->lblIterTS->Caption = IntToStr((int)itr_max_TS);
	return true;
  }
  else{
	frmhPSOTSCalcProgress->lblIterTS->Caption = IntToStr(itr_TS);
	frmhPSOTSCalcProgress->ProgressBar->Position = itr_PSO;
	frmhPSOTSCalcProgress->lblIter->Caption = IntToStr(itr_PSO);
	return false;
  }
}

void MOSP_hPSOTS::Finalize()
{
  MOSP_Algorithm::Finalize();
}

void MOSP_hPSOTS::FreeMemory()
{
  FreeArrays();

  // Free memory for all particles
  SList::SNode *pnd = Particles.head();
  while(pnd){
	MOSP_Particle *prtcl = (MOSP_Particle *)pnd->Data();
	delete prtcl;
	pnd = pnd->Next();
  }
  Particles.Clear();

  for(int c=0; c<nWCs; c++){
	delete [] Xbest[c];
  }
  delete [] Xbest;

	 #ifdef PSO_INTERMEDIATE_RESULTS
			  fclose(PSOdbgFile);
	 #endif
}

void MOSP_hPSOTS::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

void MOSP_hPSOTS::UpdateParticlesPositions()
{
  SList::SNode *pnd = Particles.head();
  while(pnd){
	MOSP_Particle *p = (MOSP_Particle *)pnd->Data();
	for(int c=0; c<nWCs; c++){
	  for(int j=0; j<nJobs; j++){
		float r1 = frandom();
		float r2 = frandom();
		p->delta[c][j] = omega * p->delta[c][j] + c1 * r1 * (p->Xbest[c][j] - p->X[c][j])
												+ c2 * r2 * (Xbest[c][j] - p->X[c][j]);
		// apply maximum and minimum limits on delta
		//if(p->delta[c][j] > 2.0) p->delta[c][j] = 2.0;
		//if(p->delta[c][j] < -2.0) p->delta[c][j] = -2.0;
		p->X[c][j] += p->delta[c][j];
	  }
	}
	pnd = pnd->Next();
  }
}

void MOSP_hPSOTS::UpdateParticlesSolutions()
{
	 #ifdef PSO_INTERMEDIATE_RESULTS
		  int pi = 1;
		  char buf[256];
		  sprintf(buf, "\n\nPSO iteration #%i\n\n", itr_PSO);
		  fprintf(PSOdbgFile, buf);
	 #endif

  SList::SNode *pnd = Particles.head();
  while(pnd){
	MOSP_Particle *p = (MOSP_Particle *)pnd->Data();
	p->XtoM();

	 #ifdef PSO_INTERMEDIATE_RESULTS
		  sprintf(buf, "\t\tParticle #%i:\n\n\t\t\tX = ", pi);
		  fprintf(PSOdbgFile, buf);
		  for(int c=0; c<nWCs; c++){
			for(int j=0; j<nJobs; j++){
			  sprintf(buf, "\t%2.2f", p->X[c][j]);
			  fprintf(PSOdbgFile, buf);
			}
			sprintf(buf, "\n\t\t\t");
			fprintf(PSOdbgFile, buf);
		  }
		  sprintf(buf, "\n\n\t\t\tM = ");
		  fprintf(PSOdbgFile, buf);
		  for(int c=0; c<nWCs; c++){
			for(int j=0; j<nJobs; j++){
			  sprintf(buf, "\t%i", p->MA[c][j]);
			  fprintf(PSOdbgFile, buf);
			}
			sprintf(buf, "\n\t\t\t");
			fprintf(PSOdbgFile, buf);
		  }
		  fprintf(PSOdbgFile, "\n");
	 #endif

	p->Construct();

	 #ifdef PSO_INTERMEDIATE_RESULTS
		  sprintf(buf, "\n\n\t\t\tNetwork solution: Cmax = %i\n", p->Cmax());
		  fprintf(PSOdbgFile, buf);
		  fprintf(PSOdbgFile, "\t\t\t\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\thead\ttail\tCritical\n");
		  for(int i=0; i<nOps; i++){
			sprintf(buf, "\t\t\t\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%c\n",
						 i, p->TS->O[i]->ID(), p->TS->S->Nodes[i].j_prv, p->TS->S->Nodes[i].j_nxt,
						 p->TS->M[p->TS->S->Nodes[i].mc]->ID(), p->TS->S->Nodes[i].m_prv,
						 p->TS->S->Nodes[i].m_nxt, p->TS->S->Nodes[i].head,
						 p->TS->S->Nodes[i].tail, ((p->TS->S->Nodes[i].bCritical)? '*' : ' '));
			fprintf(PSOdbgFile, buf);
		  }
		  fprintf(PSOdbgFile, "\n\n");
		  pi++;
	 #endif

	pnd = pnd->Next();
  }              // Note: calling DS_MWR sets itr_imp = 0 and clears tabu lists
				 //       for all particles
}

#pragma package(smart_init)
