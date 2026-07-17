//---------------------------------------------------------------------------

#ifndef FormNetEditorH
#define FormNetEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormPSEditor.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TfrmNetEditor : public TfrmPSEditor
{
__published:	// IDE-managed Components
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmNetEditor(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmNetEditor *frmNetEditor;
//---------------------------------------------------------------------------
#endif
