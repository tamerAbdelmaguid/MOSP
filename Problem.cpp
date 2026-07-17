//--- This code is Copyright 2014, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#include <math.h>
#include "Problem.h"
#include "random.h"

MOSP_Problem::SWC::~SWC()
{
  SList::SNode *mcnd = MCs.head();
  while(mcnd){
	MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	delete mc;
	mcnd = mcnd->Next();
  }
  MCs.Clear();
}

MOSP_Problem::SJob::~SJob()
{
  SList::SNode *nd = Operations.head();
  while(nd){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)nd->Data();
	delete op;
	nd = nd->Next();
  }
  Operations.Clear();
}

bool MOSP_Problem::SJob::bUsedWC(MOSP_Problem::SWC *wc)
{
  SList::SNode *opnd = Operations.head();
  while(opnd){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	if(op->WC == wc) return true;
	opnd = opnd->Next();
  }
  return false;
}

MOSP_Problem::SOperation::SOperation(char *ID, char *Des, MOSP_Problem::SJob *jb, MOSP_Problem::SWC *wc, float *t)
   : SCH_Entity(ID, Des), Job(jb), WC(wc)
{
  int i=0;
  if(t != NULL){
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  MOSP_Problem::SOpMC *opmc = new MOSP_Problem::SOpMC(mc, t[i++]);
	  AltMCs.Add(opmc);
	  mcnd = mcnd->Next();
	}
  }
}

MOSP_Problem::SOperation::SOperation(char *ID, char *Des, MOSP_Problem::SJob *jb, MOSP_Problem::SWC *wc, float t)
   : SCH_Entity(ID, Des), Job(jb), WC(wc)
{
  SList::SNode *mcnd = wc->MCs.head();
  while(mcnd){
	MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	MOSP_Problem::SOpMC *opmc = new MOSP_Problem::SOpMC(mc, t);
	AltMCs.Add(opmc);
	mcnd = mcnd->Next();
  }
}

MOSP_Problem::SOperation::~SOperation()
{
  SList::SNode *opmcnd = AltMCs.head();
  while(opmcnd){
	MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)opmcnd->Data();
    delete opmc;
	opmcnd = opmcnd->Next();
  }
  AltMCs.Clear();
}

float MOSP_Problem::SOperation::pt(MOSP_Problem::SMC *mc)
{
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->MC == mc) return opmc->pt;
	nd = nd->Next();
  }
  return -1;
}

float MOSP_Problem::SOperation::minpt()
{
  float minpt = MAXFLOAT;
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->pt < minpt) return minpt = opmc->pt;
	nd = nd->Next();
  }
  return minpt;
}

float MOSP_Problem::SOperation::maxpt()
{
  float maxpt = 0;
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->pt > maxpt) return maxpt = opmc->pt;
	nd = nd->Next();
  }
  return maxpt;
}

//---------------------------------------------------------- MOSP_Problem

MOSP_Problem::MOSP_Problem(char *ID, char *Des, MOSP_Problem::TObjective O)
   : SCH_Entity (ID, Des), Objective(O)
{
}

MOSP_Problem::MOSP_Problem(char *ID, char *Des, int nWCs, int nJobs, int *nMCs, int *pt)
   : SCH_Entity (ID, Des), Objective(MIN_MAKESPAN)    // A constructor for Matta's PMOSP instances
{
  char IDbuf[8];
  char Desbuf[65];

  // Construct WCs
  for(int i=0; i<nWCs; i++){
	sprintf(IDbuf, "C%i", i+1);
	sprintf(Desbuf, "Processing center #%i", i+1);
	SWC *newWC = new SWC(IDbuf, Desbuf);
	WCs.Add((void *)newWC);

	// Construct MCs
	for(int j=0; j<nMCs[i]; j++){
	  sprintf(IDbuf, "M%i-%i", i+1, j+1);
	  sprintf(Desbuf, "Machine #%i of center #%i", i+1, j+1);
	  SMC *newMC = new SMC(IDbuf, Desbuf, newWC, 0);
	  newWC->MCs.Add((void *)newMC);
	}
  }

  // Construct Jobs
  for(int i=0; i<nJobs; i++){
	sprintf(IDbuf, "J%i", i+1);
	sprintf(Desbuf, "Job order #%i", i+1);
	SJob *newjob = new SJob(IDbuf, Desbuf, 0, 0, 1);
	Jobs.Add((void *)newjob);

	// Construct Operations
	SList::SNode *wcnd = WCs.head();
	for(int j=0; j<nWCs; j++){
	  SWC *wc = (SWC *)wcnd->Data();
	  sprintf(IDbuf, "%i-%i", i+1, j+1);
	  sprintf(Desbuf, "Operation #%i of job J%i", j+1, i+1);
	  SOperation *newop = new SOperation(IDbuf, Desbuf, newjob, wc, pt[j]);
	  newjob->Operations.Add((void *)newop);
	  wcnd = wcnd->Next();
	}
  }
}

MOSP_Problem::MOSP_Problem(char *ID, char *Des, int nWCs, int nJobs,
			 int qwLevel, int LL, int PTVL)
	 : SCH_Entity (ID, Des), Objective(MIN_MAKESPAN)   // Specially designed MOSP instances
{
  char IDbuf[8];
  char Desbuf[65];

  warmup_random(random(1000)/1000.);

  float *mainPT = new float[nWCs];

  // Construct WCs
  for(int w=0; w<nWCs; w++){
	sprintf(IDbuf, "C%i", w+1);
	sprintf(Desbuf, "Processing center #%i", w+1);
	SWC *newWC = new SWC(IDbuf, Desbuf);
	WCs.Add((void *)newWC);

	mainPT[w] = rndreal(20, 60);

	// Construct MCs
	int nMCs;
	if(qwLevel != 0) nMCs = (qwLevel == 1)? rnd(1, 3) : rnd(4, 6);
	else nMCs = rnd(1, 4);
	for(int m=0; m<nMCs; m++){
	  sprintf(IDbuf, "M%i-%i", w+1, m+1);
	  sprintf(Desbuf, "Machine #%i of center #%i", m+1, w+1);
	  SMC *newMC = new SMC(IDbuf, Desbuf, newWC, 0);
	  newWC->MCs.Add((void *)newMC);
	}
  }

  // Construct Jobs
  for(int j=0; j<nJobs; j++){
	sprintf(IDbuf, "J%i", j+1);
	sprintf(Desbuf, "Job order #%i", j+1);
	SJob *newjob = new SJob(IDbuf, Desbuf, 0, 0, 1);

	// Construct Operations
	SList::SNode *wcnd = WCs.head();
	for(int w=0; w<nWCs; w++){
	  if(LL == 1){
	    if(flip(0.6)){
		  wcnd = wcnd->Next();
		  continue;
		}
	  }
	  else if(LL == 2){
		if(flip(0.3)){
		  wcnd = wcnd->Next();
		  continue;
		}
	  }
	  else if(LL == 3){
		if(flip(0.1)){
		  wcnd = wcnd->Next();
		  continue;
		}
	  }
	  else{
		if(flip(0.6)){
		  wcnd = wcnd->Next();
		  continue;
		}
	  }

	  SWC *wc = (SWC *)wcnd->Data();
	  sprintf(IDbuf, "%i-%i", j+1, w+1);
	  sprintf(Desbuf, "Operation job J%i on workstation #%i", j+1, w+1);

	  float ptf;
	  float *pt = new float[wc->MCs.Count()];

      int m = 0;
	  for(SList::SNode *mcnd = wc->MCs.head(); mcnd != NULL; mcnd = mcnd->Next()){
		if(PTVL == 1) ptf = rndreal(0.95, 1.05);
		else if(PTVL == 2) ptf = rndreal(0.75, 1.25);
		else ptf = rndreal(0.7, 1.3);
		pt[m] = ceil(ptf * mainPT[w] * float(wc->MCs.Count()));
		m++;
	  }

	  SOperation *newop = new SOperation(IDbuf, Desbuf, newjob, wc, pt);
	  newjob->Operations.Add((void *)newop);
	  wcnd = wcnd->Next();

	  delete [] pt;
	}

	if(newjob->Operations.Count() == 0){
	  delete newjob;
	}
	else Jobs.Add((void *)newjob);
  }

  delete [] mainPT;
}

MOSP_Problem::~MOSP_Problem()
{
  SList::SNode *nd;

  nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	delete wc;
	nd = nd->Next();
  }
  WCs.Clear();

  nd = Jobs.head();
  while(nd){
    SJob *jb = (SJob *)nd->Data();
    delete jb;
    nd = nd->Next();
  }
  Jobs.Clear();
}

MOSP_Problem::SWC *MOSP_Problem::AddWC(char *ID, char *Des)
{
  if(UsedID(ID)) return NULL;
  SWC *newWC = new SWC(ID, Des);
  WCs.Add(newWC);
  return newWC;
}

MOSP_Problem::SWC *MOSP_Problem::InsertWC(SWC *curWC, char *ID, char *Des)
{
  if(UsedID(ID)) return NULL;
  SWC *newWC = new SWC(ID, Des);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *ewc = (SWC *)nd->Data();
	if(ewc == curWC){
	  curNode = nd;
	  break;
	}
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newWC;
	return NULL;
  }

  WCs.AddBefore(curNode, newWC);

  return newWC;
}

MOSP_Problem::SWC *MOSP_Problem::InsertWC(int icurWC, char *ID, char *Des)
{
  if(icurWC < 0 || icurWC >= WCs.Count()) return NULL;
  return InsertWC((SWC *)(WCs[icurWC]), ID, Des);
}

MOSP_Problem::SMC *MOSP_Problem::AddMC(char *ID, char *Des, MOSP_Problem::SWC *wc, float rt)
{
  if(UsedID(ID)) return NULL;
  SMC *newmc = new SMC(ID, Des, wc, rt);
  wc->MCs.Add(newmc);

  // loop through all operations to add the new machine to the list of alternative
  // machines whenever an operation uses wc
  //
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == wc){
		SOpMC *newopmc = new SOpMC(newmc);
		op->AltMCs.Add(newopmc);
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  return newmc;
}

MOSP_Problem::SMC *MOSP_Problem::InsertMC(MOSP_Problem::SMC *curMC, char *ID, char *Des, MOSP_Problem::SWC *wc, float rt)
{
  if(UsedID(ID)) return NULL;
  SMC *newmc = new SMC(ID, Des, wc, rt);

  SList::SNode *curnd = NULL;
  SList::SNode *mcnd = wc->MCs.head();
  while(mcnd){
	SMC *emc = (SMC *)mcnd->Data();
	if(emc == curMC){
	  curnd = mcnd;
	  break;
	}
	mcnd = mcnd->Next();
  }
  if(curnd == NULL){
	delete newmc;
	return NULL;
  }

  wc->MCs.AddBefore(curnd, newmc);

  // loop through all operations to add the new machine to the list of alternative
  // machines whenever an operation uses wc
  //
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == wc){
		SOpMC *newopmc = new SOpMC(newmc);
		curnd = NULL;
		mcnd = op->AltMCs.head();
		while(mcnd){
		  SOpMC *eopmc = (SOpMC *)mcnd->Data();
		  if(eopmc->MC == curMC){
			curnd = mcnd;
			break;
		  }
		  mcnd = mcnd->Next();
		}
		if(curnd) op->AltMCs.AddBefore(curnd, newopmc);
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  return newmc;
}

MOSP_Problem::SMC *MOSP_Problem::InsertMC(int icurMC, char *ID, char *Des, MOSP_Problem::SWC *wc, float rt)
{
  if(icurMC < 0 || icurMC >= wc->MCs.Count()) return NULL;
  return InsertMC((SMC *)(wc->MCs[icurMC]), ID, Des, wc, rt);
}

MOSP_Problem::SJob *MOSP_Problem::AddJob(char *ID, char *Des, float rt,
									 float dd, float w)
{
  if(UsedID(ID)) return NULL;
  SJob *newJob = new SJob(ID, Des, rt, dd, w);
  Jobs.Add(newJob);

  return newJob;
}

MOSP_Problem::SJob *MOSP_Problem::InsertJob(MOSP_Problem::SJob *curJob,
	   char *ID, char *Des, float rt, float dd, float w)
{
  if(UsedID(ID)) return NULL;
  SJob *newJob = new SJob(ID, Des, rt, dd, w);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = Jobs.head();
  while(nd){
	SJob *ejb = (SJob *)nd->Data();
	if(ejb == curJob) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newJob;
	return NULL;
  }

  Jobs.AddBefore(curNode, newJob);

  return newJob;
}

MOSP_Problem::SJob *MOSP_Problem::InsertJob(int icurJob, char *ID, char *Des,
						   float rt, float dd, float w)
{
  if(icurJob < 0 || icurJob >= Jobs.Count()) return NULL;
  return InsertJob((SJob *)(Jobs[icurJob]), ID, Des, rt, dd, w);
}

MOSP_Problem::SOperation *MOSP_Problem::AddOperation(char *ID, char *Des, SJob *j, SWC *p, float *t)
{
  if(UsedID(ID)) return NULL;
  SOperation *newOP;
  newOP = new SOperation(ID, Des, j, p, t);
  j->Operations.Add(newOP);
  return newOP;
}

MOSP_Problem::SOperation *MOSP_Problem::
  InsertOperation(MOSP_Problem::SOperation *curOp, char *ID, char *Des, SJob *j, SWC *p, float *t)
{
  if(UsedID(ID)) return NULL;
  SOperation *newOp = new SOperation(ID, Des, j, p, t);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = j->Operations.head();
  while(nd){
	MOSP_Problem::SOperation *eop = (MOSP_Problem::SOperation *)nd->Data();
	if(eop == curOp) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newOp;
	return NULL;
  }

  j->Operations.AddBefore(curNode, newOp);
  return newOp;
}

MOSP_Problem::SOperation *MOSP_Problem::
  InsertOperation(int icurOp, char *ID, char *Des, SJob *j, SWC *p, float *t)
{
  if(icurOp < 0 || icurOp >= j->Operations.Count()) return NULL;
  return InsertOperation((SOperation *)(j->Operations[icurOp]), ID, Des, j, p, t);
}

bool MOSP_Problem::RemoveMC(SMC *mc)
{
  // first loop through all operations to remove mc from their lists
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == mc->WC){
        SOpMC *opmc;
		SList::SNode *opmcnd = op->AltMCs.head();
		while(opmcnd){
		  opmc = (SOpMC *)opmcnd->Data();
		  if(opmc->MC == mc){
			op->AltMCs.Remove(opmcnd);
			delete opmc;
			break;
		  }
		  opmcnd = opmcnd->Next();
		}
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  mc->WC->MCs.Remove((void *)mc);
  delete mc;
  return true;
}

bool MOSP_Problem::RemoveWC(SWC *wc)
{
  // Check if it is currently used by any operation
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
	SJob *jb = (SJob *)jbnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  SOperation *sop = (SOperation *)opnd->Data();
	  if(sop->WC == wc) return false;
	  opnd = opnd->Next();
	}
	jbnd = jbnd->Next();
  }

  WCs.Remove((void *)wc);
  delete wc;
  return true;
}

bool MOSP_Problem::RemoveJob(SJob *jb)
{
  // first delete all operations related to this job
  SList::SNode *opnd = jb->Operations.head();
  while(opnd){
	SOperation *sop = (SOperation *)opnd->Data();
	delete sop;
    opnd = opnd->Next();
  }
  jb->Operations.Clear();

  Jobs.Remove((void *)jb);
  delete jb;

  return true;
}

bool MOSP_Problem::RemoveOperation(SOperation *op)
{
  SJob *j = op->Job;
  j->Operations.Remove((void *) op);
  delete op;
  return true;
}

bool MOSP_Problem::ModifyWC(SWC *wc, char *id, char *des)
{
  if(UsedIDinWCs(id, wc) || UsedIDinMCs(id) || UsedIDinJOs(id) || UsedIDinOps(id)) return false;
  strncpy(wc->ID(), id, 8);
  strncpy(wc->Description(), des, 64);
  return true;
}

bool MOSP_Problem::ModifyMC(SMC *mc, char *id, char *des, float rt)
{
  if(UsedIDinWCs(id) || UsedIDinMCs(id, mc) || UsedIDinJOs(id) || UsedIDinOps(id)) return false;
  strncpy(mc->ID(), id, 8);
  strncpy(mc->Description(), des, 64);
  mc->ReadyTime = rt;
  return true;
}

bool MOSP_Problem::ModifyJob(SJob *jb, char *id, char *des, float rt,
                                          float dd, float w, float)
{
  if(UsedIDinJOs(id, jb) || UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinOps(id)) return false;
  strncpy(jb->ID(), id, 8);
  strncpy(jb->Description(), des, 64);
  jb->ReleaseTime = rt;
  jb->DueDate = dd;
  jb->Weight = w;
  return true;
}

bool MOSP_Problem::ModifyOperation(MOSP_Problem::SOperation *op, char *id, char *des, SWC* wc, float *t)
{
  if(UsedIDinJOs(id) || UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinOps(id, op)) return false;

  strncpy(op->ID(), id, 8);
  strncpy(op->Description(), des, 64);

  if(op->WC == wc){
	int i=0;
	for(SList::SNode *opmcnd = op->AltMCs.head(); opmcnd; opmcnd = opmcnd->Next()){
	  SOpMC *opmc = (SOpMC *)opmcnd->Data();
	  opmc->pt = t[i++];
	}
  }
  else{
	for(SList::SNode *opmcnd = op->AltMCs.head(); opmcnd; opmcnd = opmcnd->Next()){
	  SOpMC *opmc = (SOpMC *)opmcnd->Data();
	  delete opmc;
	}
	op->AltMCs.Clear();
	int i=0;
	for(SList::SNode *mcnd = wc->MCs.head(); mcnd; mcnd = mcnd->Next()){
      SMC *mc = (SMC *)mcnd->Data();
	  SOpMC *newopmc = new SOpMC(mc, t[i++]);
	  op->AltMCs.Add(newopmc);
	}
	op->WC = wc;
  }

  return true;
}

bool MOSP_Problem::UsedIDinWCs(char *id, SWC *except)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	if(strcmp(id, wc->ID()) == 0 && wc != except) return true;
	nd = nd->Next();
  }
  return false;
}

bool MOSP_Problem::UsedIDinMCs(char *id, SMC *except)
{
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  SMC *mc = (SMC *)mcnd->Data();
	  if(strcmp(id, mc->ID()) == 0 && mc != except) return true;
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
  return false;
}

bool MOSP_Problem::UsedIDinJOs(char *id, SJob *except)
{
  SList::SNode *nd = Jobs.head();
  while(nd){
    SJob *jb = (SJob *)nd->Data();
    if(strcmp(id, jb->ID()) == 0 && jb != except) return true;
    nd = nd->Next();
  }
  return false;
}

bool MOSP_Problem::UsedIDinOps(char *id, SOperation *except)
{
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    SList::SNode *nd = jb->Operations.head();
    while(nd){
      SOperation *op = (SOperation *)nd->Data();
      if(strcmp(id, op->ID()) == 0 && op != except) return true;
      nd = nd->Next();
    }
    jbnd = jbnd->Next();
  }
  return false;
}

bool MOSP_Problem::UsedID(char *id)
{
  return UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinJOs(id) || UsedIDinOps(id);
}

MOSP_Problem::SWC *MOSP_Problem::WCbyID(char *id)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	if(strcmp(id, wc->ID()) == 0) return wc;
	nd = nd->Next();
  }
  return NULL;
}

MOSP_Problem::SMC *MOSP_Problem::MCbyID(char *id)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  SMC *mc = (SMC *)mcnd->Data();
      if(strcmp(id, mc->ID()) == 0) return mc;
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }
  return NULL;
}

MOSP_Problem::SJob *MOSP_Problem::JObyID(char *id)
{
  SList::SNode *nd = Jobs.head();
  while(nd){
    SJob *jb = (SJob *)nd->Data();
    if(strcmp(id, jb->ID()) == 0) return jb;
    nd = nd->Next();
  }
  return NULL;
}

MOSP_Problem::SOperation *MOSP_Problem::OpByID(char *id)
{
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    SList::SNode *nd = jb->Operations.head();
    while(nd){
      SOperation *op = (SOperation *)nd->Data();
      if(strcmp(id, op->ID()) == 0) return op;
      nd = nd->Next();
    }
    jbnd = jbnd->Next();
  }
  return NULL;
}

int MOSP_Problem::nOperations()
{
  int nops = 0;
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    nops += (jb->Operations.Count());
    jbnd = jbnd->Next();
  }
  return nops;
}

int MOSP_Problem::nOperationsByWC(SWC *wc)
{
  int nops = 0;
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    SList::SNode *opnd = jb->Operations.head();
    while(opnd){
      SOperation *op = (SOperation *)opnd->Data();
	  if(op->WC == wc) nops++;
      opnd = opnd->Next();
    }
    jbnd = jbnd->Next();
  }
  return nops;
}

int MOSP_Problem::nMachines()
{
  int nmcs = 0;
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	nmcs += (wc->MCs.Count());
	wcnd = wcnd->Next();
  }
  return nmcs;
}

bool MOSP_Problem::isProportionate()
{
  // A proportionate MOSP means that all jobs needs processing on all workcenters
  // and the processing times on all parallel machines of any workcenter are equal
  //

  // 1. Check if all jobs need processing on all workcenters
  int nWCs = WCs.Count();
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	if(jb->Operations.Count() != nWCs) return false;
	jnd = jnd->Next();
  }

  // 2. Check if each job will have the same processing time on all prallel machines
  //    for all workcenters
  jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  SOperation *op = (SOperation *)opnd->Data();
	  if(op->minpt() != op->maxpt()) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }

  // 3. Check if all jobs will have the same processing time on the same workcenter
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	float pt = -1;
	jnd = Jobs.head();
	while(jnd){
	  SJob *jb = (SJob *)jnd->Data();
	  SList::SNode *opnd = jb->Operations.head();
	  SOperation *gop = NULL;
	  while(opnd){
		SOperation *op = (SOperation *)opnd->Data();
		if(op->WC == wc){
		  gop = op;
		  break;
		}
		opnd = opnd->Next();
	  }
	  if(gop == NULL) return false;
	  if(pt == -1) pt = gop->minpt();
	  if(pt != gop->minpt()) return false;
	  jnd = jnd->Next();
	}
	wcnd = wcnd->Next();
  }

  return true;
}
