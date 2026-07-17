//---------------------------------------------------------------------------

#ifndef FormTSCalcProgressH
#define FormTSCalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormCalcProgress.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmTSCalcProgress : public TfrmCalcProgress
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TProgressBar *ProgressBar;
	TLabel *lblCurrentCmax;
	TLabel *lblBestCmax;
	TLabel *lblIter;
	TLabel *Label4;
	TLabel *lblInitialCmax;
	TLabel *Label5;
	TLabel *lblLB;
	TLabel *Label6;
	TLabel *lblImpIter;
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmTSCalcProgress(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTSCalcProgress *frmTSCalcProgress;
//---------------------------------------------------------------------------
#endif
