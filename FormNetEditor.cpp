//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormNetEditor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormPSEditor"
#pragma resource "*.dfm"
TfrmNetEditor *frmNetEditor;
//---------------------------------------------------------------------------
__fastcall TfrmNetEditor::TfrmNetEditor(TComponent* Owner)
	: TfrmPSEditor(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmNetEditor::FormShow(TObject *Sender)
{
  TfrmPSEditor::FormShow(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfrmNetEditor::btnRunClick(TObject *)
{
//  Write here your own implementation of processing sequences interpretations
// to networks and solutions...
}
//---------------------------------------------------------------------------
