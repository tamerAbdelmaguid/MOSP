//---------------------------------------------------------------------------

#ifndef FormTSH
#define FormTSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "TS.h"
//---------------------------------------------------------------------------
class TfrmTS : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtnIterations;
	TEdit *edtMaxTime;
	TEdit *edtTabuSize;
	TEdit *edtnImpIterations;
	TLabel *Label4;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
private:	// User declarations
    MOSP_TS *TS;

public:		// User declarations
	__fastcall TfrmTS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTS *frmTS;
//---------------------------------------------------------------------------
#endif
