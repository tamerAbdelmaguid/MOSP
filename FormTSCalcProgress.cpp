//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormTSCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormCalcProgress"
#pragma resource "*.dfm"
TfrmTSCalcProgress *frmTSCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmTSCalcProgress::TfrmTSCalcProgress(TComponent* Owner)
	: TfrmCalcProgress(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmTSCalcProgress::FormShow(TObject *)
{
  lblLB->Caption = "";
  lblInitialCmax->Caption = "";
  lblCurrentCmax->Caption = "";
  lblBestCmax->Caption = "";
  lblIter->Caption = "";
  lblImpIter->Caption = "";
  ProgressBar->Position = 0;
}
//---------------------------------------------------------------------------


