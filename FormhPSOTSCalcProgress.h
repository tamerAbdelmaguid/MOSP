//---------------------------------------------------------------------------

#ifndef FormhPSOTSCalcProgressH
#define FormhPSOTSCalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormTSCalcProgress.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmhPSOTSCalcProgress : public TfrmTSCalcProgress
{
__published:	// IDE-managed Components
	TLabel *Label7;
	TLabel *lblIterTS;
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmhPSOTSCalcProgress(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmhPSOTSCalcProgress *frmhPSOTSCalcProgress;
//---------------------------------------------------------------------------
#endif
