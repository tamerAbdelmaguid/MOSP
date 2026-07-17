//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormGeneralMOSPInstances.h"
#include "Problem.h"
#include "FormProblem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmGeneralMOSPInstances *frmGeneralMOSPInstances;
//---------------------------------------------------------------------------
__fastcall TfrmGeneralMOSPInstances::TfrmGeneralMOSPInstances(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralMOSPInstances::FormShow(TObject *)
{
  cbxNoOfPCs->Enabled = true;
  cbxnJobsTonWCs->Enabled = true;
  cbxqwLevel->Enabled = true;
  cbxLoadingLevel->Enabled = true;
  cbxPTVL->Enabled = true;
  cbxNoOfPCs->ItemIndex = 0;
  cbxnJobsTonWCs->ItemIndex = 0;
  cbxqwLevel->ItemIndex = 0;
  cbxLoadingLevel->ItemIndex = 0;
  cbxPTVL->ItemIndex = 0;
  edtnReplicates->Text = "5";
  rgExperimentType->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralMOSPInstances::btnGenerateClick(TObject *)
{
  AnsiString s;
  AnsiString prbname;

  int nReplicates;
  s = edtnReplicates->Text;
  try {
	nReplicates = StrToInt(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate values for the number of replicates",
							  L"Error reading numbers", MB_OK);
	  return;
	}

  if(rgExperimentType->ItemIndex == 0){  // factorial experiment
	int nPCs, nJobs;
	int qwLevel, LL, PTVL;

	switch(cbxNoOfPCs->ItemIndex){
	  case 0: nPCs = 6;   break;
	  case 1: nPCs = 12;  break;
	  case 2: nPCs = 20;
	}
	switch(cbxnJobsTonWCs->ItemIndex){
	  case 0: nJobs = 2 * nPCs;   break;
	  case 1: nJobs = 4 * nPCs;  break;
	  case 2: nJobs = 6 * nPCs;
	}
	qwLevel = cbxqwLevel->ItemIndex + 1;
	LL = cbxLoadingLevel->ItemIndex + 1;
	PTVL = cbxPTVL->ItemIndex + 1;

	for(int r = 1; r <= nReplicates; r++){
	  prbname = "MOSP-" + IntToStr(nPCs) + "-" + IntToStr(nJobs) + "-" +
				IntToStr(qwLevel) + "-" + IntToStr(LL) + "-" + IntToStr(PTVL) + "-"
				+ IntToStr(r);
	  MOSP_Problem *newProblem = new MOSP_Problem(prbname.c_str(), "MOSP problem",
									   nPCs, nJobs, qwLevel, LL, PTVL);
	  TfrmProblem *Child = new TfrmProblem(Application, newProblem, prbname);
	  Child->Caption = prbname;
	}
  }
  else{  // Taguchi experiment
	for(int r = 1; r <= nReplicates; r++){
	  prbname = "MOSP-T-" + IntToStr(r);
	  MOSP_Problem *newProblem = new MOSP_Problem(prbname.c_str(), "MOSP problem",
									   10, 30, 0, 0, 0);
	  TfrmProblem *Child = new TfrmProblem(Application, newProblem, prbname);
	  Child->Caption = prbname;
	}
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralMOSPInstances::rgExperimentTypeClick(TObject *)
{

  switch(rgExperimentType->ItemIndex){
	case 0:
			cbxNoOfPCs->Enabled = true;
			cbxnJobsTonWCs->Enabled = true;
			cbxqwLevel->Enabled = true;
			cbxLoadingLevel->Enabled = true;
			cbxPTVL->Enabled = true;
			cbxNoOfPCs->ItemIndex = 0;
			cbxnJobsTonWCs->ItemIndex = 0;
			cbxqwLevel->ItemIndex = 0;
			cbxLoadingLevel->ItemIndex = 0;
			cbxPTVL->ItemIndex = 0;
			edtnReplicates->Text = "5";
			break;

	case 1:
			cbxNoOfPCs->ItemIndex = -1;
			cbxnJobsTonWCs->ItemIndex = -1;
			cbxqwLevel->ItemIndex = -1;
			cbxLoadingLevel->ItemIndex = -1;
			cbxPTVL->ItemIndex = -1;
			cbxNoOfPCs->Enabled = false;
			cbxnJobsTonWCs->Enabled = false;
			cbxqwLevel->Enabled = false;
			cbxLoadingLevel->Enabled = false;
			cbxPTVL->Enabled = false;
			edtnReplicates->Text = "30";
  }

}
//---------------------------------------------------------------------------

