//---------------------------------------------------------------------------

#ifndef FormSSPRMultiRunsH
#define FormSSPRMultiRunsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "Problem.h"
#include "SSPR.h"

//---------------------------------------------------------------------------
class TfrmSSPRMultiRuns : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label13;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TEdit *edtPopSize;
	TEdit *edtDeltaThreshold;
	TEdit *edtTabuSize;
	TEdit *edtnTSIterations;
	TEdit *edtnImpIterations;
	TEdit *edtTSnBestMoves;
	TEdit *edtTSnRandomMoves;
	TEdit *edtnSSPRIterations;
	TEdit *edtEpsilon;
	TStringGrid *grdRuns;
	TRadioGroup *rgNRuns;
	TBitBtn *btnClose;
	TBitBtn *btnRun;
	TLabel *lblLB;
	TLabel *Label5;
	TLabel *Label10;
	TLabel *lblbestCmax;
	TProgressBar *pbSSPRIterations;
	TLabel *Label15;
	TLabel *Label9;
	TProgressBar *pbConstruction;
	TProgressBar *pbRun;
	TLabel *Label2;
	TSpeedButton *btnTerminate;
	TLabel *Label1;
	TLabel *Label3;
	TMemo *memoMakespan;
	TMemo *memoTime;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnTerminateClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations
	MOSP_SSPR *ssprAlg;
    void __fastcall SetDefaults();
	void __fastcall TaguchiRuns();

public:		// User declarations
    bool bActive;

	MOSP_Problem *Problem;

	__fastcall TfrmSSPRMultiRuns(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSSPRMultiRuns *frmSSPRMultiRuns;
//---------------------------------------------------------------------------
#endif
