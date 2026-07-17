//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormAlgorithm.h"
#include "FormSol.h"
#include "MOSPSolver.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmAlgorithm *frmAlgorithm;
//---------------------------------------------------------------------------
__fastcall TfrmAlgorithm::TfrmAlgorithm(TComponent* Owner)
        : TForm(Owner)
{
  frmSolution = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmAlgorithm::FormShow(TObject *)
{
  lblTime->Caption = "";
  if(frmSolution){ delete frmSolution;  frmSolution=NULL; }
  frmSolution = new TfrmSolution(this, Problem);
  frmSolution->Parent = pnlSolution;
  frmSolution->pgSolution->TabIndex = 0;
  frmSolution->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAlgorithm::btnCloseClick(TObject *)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAlgorithm::FormClose(TObject *,
	  TCloseAction &)
{
  frmSolution->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAlgorithm::DisplaySolution(MOSP_Solution *Sol)
{
  frmSolution->SetCurrentSolution(Sol);
}
//---------------------------------------------------------------------------

