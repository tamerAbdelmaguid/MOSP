//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormDissimilarityMeasure.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDissimilarityMeasure *frmDissimilarityMeasure;
//---------------------------------------------------------------------------
__fastcall TfrmDissimilarityMeasure::TfrmDissimilarityMeasure(TComponent* Owner)
	: TForm(Owner)
{
  PSI1 = NULL;
  DSG1 = NULL;
  VPS1 = NULL;
  VOO1 = NULL;
  LoadedSol1 = NULL;

  PSI2 = NULL;
  DSG2 = NULL;
  VPS2 = NULL;
  VOO2 = NULL;
  LoadedSol2 = NULL;

  NSTester1 = NULL;
  NSTester1 = NULL;
  Vec1 = NULL;
  Vec2 = NULL;

  SSPR_Sol1 = NULL;
  SSPR_Sol2 = NULL;
  SSPR_Alg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::FormShow(TObject *Sender)
{
  frmSol1 = new TfrmSolution(this, Problem);
  frmSol1->pnlOFs->Visible = false;
  frmSol1->pgSolution->TabIndex = 0;
  frmSol1->Parent = pnlGantt1;
  frmSol1->Show();
  frmSol2 = new TfrmSolution(this, Problem);
  frmSol2->pnlOFs->Visible = false;
  frmSol2->pgSolution->TabIndex = 0;
  frmSol2->Parent = pnlGantt2;
  frmSol2->Show();

  frmSol1->btnLoadSol->Visible = false;
  frmSol1->btnSaveSol->Visible = false;

  grdPS1->RowCount = Problem->nMachines();
  grdPS1->ColCount = 2;

  grdOO1->RowCount = Problem->Jobs.Count();
  grdOO1->ColCount = 2;

  frmSol2->btnLoadSol->Visible = false;
  frmSol2->btnSaveSol->Visible = false;

  grdPS2->RowCount = Problem->nMachines();
  grdPS2->ColCount = 2;

  grdOO2->RowCount = Problem->Jobs.Count();
  grdOO2->ColCount = 2;

  int maxops = 1;
  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	if(maxops < Problem->nOperationsByWC(pc)) maxops = Problem->nOperationsByWC(pc);
	pcnd = pcnd->Next();
  }
  grdPS1->ColCount = maxops + 1;
  grdPS2->ColCount = maxops + 1;

  maxops = 1;
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jnd->Data();
	if(maxops < jb->Operations.Count()) maxops = jb->Operations.Count();
	jnd = jnd->Next();
  }
  grdOO1->ColCount = maxops + 1;
  grdOO2->ColCount = maxops + 1;

  btnRunDSG1Click(Sender);
  btnRunDSG2Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::FormClose(TObject *, TCloseAction &)

{
  frmSol1->Close();     delete frmSol1;
  frmSol2->Close();     delete frmSol2;

  if(PSI1) delete PSI1;   PSI1 = NULL;
  if(DSG1) delete DSG1;   DSG1 = NULL;
  if(VPS1) delete VPS1;   VPS1 = NULL;
  if(VOO1) delete VOO1;   VOO1 = NULL;
  if(LoadedSol1) delete LoadedSol1;       LoadedSol1 = NULL;

  if(PSI2) delete PSI2;   PSI2 = NULL;
  if(DSG2) delete DSG2;   DSG2 = NULL;
  if(VPS2) delete VPS2;   VPS2 = NULL;
  if(VOO2) delete VOO2;   VOO2 = NULL;
  if(LoadedSol2) delete LoadedSol2;       LoadedSol2 = NULL;

  if(NSTester1){ NSTester1->FreeMemory(); delete NSTester1; }    NSTester1 = NULL;
  if(NSTester2){ NSTester2->FreeMemory(); delete NSTester2; }    NSTester2 = NULL;
  if(Vec1) delete Vec1;   Vec1 = NULL;
  if(Vec2) delete Vec2;   Vec2 = NULL;

  if(SSPR_Sol1) delete SSPR_Sol1;     SSPR_Sol1 = NULL;
  if(SSPR_Sol2) delete SSPR_Sol2;     SSPR_Sol2 = NULL;
  if(SSPR_Alg){  SSPR_Alg->FreeMemory();  delete SSPR_Alg; }     SSPR_Alg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnRunDSG1Click(TObject *)
{
  if(DSG1) delete DSG1;
  DSG1 = new MOSP_DSG(Problem, MOSP_DSG::RANDOM, MOSP_DSG::MCRANDOM);
  DSG1->Run();
  UpdateVPS1(DSG1->Solution);
  UpdateVOO1(DSG1->Solution);
  frmSol1->SetCurrentSolution(DSG1->Solution);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnRunDSG2Click(TObject *)
{
  if(DSG2) delete DSG2;
  DSG2 = new MOSP_DSG(Problem, MOSP_DSG::RANDOM, MOSP_DSG::MCRANDOM);
  DSG2->Run();
  UpdateVPS2(DSG2->Solution);
  UpdateVOO2(DSG2->Solution);
  frmSol2->SetCurrentSolution(DSG2->Solution);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnLoad1Click(TObject *)
{
  frmSol1->btnLoadSolClick(this);
  UpdateVPS1(frmSol1->CurrentSol);
  UpdateVOO1(frmSol1->CurrentSol);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnSave1Click(TObject *)
{
  frmSol1->btnSaveSolClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnLoad2Click(TObject *)
{
  frmSol2->btnLoadSolClick(this);
  UpdateVPS2(frmSol2->CurrentSol);
  UpdateVOO2(frmSol2->CurrentSol);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnSave2Click(TObject *)
{
  frmSol2->btnSaveSolClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::DisplayVPS(TStringGrid *grdPS, MOSP_VPS *vps)
{
  int r=0, c;
  SList::SNode *psnd = vps->PSs.tail();
  while(psnd){
	MOSP_PS *ps = (MOSP_PS *)psnd->Data();
	SList::SNode *opnd = ps->PS.head();
	grdPS->Cells[0][r] = ps->MC->ID();
	c = 1;
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  grdPS->Cells[c][r] = op->ID();
	  opnd = opnd->Next();
	  c++;
	}
	while(c < grdPS->ColCount){
	  grdPS->Cells[c][r] = "";
	  c++;
	}
	psnd = psnd->Prev();
	r++;
  }
  grdPS->Row = r-1;
  grdPS->Col = 1;

  lblJDissmM->Caption = "";
  lblWDissmM->Caption = "";
  lblDistance->Caption = "";

  lblJDissmM_Apr->Caption = "";
  lblWDissmM_Apr->Caption = "";
  lblDistance_Apr->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::UpdateVPS1(MOSP_Solution *Sol)
{
  if(VPS1) delete VPS1;
  VPS1 = new MOSP_VPS(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VPS1->AppendOp(tsk->SelectedMC, tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVPS(grdPS1, VPS1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::UpdateVPS2(MOSP_Solution *Sol)
{
  if(VPS2) delete VPS2;
  VPS2 = new MOSP_VPS(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VPS2->AppendOp(tsk->SelectedMC, tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVPS(grdPS2, VPS2);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmDissimilarityMeasure::ReadVPS(TStringGrid *grdPS, int vpsno)
{
  AnsiString s;
  int r, c;

  MOSP_VPS *vps = new MOSP_VPS(Problem);

  for(r=0; r<grdPS->RowCount; r++){
	s = grdPS->Cells[0][r];
	MOSP_Problem::SMC *mc = Problem->MCbyID(s.c_str());
	if(!mc){
	  String msg = "Unknown machine " + s;
	  Application->MessageBox(msg.c_str(), L"Error reading machine", MB_OK);
	  delete vps;
	  return false;
	}

	for(c=1; c<grdPS->ColCount; c++){
	  s = grdPS->Cells[c][r];
      if(s == "") break;
	  MOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(!op){
		String msg = "Unknown operation "+s+" in machines processing sequences";
		Application->MessageBox(msg.c_str(), L"Error reading operation", MB_OK);
		delete vps;
		return false;
	  }
	  vps->AppendOp(mc, op);
	}
  }

  if(!vps->isComplete()){
	Application->MessageBox(L"Incomplete vector of processing sequences",
							L"Invalid processing sequences",
							MB_OK);
	delete vps;
	return false;
  }

  if(vpsno == 1){
	if(VPS1) delete VPS1;
	VPS1 = vps;
  }
  else{
	if(VPS2) delete VPS2;
	VPS2 = vps;
  }

  return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::DisplayVOO(TStringGrid *grdOO, MOSP_VOO *voo)
{
  int r=0, c;
  SList::SNode *oond = voo->OOs.head();
  while(oond){
	MOSP_OO *oo = (MOSP_OO *)oond->Data();
	SList::SNode *opnd = oo->OO.head();
	grdOO->Cells[0][r] = oo->job->ID();
	c = 1;
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  grdOO->Cells[c][r] = op->ID();
	  opnd = opnd->Next();
	  c++;
	}
	while(c < grdOO->ColCount){
	  grdOO->Cells[c][r] = "";
	  c++;
	}
	oond = oond->Next();
	r++;
  }
  grdOO->Row = 0;
  grdOO->Col = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::UpdateVOO1(MOSP_Solution *Sol)
{
  if(VOO1) delete VOO1;
  VOO1 = new MOSP_VOO(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VOO1->AppendOp(tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVOO(grdOO1, VOO1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::UpdateVOO2(MOSP_Solution *Sol)
{
  if(VOO2) delete VOO2;
  VOO2 = new MOSP_VOO(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VOO2->AppendOp(tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVOO(grdOO2, VOO2);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmDissimilarityMeasure::ReadVOO(TStringGrid *grdOO, int voono)
{
  AnsiString s;
  int r, c;

  MOSP_VOO *voo = new MOSP_VOO(Problem);

  for(r=0; r<grdOO->RowCount; r++){
	s = grdOO->Cells[0][r];
	MOSP_Problem::SJob *jb = Problem->JObyID(s.c_str());
	if(!jb){
	  String msg = "Unknown job "+s;
	  Application->MessageBox(msg.c_str(), L"Error reading job", MB_OK);
	  delete voo;
	  return false;
	}

	for(c=1; c<grdOO->ColCount; c++){
	  s = grdOO->Cells[c][r];
      if(s == "") break;
	  MOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(!op){
		String msg = "Unknown operation "+s+" in operation\'s job order";
		Application->MessageBox(msg.c_str(), L"Error reading operation", MB_OK);
		delete voo;
		return false;
	  }
	  voo->AppendOp(op);
	}
  }

  if(!voo->isComplete()){
	Application->MessageBox(L"Incomplete operations\' job orders",
							L"Invalid job orders",
							MB_OK);
	delete voo;
	return false;
  }

  if(voono == 1){
	if(VOO1) delete VOO1;
	VOO1 = voo;
  }
  else{
	if(VOO2) delete VOO2;
	VOO2 = voo;
  }

  return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::btnRunPSI1Click(TObject *)
{
  if(ReadVPS(grdPS1, 1) && ReadVOO(grdOO1, 1)){
	if(InterpretVectors1()){
	  DisplayVPS(grdPS1, VPS1);
	  DisplayVOO(grdOO1, VOO1);
	  frmSol1->SetCurrentSolution(PSI1->Solution);
	}
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmDissimilarityMeasure::btnRunPSI2Click(TObject *)
{
  if(ReadVPS(grdPS2, 2) && ReadVOO(grdOO2, 2)){
	if(InterpretVectors2()){
	  DisplayVPS(grdPS2, VPS2);
	  DisplayVOO(grdOO2, VOO2);
	  frmSol2->SetCurrentSolution(PSI2->Solution);
	}
  }
}
//---------------------------------------------------------------------------
bool __fastcall TfrmDissimilarityMeasure::InterpretVectors1()
{
  if(PSI1) delete PSI1;
  PSI1 = new MOSP_PSI(Problem, VPS1, VOO1);
  PSI1->Run();
  if(PSI1->SolStatus == MOSP_Algorithm::INFEASIBLE){
	Application->MessageBox(L"The represented processing sequences and operations\' orders results in "\
							"an infeasible solution",
							L"Infeasible solution!!",
							MB_OK);
	return false;
  }
  return true;
}

bool __fastcall TfrmDissimilarityMeasure::InterpretVectors2()
{
  if(PSI2) delete PSI2;
  PSI2 = new MOSP_PSI(Problem, VPS2, VOO2);
  PSI2->Run();
  if(PSI2->SolStatus == MOSP_Algorithm::INFEASIBLE){
	Application->MessageBox(L"The represented processing sequences and operations\' orders results in "\
							"an infeasible solution",
							L"Infeasible solution!!",
							MB_OK);
	return false;
  }
  return true;
}
void __fastcall TfrmDissimilarityMeasure::btnDissmMClick(TObject *)
{
  if(NSTester1){ NSTester1->FreeMemory(); delete NSTester1; }
  NSTester1 = new MOSP_NSTester(Problem, frmSol1->CurrentSol);
  NSTester1->AllocateMemory();
  NSTester1->Initialize();

  if(Vec1) delete Vec1;
  Vec1 = new MOSP_VectorSol(NSTester1);
  Vec1->ConvertTasksToVect();

  if(NSTester2){ NSTester2->FreeMemory(); delete NSTester2; }
  NSTester2 = new MOSP_NSTester(Problem, frmSol2->CurrentSol);
  NSTester2->AllocateMemory();
  NSTester2->Initialize();

  if(Vec2) delete Vec2;
  Vec2 = new MOSP_VectorSol(NSTester2);
  Vec2->ConvertTasksToVect();

  int jd = Vec1->Psi_j(Vec2);
  int wd = Vec1->Psi_w(Vec2);
  lblJDissmM->Caption = IntToStr(jd);
  lblWDissmM->Caption = IntToStr(wd);

  lblDistance->Caption = IntToStr(jd+wd);
}
//---------------------------------------------------------------------------

void __fastcall TfrmDissimilarityMeasure::btnAprDistanceClick(TObject *)
{
  if(SSPR_Alg){ SSPR_Alg->FreeMemory();  delete SSPR_Alg;  }
  SSPR_Alg = new MOSP_SSPR(Problem);
  SSPR_Alg->AllocateMemory();

  if(SSPR_Sol1) delete SSPR_Sol1;
  SSPR_Sol1 = new MOSP_SSPR_Solution(SSPR_Alg);
  if(!SSPR_Sol1->InterpretMOSPSolution(frmSol1->CurrentSol)) return;

  if(SSPR_Sol2) delete SSPR_Sol2;
  SSPR_Sol2 = new MOSP_SSPR_Solution(SSPR_Alg);
  if(!SSPR_Sol2->InterpretMOSPSolution(frmSol2->CurrentSol)) return;

  int jDistance, wDistance;
  int D = SSPR_Alg->Distance(SSPR_Sol1, SSPR_Sol2, jDistance, wDistance);

  lblJDissmM_Apr->Caption = IntToStr(jDistance);
  lblWDissmM_Apr->Caption = IntToStr(wDistance);
  lblDistance_Apr->Caption = IntToStr(D);
}
//---------------------------------------------------------------------------

