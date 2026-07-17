//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSol.h"
#include <string.h>
#include "Main.h"
#include <io.h>
#include "FileSystem.h"
#include "random.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSolution *frmSolution;

TColor JobColors[100];
TColor FixedJobColors [] = {
  clYellow, clLtGray, clLime, clGreen, clGray, clFuchsia, clNavy, clMaroon,
  clAqua, clTeal, clRed, clPurple, clOlive, clBlue, clBlack
};

//---------------------------------------------------------------------------
__fastcall TfrmSolution::TfrmSolution(TComponent* Owner, MOSP_Problem *P)
        : TForm(Owner), Problem(P)
{
  CurrentSol = NULL;
  LoadedSol = NULL;
  nwAlg = NULL;
  Net = NULL;

  unsigned int i;
  for(i=0; i<sizeof(FixedJobColors)/sizeof(TColor); i++)
    JobColors[i] = FixedJobColors[i];
  randomize();
  warmup_random(random(1000)/1000.);
  for(; i<100; i++)
    JobColors[i] = (TColor)RGB(rnd(0,15)*15, rnd(0,15)*15, rnd(0,15)*15);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::DrawGantt()
{
  if(!CurrentSol) return;

  Gantt->Clear();

  // Draw Gantt chart for the MCs
  //
  int i, r = 0;
  String s;
  SList::SNode *pcnd = CurrentSol->Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	SList::SNode *mcnd = pc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  //s = pc->ID();
	  s = /*s + "." + */ mc->ID();
	  Gantt->AddGantt(0, 0, r, s);
	  r++;
	  mcnd = mcnd->Next();
	}
	pcnd = pcnd->Next();
  }

  TColor c;
  SList::SNode *prnd = CurrentSol->ScheduledTasks.head();
  while(prnd){
	MOSP_Solution::SchTask *pr = (MOSP_Solution::SchTask *)prnd->Data();
	if(pr->SelectedMC){
	  r = pr->SelectedMC->i;
	  i = pr->Operation->Job->i;
	  s = pr->SelectedMC->ID();
	  c = (i<100)? JobColors[i] : (TColor)RGB(rnd(0,255), rnd(0,255), rnd(0,255));
	  Gantt->AddGanttColor(pr->StartTime, pr->EndTime, r, s, c);
	}
	prnd = prnd->Next();
  }

  // Draw critical paths
  CPaths->Clear();
  int x, y1, y2;
  for(int i=0; i<nwAlg->nOps; i++){
	if(Net->Nodes[i].bCritical){
	  x = Net->Nodes[i].mc;
	  y1 = Net->Nodes[i].estart;
	  y2 = y1 + Net->Nodes[i].weight;
	  CPaths->AddArrow(y1, x, y2, x, "", clRed);

	  int prv = Net->Nodes[i].j_prv;
	  if(prv != -1)
		if(Net->Nodes[prv].bCritical){
		  y2 = Net->Nodes[prv].mc;
		  CPaths->AddArrow(y1, y2, y1, x, "", clRed);
	    }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::FillTables()
{
  if(!CurrentSol) return;

  String s;

  lstMachines->Clear();
  SList::SNode *pcnd = CurrentSol->Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	SList::SNode *mcnd = pc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  //s = pc->ID();
	  s = /*s + "." +*/ mc->ID();
	  lstMachines->Items->Add(s);
	  mcnd = mcnd->Next();
	}
	pcnd = pcnd->Next();
  }
  lstMachines->ItemIndex = 0;
  lstMachinesClick(NULL);

  lstJobs->Clear();
  SList::SNode *jbnd = CurrentSol->Problem->Jobs.head();
  while(jbnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jbnd->Data();
	lstJobs->Items->Add(jb->ID());
    jbnd = jbnd->Next();
  }
  lstJobs->ItemIndex = 0;
  lstJobsClick(NULL);

  FillNetworkTables();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::DisplayMeasures()
{
  if(!CurrentSol) return;

  lblMakespan->Caption = Makespan;
  lblMeanFlowTime->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_FLOWTIME);
  lblMeanLateness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_LATENESS);
  lblMeanTardiness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_TARDINESS);
  lblMaxLateness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MAX_LATENESS);
  lblMaxTardiness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MAX_TARDINESS);
  lblNTardyJobs->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_N_TARDY);
  lblMWLateness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_W_LATENESS);
  lblMWTardiness->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_W_TARDINESS);
  lblMWFlowTime->Caption = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MEAN_W_FLOWTIME);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::FormShow(TObject *)
{
  lblMakespan->Caption = "";
  lblMeanFlowTime->Caption = "";
  lblMeanLateness->Caption = "";
  lblMeanTardiness->Caption = "";
  lblMaxLateness->Caption = "";
  lblMaxTardiness->Caption = "";
  lblNTardyJobs->Caption = "";
  lblMWLateness->Caption = "";
  lblMWTardiness->Caption = "";
  lblMWFlowTime->Caption = "";
  grdMachineOps->Cells[0][0] = "Operation ID";
  grdMachineOps->Cells[1][0] = "Job ID";
  grdMachineOps->Cells[2][0] = "Start time";
  grdMachineOps->Cells[3][0] = "Finish time";
  grdJobOps->Cells[0][0] = "Operation ID";
  grdJobOps->Cells[1][0] = "Machine ID";
  grdJobOps->Cells[2][0] = "Start time";
  grdJobOps->Cells[3][0] = "Finish time";
  grdJobs->Cells[0][0] = "Job ID";
  grdJobs->Cells[1][0] = "First";
  grdJobs->Cells[2][0] = "Last";
  grdMachines->Cells[0][0] = "M/C ID";
  grdMachines->Cells[1][0] = "First";
  grdMachines->Cells[2][0] = "Last";
  grdNetwork->Cells[0][0] = "No.";
  grdNetwork->Cells[1][0] = "ID";
  grdNetwork->Cells[2][0] = "j_prv";
  grdNetwork->Cells[3][0] = "j_nxt";
  grdNetwork->Cells[4][0] = "M/C";
  grdNetwork->Cells[5][0] = "m_prv";
  grdNetwork->Cells[6][0] = "m_nxt";
  grdNetwork->Cells[7][0] = "weight";
  grdNetwork->Cells[8][0] = "estart";
  grdNetwork->Cells[9][0] = "lstart";
  grdNetwork->Cells[10][0] = "tail";
  grdNetwork->Cells[11][0] = "Critical";

  btnSaveSol->Enabled = false;
  LoadedSol = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::SetCurrentSolution(MOSP_Solution *Sol, MOSP_Network *nt)
{
  if(!Sol) return;

  CurrentSol = Sol;
  Makespan = CurrentSol->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN);

  if(!nt){
	if(nwAlg){ nwAlg->FreeMemory();  delete nwAlg; }
	nwAlg = new MOSP_NSTester(CurrentSol->Problem, CurrentSol);
	nwAlg->AllocateMemory();
	nwAlg->Initialize();

	if(Net) delete Net;
	Net = new MOSP_Network(nwAlg);
	Net->ConvertTasksToNet();
  }
  else Net = new MOSP_Network(nt);

  DrawGantt();
  FillTables();
  DisplayMeasures();
  btnSaveSol->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::lstMachinesClick(TObject *)
{
  if(!CurrentSol) return;

  MOSP_Problem::SMC *SelMC = NULL;

  SList::SNode *pcnd = CurrentSol->Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	SList::SNode *mcnd = pc->MCs.head();
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  if(mc->i == lstMachines->ItemIndex){
		SelMC = mc;
		break;
	  }
	  mcnd = mcnd->Next();
	}
    if(SelMC) break;
	pcnd = pcnd->Next();
  }

  int n=0;
  SList::SNode *prnd = CurrentSol->ScheduledTasks.head();
  while(prnd){
    MOSP_Solution::SchTask *pr = (MOSP_Solution::SchTask *)prnd->Data();
    if(pr->SelectedMC == SelMC) n++;
	prnd = prnd->Next();
  }
  n++;

  if(n > 1){
	grdMachineOps->RowCount = n;

	int r = 1;
	SList::SNode *tsknd = CurrentSol->ScheduledTasks.head();
	while(tsknd){
	  MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	  if(tsk->SelectedMC == SelMC){
		grdMachineOps->Cells[0][r] = tsk->Operation->ID();
		grdMachineOps->Cells[1][r] = tsk->Operation->Job->ID();
		grdMachineOps->Cells[2][r] = FloatToStr(tsk->StartTime);
		grdMachineOps->Cells[3][r] = FloatToStr(tsk->EndTime);
		r++;
	  }
	  tsknd = tsknd->Next();
	}
  }
  else {
	grdMachineOps->RowCount = 2;
	grdMachineOps->Cells[0][1] = "";
	grdMachineOps->Cells[1][1] = "";
	grdMachineOps->Cells[2][1] = "";
	grdMachineOps->Cells[3][1] = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::lstJobsClick(TObject *)
{
  if(!CurrentSol) return;

  MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)
							(CurrentSol->Problem->Jobs[lstJobs->ItemIndex]);

  int n = jb->Operations.Count() + 1;
  grdJobOps->RowCount = ((n == 1)? 2 : n);

  int r=1;
  SList::SNode *tsknd = CurrentSol->ScheduledTasks.head();
  while(tsknd){
	MOSP_Solution::SchTask *tsk = (MOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->Operation->Job == jb){
	  grdJobOps->Cells[0][r] = tsk->Operation->ID();
	  if(tsk->SelectedMC) grdJobOps->Cells[1][r] = tsk->SelectedMC->ID();
	  else grdJobOps->Cells[1][r] = "N/A";
	  grdJobOps->Cells[2][r] = FloatToStr(tsk->StartTime);
	  grdJobOps->Cells[3][r] = FloatToStr(tsk->EndTime);
	  r++;
	}
	tsknd = tsknd->Next();
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::btnSaveSolClick(TObject *)
{
  if(!CurrentSol) return;
  String msg;
  AnsiString s = MainForm->CurrentChild->Caption;
  int i = s.AnsiPos(".mosp")-1;
  if(i != -1) s = s.SubString(1, i) + ".mosp.sol";
  else s = s + ".mosp.sol";
  dlgSaveSol->FileName = s;
  if(dlgSaveSol->Execute()){
	s = dlgSaveSol->FileName;
	if(access(s.c_str(), 0) == 0){
	  msg = "The file "+s+" already exists, would you like to replace it?";
	  int r = Application->MessageBox((const wchar_t *)msg.c_str(), L"File exists", MB_YESNO);
	  if(r == IDNO) return;
	}
	MOSP_FIO_RESULT Result;
	SaveSolution(Problem, CurrentSol, s.c_str(), Result);
	if(Result != SUCCESS){
	  msg = s;
	  Application->MessageBox((const wchar_t *)msg.c_str(), L"Error writing file", MB_OK);
      return;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::btnLoadSolClick(TObject *)
{
  dlgLoadSol->InitialDir = MainForm->WorkingDirectory;
  bSuccessfulSolLoading = false;
  if (dlgLoadSol->Execute()){
	AnsiString s = dlgLoadSol->FileName;
	String msg;
	MOSP_FIO_RESULT Result;
	MOSP_Solution *newSol = LoadSolution(Problem, (char *)s.c_str(), Result);
	if(Result != SUCCESS){
	  switch(Result){
		case FAIL_TO_OPEN:
		  msg = "failed to open file";
		  break;
		case UNKNOWN_FILE_TYPE:
		  msg = "unknown file type!";
		  break;
		case DATA_ERROR:
		  msg = "Error in data";
		  break;
		case INCOMPATIBLE_PROBLEM:
		  msg = "File contains solution of a problem with different structure!";
		  break;
	  }
	  Application->MessageBox((const wchar_t *)msg.c_str(), L"Error reading file", MB_OK);
	  return;
	}
	if(newSol->isFeasible()){
	  if(LoadedSol) delete LoadedSol;
	  LoadedSol = newSol;
	  SetCurrentSolution(LoadedSol);
      bSuccessfulSolLoading = true;
	  if(!newSol->isComplete()){
        Application->MessageBox(L"Loaded solution is incomplete", L"Incomplete solution", MB_OK);
	  }
	}
	else{
	  Application->MessageBox(L"File contains an infeasible solution", L"Infeasible solution", MB_OK);
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::FormClose(TObject *,
	  TCloseAction &)
{
  if(LoadedSol) delete LoadedSol;     LoadedSol = NULL;
  if(nwAlg){ nwAlg->FreeMemory();  delete nwAlg; }     nwAlg = NULL;
  if(Net) delete Net;   Net = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::ExportWMF1Click(TObject *)
{
  AnsiString s = MainForm->CurrentChild->Caption;
  int i = s.AnsiPos(".mosp")-1;
  if(i != -1) s = s.SubString(1, i) + ".wmf";
  else s = s + ".wmf";
  dlgSaveWMF->FileName = s;
  if(dlgSaveWMF->Execute()){
	s = dlgSaveWMF->FileName;
	chrtJSSPGantt->SaveToMetafileEnh(s);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolution::FillNetworkTables()
{
  if(!CurrentSol) return;

  grdJobs->RowCount = nwAlg->nJobs + 1;
  grdMachines->RowCount = nwAlg->nMCs + 1;
  grdNetwork->RowCount = nwAlg->nOps + 1;

  for(int j=0; j<nwAlg->nJobs; j++){
	grdJobs->Cells[0][j+1] = nwAlg->J[j]->ID();
	grdJobs->Cells[1][j+1] = IntToStr(Net->FirstjNode[j]);
	grdJobs->Cells[2][j+1] = IntToStr(Net->LastjNode[j]);
  }
  for(int m=0; m<nwAlg->nMCs; m++){
	grdMachines->Cells[0][m+1] = nwAlg->M[m]->ID();
	grdMachines->Cells[1][m+1] = IntToStr(Net->FirstmNode[m]);
	grdMachines->Cells[2][m+1] = IntToStr(Net->LastmNode[m]);
  }
  for(int i=0; i<nwAlg->nOps; i++){
	grdNetwork->Cells[0][i+1] = IntToStr(i);
	grdNetwork->Cells[1][i+1] = nwAlg->O[i]->ID();
	grdNetwork->Cells[2][i+1] = IntToStr(Net->Nodes[i].j_prv);
	grdNetwork->Cells[3][i+1] = IntToStr(Net->Nodes[i].j_nxt);
	grdNetwork->Cells[4][i+1] = (Net->Nodes[i].mc != -1)? nwAlg->M[Net->Nodes[i].mc]->ID() : "N/A";
	grdNetwork->Cells[5][i+1] = IntToStr(Net->Nodes[i].m_prv);
	grdNetwork->Cells[6][i+1] = IntToStr(Net->Nodes[i].m_nxt);
	grdNetwork->Cells[7][i+1] = IntToStr(Net->Nodes[i].weight);
	grdNetwork->Cells[8][i+1] = IntToStr(Net->Nodes[i].estart);
	grdNetwork->Cells[9][i+1] = IntToStr(Net->Nodes[i].lstart);
	grdNetwork->Cells[10][i+1] = IntToStr(Net->Nodes[i].tail);
	grdNetwork->Cells[11][i+1] = (Net->Nodes[i].bCritical)? "*" : "";
  }
}


