//---------------------------------------------------------------------------


#pragma hdrstop

#include "PSI.h"
#include "FormTSCalcProgress.h"
#include "FormTS.h"
#include "FileSystem.h"

//------------------------ Definitions for processing sequence class functions

//-------------------- Processing sequence on a given machine

bool MOSP_PS::AppendOp(MOSP_Problem::SOperation *op)
{
  if(PS.IndexOf((void *)op) != -1) return false;
  PS.Add(op);
  return true;
}

bool MOSP_PS::InsertOp(MOSP_Problem::SOperation *newop, MOSP_Problem::SOperation *op)
{
  if(PS.IndexOf((void *)newop) != -1) return false;
  SList::SNode *curNode = NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	MOSP_Problem::SOperation *eop = (MOSP_Problem::SOperation *)nd->Data();
	if(eop == op) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL) return false;

  PS.AddBefore(curNode, newop);
  return true;
}

bool MOSP_PS::InsertOp(MOSP_Problem::SOperation *newop, int pos)
{
  if(pos < 0 || pos > PS.Count()) return false;
  if(pos == PS.Count()){
	PS.Add(newop);
	return true;
  }
  SList::SNode *nd = PS.head();
  while(pos != 0){ nd = nd->Next();   pos--;  }
  if(nd){
	PS.AddBefore(nd, newop);
	return true;
  }
  return false;
}

bool MOSP_PS::Swap(MOSP_Problem::SOperation *op1, MOSP_Problem::SOperation *op2)
{
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	if(op==op1) nd1 = nd;
	if(op==op2) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	nd = nd->Next();
  }
  if(nd1 == NULL || nd2 == NULL) return false;
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

bool MOSP_PS::Swap(int pos1, int pos2)
{
  if(pos1<0 || pos1>PS.Count() || pos2<0 || pos2>PS.Count() || pos1==pos2)
	return false;
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	if(pos1 == 0) nd1 = nd;
	if(pos2 == 0) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	pos1--;    pos2--;
	nd = nd->Next();
  }
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

//-------------------- Vector of processing sequences on machines

MOSP_VPS::MOSP_VPS(MOSP_Problem *P)
{
  Problem = P;
  SList::SNode *wcnd = Problem->WCs.head();
  while(wcnd){
	MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  PSs.Add(new MOSP_PS(mc));
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
}

MOSP_VPS::MOSP_VPS(MOSP_VPS *vps)
{
  Problem = vps->Problem;
  SList::SNode *wcnd = Problem->WCs.head();
  while(wcnd){
	MOSP_Problem::SWC *wc = (MOSP_Problem::SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  MOSP_PS *newps = new MOSP_PS(mc);
	  PSs.Add(newps);
	  newps->PS.Add(vps->PS(mc)->PS);
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
}

MOSP_PS *MOSP_VPS::PS(MOSP_Problem::SMC *mc)
{
  MOSP_PS *ret = NULL;
  SList::SNode *nd = PSs.head();
  while(nd){
	MOSP_PS *ps = (MOSP_PS *)nd->Data();
	if(ps->MC == mc){ ret = ps;   break; }
	nd = nd->Next();
  }
  return ret;
}

MOSP_VPS::~MOSP_VPS()
{
  SList::SNode *nd = PSs.head();
  while(nd){
	MOSP_PS *ps = (MOSP_PS *)nd->Data();
	delete ps;
    nd = nd->Next();
  }
  PSs.Clear();
}

bool MOSP_VPS::AppendOp(MOSP_Problem::SMC *mc, MOSP_Problem::SOperation *op)
{
  MOSP_PS *ps=NULL;
  SList::SNode *psnd = PSs.head();
  while(psnd){
	ps = (MOSP_PS *)psnd->Data();
	if(ps->MC == mc) break;
    psnd = psnd->Next();
  }
  if(!ps) return false;
  return ps->AppendOp(op);
}

bool MOSP_VPS::isIncluded(MOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	MOSP_PS *ps = (MOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC){
      psnd = psnd->Next();
	  continue;
	}
	if(ps->PS.IndexOf(op) != -1) return true;
	psnd = psnd->Next();
  }
  return false;
}

bool MOSP_VPS::operator ==(MOSP_VPS &vps)
{
  if(Problem != vps.Problem) return false;

  SList::SNode *psnd1, *psnd2;
  psnd1 = PSs.head();
  psnd2 = vps.PSs.head();
  while(psnd1 && psnd2){
	MOSP_PS *ps1 = (MOSP_PS *)psnd1->Data();
	MOSP_PS *ps2 = (MOSP_PS *)psnd2->Data();
	if(ps1->PS.Count() != ps2->PS.Count() || ps1->MC != ps2->MC) return false;
    SList::SNode *opnd1, *opnd2;
    opnd1 = ps1->PS.head();
    opnd2 = ps2->PS.head();
	for(int j=0; j<ps1->PS.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return false;
	  opnd1 = opnd1->Next();
      opnd2 = opnd2->Next();
    }
    psnd1 = psnd1->Next();
    psnd2 = psnd2->Next();
  }
  return true;
}

bool MOSP_VPS::operator !=(MOSP_VPS &vps)
{
  if(Problem != vps.Problem) return true;

  SList::SNode *psnd1, *psnd2;
  psnd1 = PSs.head();
  psnd2 = vps.PSs.head();
  while(psnd1 && psnd2){
	MOSP_PS *ps1 = (MOSP_PS *)psnd1->Data();
	MOSP_PS *ps2 = (MOSP_PS *)psnd2->Data();
	if(ps1->PS.Count() != ps2->PS.Count() || ps1->MC != ps2->MC) return true;
	SList::SNode *opnd1, *opnd2;
	opnd1 = ps1->PS.head();
	opnd2 = ps2->PS.head();
	for(int j=0; j<ps1->PS.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return true;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	psnd1 = psnd1->Next();
	psnd2 = psnd2->Next();
  }
  return false;
}

bool MOSP_VPS::Copy(MOSP_VPS *vps)
{
  if(vps->Problem != Problem) return false;
  SList::SNode *psnd = PSs.head(), *psndg = vps->PSs.head();
  while(psnd){
    MOSP_PS *ps = (MOSP_PS *)psnd->Data();
    MOSP_PS *psg = (MOSP_PS *)psndg->Data();
    ps->PS.Clear();
	ps->MC = psg->MC;
    ps->PS.Add(psg->PS);
    psnd = psnd->Next();
    psndg = psndg->Next();
  }
  return true;
}

MOSP_VPS& MOSP_VPS::operator =(MOSP_VPS &vps)
{
  if(vps.Problem != Problem) return *this;
  SList::SNode *psnd = PSs.head(), *psndg = vps.PSs.head();
  while(psnd){
    MOSP_PS *ps = (MOSP_PS *)psnd->Data();
    MOSP_PS *psg = (MOSP_PS *)psndg->Data();
    ps->PS.Clear();
	ps->MC = psg->MC;
    ps->PS.Add(psg->PS);
    psnd = psnd->Next();
	psndg = psndg->Next();
  }
  return *this;
}

bool MOSP_VPS::isComplete()
{
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  if(!isIncluded(op)) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }
  return true;
}

MOSP_Problem::SMC *MOSP_VPS::MCAssignedToOP(MOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	MOSP_PS *ps = (MOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC) continue;
	if(ps->PS.IndexOf(op) != -1) return ps->MC;
	psnd = psnd->Next();
  }
  return NULL;
}

MOSP_PS *MOSP_VPS::PSContainingOP(MOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	MOSP_PS *ps = (MOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC) continue;
	if(ps->PS.IndexOf(op) != -1) return ps;
	psnd = psnd->Next();
  }
  return NULL;
}

//------------ Definitions of operations order within jobs management functions

//-------------------- Order of operations within a given job

bool MOSP_OO::AppendOp(MOSP_Problem::SOperation *op)
{
  if(OO.IndexOf((void *)op) != -1) return false;
  OO.Add(op);
  return true;
}

bool MOSP_OO::InsertOp(MOSP_Problem::SOperation *newop, MOSP_Problem::SOperation *op)
{
  if(OO.IndexOf((void *)newop) != -1) return false;
  SList::SNode *curNode = NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	MOSP_Problem::SOperation *eop = (MOSP_Problem::SOperation *)nd->Data();
	if(eop == op) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL) return false;

  OO.AddBefore(curNode, newop);
  return true;
}

bool MOSP_OO::InsertOp(MOSP_Problem::SOperation *newop, int pos)
{
  if(pos < 0 || pos > OO.Count()) return false;
  if(pos == OO.Count()){
	OO.Add(newop);
	return true;
  }
  SList::SNode *nd = OO.head();
  while(pos != 0){ nd = nd->Next();   pos--;  }
  if(nd){
	OO.AddBefore(nd, newop);
	return true;
  }
  return false;
}

bool MOSP_OO::Swap(MOSP_Problem::SOperation *op1, MOSP_Problem::SOperation *op2)
{
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	if(op==op1) nd1 = nd;
	if(op==op2) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	nd = nd->Next();
  }
  if(nd1 == NULL || nd2 == NULL) return false;
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

bool MOSP_OO::Swap(int pos1, int pos2)
{
  if(pos1<0 || pos1>OO.Count() || pos2<0 || pos2>OO.Count() || pos1==pos2)
	return false;
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	if(pos1 == 0) nd1 = nd;
	if(pos2 == 0) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	pos1--;    pos2--;
	nd = nd->Next();
  }
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

//-------------------- Vector of operations orders within jobs

MOSP_VOO::MOSP_VOO(MOSP_Problem *P)
{
  Problem = P;
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jbnd->Data();
	OOs.Add(new MOSP_OO(jb));
	jbnd = jbnd->Next();
  }
}

MOSP_VOO::MOSP_VOO(MOSP_VOO *voo)
{
  Problem = voo->Problem;
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jbnd->Data();
	MOSP_OO *newoo = new MOSP_OO(jb);
	OOs.Add(newoo);
	newoo->OO.Add(voo->OO(jb)->OO);
	jbnd = jbnd->Next();
  }
}

MOSP_OO *MOSP_VOO::OO(MOSP_Problem::SJob *jb)
{
  MOSP_OO *ret = NULL;
  SList::SNode *nd = OOs.head();
  while(nd){
	MOSP_OO *oo = (MOSP_OO *)nd->Data();
	if(oo->job == jb){ ret = oo;   break; }
	nd = nd->Next();
  }
  return ret;
}

MOSP_VOO::~MOSP_VOO()
{
  SList::SNode *nd = OOs.head();
  while(nd){
	MOSP_OO *oo = (MOSP_OO *)nd->Data();
	delete oo;
	nd = nd->Next();
  }
  OOs.Clear();
}

bool MOSP_VOO::AppendOp(MOSP_Problem::SOperation *op)
{
  MOSP_OO *oo=NULL;
  SList::SNode *oond = OOs.head();
  while(oond){
	oo = (MOSP_OO *)oond->Data();
	if(oo->job == op->Job) break;
	oond = oond->Next();
  }
  if(!oo) return false;
  return oo->AppendOp(op);
}

bool MOSP_VOO::isIncluded(MOSP_Problem::SOperation *op)
{
  SList::SNode *oond = OOs.head();
  while(oond){
	MOSP_OO *oo = (MOSP_OO *)oond->Data();
	if(oo->job != op->Job){
	  oond = oond->Next();
	  continue;
	}
	if(oo->OO.IndexOf(op) != -1) return true;
	oond = oond->Next();
  }
  return false;
}

bool MOSP_VOO::operator ==(MOSP_VOO &voo)
{
  if(Problem != voo.Problem) return false;

  SList::SNode *oond1, *oond2;
  oond1 = OOs.head();
  oond2 = voo.OOs.head();
  while(oond1 && oond2){
	MOSP_OO *oo1 = (MOSP_OO *)oond1->Data();
	MOSP_OO *oo2 = (MOSP_OO *)oond2->Data();
	if(oo1->OO.Count() != oo2->OO.Count() || oo1->job != oo2->job) return false;
	SList::SNode *opnd1, *opnd2;
	opnd1 = oo1->OO.head();
	opnd2 = oo2->OO.head();
	for(int j=0; j<oo1->OO.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return false;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	oond1 = oond1->Next();
	oond2 = oond2->Next();
  }
  return true;
}

bool MOSP_VOO::operator !=(MOSP_VOO &voo)
{
  if(Problem != voo.Problem) return true;

  SList::SNode *oond1, *oond2;
  oond1 = OOs.head();
  oond2 = voo.OOs.head();
  while(oond1 && oond2){
	MOSP_OO *oo1 = (MOSP_OO *)oond1->Data();
	MOSP_OO *oo2 = (MOSP_OO *)oond2->Data();
	if(oo1->OO.Count() != oo2->OO.Count() || oo1->job != oo2->job) return true;
	SList::SNode *opnd1, *opnd2;
	opnd1 = oo1->OO.head();
	opnd2 = oo2->OO.head();
	for(int j=0; j<oo1->OO.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return true;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	oond1 = oond1->Next();
	oond2 = oond2->Next();
  }
  return false;
}

bool MOSP_VOO::Copy(MOSP_VOO *voo)
{
  if(voo->Problem != Problem) return false;
  SList::SNode *oond = OOs.head(), *oondg = voo->OOs.head();
  while(oond){
	MOSP_OO *oo = (MOSP_OO *)oond->Data();
	MOSP_OO *oog = (MOSP_OO *)oondg->Data();
	oo->OO.Clear();
	oo->job = oog->job;
	oo->OO.Add(oog->OO);
	oond = oond->Next();
	oondg = oondg->Next();
  }
  return true;
}

MOSP_VOO& MOSP_VOO::operator =(MOSP_VOO &voo)
{
  if(voo.Problem != Problem) return *this;
  SList::SNode *oond = OOs.head(), *oondg = voo.OOs.head();
  while(oond){
	MOSP_OO *oo = (MOSP_OO *)oond->Data();
	MOSP_OO *oog = (MOSP_OO *)oondg->Data();
	oo->OO.Clear();
	oo->job = oog->job;
	oo->OO.Add(oog->OO);
	oond = oond->Next();
	oondg = oondg->Next();
  }
  return *this;
}

bool MOSP_VOO::isComplete()
{
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  if(!isIncluded(op)) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }
  return true;
}

//--------------- A Simple processing sequence and job order interpretter

MOSP_PSI::MOSP_PSI(MOSP_Problem *P, MOSP_VPS *vps, MOSP_VOO *voo, char *N)
  : MOSP_Algorithm(P, N), VPS(vps), VOO(voo)
{
}

MOSP_PSI::MOSP_PSI(MOSP_Problem *P, MOSP_VPS *vps, MOSP_VOO *voo)
  : MOSP_Algorithm(P, "Simple Processing Sequnce Interpretter (PSI)"),
	VPS(vps), VOO(voo)
{
}

MOSP_PSI::~MOSP_PSI()
{
}

void MOSP_PSI::AllocateMemory()
{
  MOSP_Algorithm::AllocateMemory();
  tmpVPS = new MOSP_VPS(&VPS);
  tmpVOO = new MOSP_VOO(&VOO);
  nxt = new int[Problem->Jobs.Count()];
  mcrt = new float[Problem->nMachines()];
  jbrt = new float[Problem->Jobs.Count()];
}

void MOSP_PSI::Initialize()
{
  SList::SNode *nd, *wcnd, *mcnd, *jbnd, *opnd;
  int i, j, m;

  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
	opnd = tmpVOO->OO(J[j])->OO.head();
	if(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  nxt[j] = op->i;   // index of the next schedulable operation based on OOs
	}
	else nxt[j] = -1;
  }

  bool bContinue;
  do{
	bContinue = false;
	nd = tmpVPS->PSs.head();
	while(nd){
	  MOSP_PS *ps = (MOSP_PS *)nd->Data();
	  if(ps->PS.Count() != 0){
		MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)(ps->PS[0]);
		j = op->Job->i;  // job index
		i = op->i; // operation index
		m = ps->MC->i;  // machine index
		if(nxt[j] == i){
		  MOSP_Solution::SchTask *task = tsk[i];
		  task->SelectedMC = ps->MC;
		  task->StartTime = fmax(jbrt[j], mcrt[m]);
		  task->EndTime = task->StartTime + pt[j][m];
		  task->bScheduled = true;
		  jbrt[j] = task->EndTime;
		  mcrt[m] = task->EndTime;

		  MOSP_OO *joo = tmpVOO->OO(J[j]);
		  joo->OO.Remove(joo->OO.head());
		  opnd = joo->OO.head();
		  if(opnd){
			MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
			nxt[j] = op->i;   // index of the next schedulable operation based on OOs
		  }
		  else nxt[j] = -1;

		  ps->PS.Remove(ps->PS.head());
		  bContinue = true;
		  break;
		}
	  }
	  nd = nd->Next();
	}
  }while(bContinue);

  bool bComplete = true;          // check for infeasible processing sequence
  nd = tmpVPS->PSs.head();
  while(nd){
	MOSP_PS *ps = (MOSP_PS *)nd->Data();
	if(ps->PS.Count() != 0){
	  bComplete = false;
	  break;
	}
	nd = nd->Next();
  }
  if(!bComplete) SolStatus = INFEASIBLE;
  else SolStatus = FEASIBLE;
}

bool MOSP_PSI::Stop()
{
  return true;
}

void MOSP_PSI::Finalize()
{
  MOSP_Algorithm::Finalize();
}

void MOSP_PSI::FreeMemory()
{
  MOSP_Algorithm::FreeMemory();

  delete [] nxt;
  delete [] mcrt;
  delete [] jbrt;
  delete tmpVPS;
  delete tmpVOO;
}

void MOSP_PSI::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
