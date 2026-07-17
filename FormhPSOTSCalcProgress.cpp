//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormhPSOTSCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormTSCalcProgress"
#pragma resource "*.dfm"
TfrmhPSOTSCalcProgress *frmhPSOTSCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmhPSOTSCalcProgress::TfrmhPSOTSCalcProgress(TComponent* Owner)
	: TfrmTSCalcProgress(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmhPSOTSCalcProgress::FormShow(TObject *Sender)
{
  TfrmTSCalcProgress::FormShow(Sender);
  lblIterTS->Caption = "";
}
//---------------------------------------------------------------------------

