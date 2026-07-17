//---------------------------------------------------------------------------

#ifndef FormCalcProgressH
#define FormCalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "Algorithm.h"

//---------------------------------------------------------------------------
class TfrmCalcProgress : public TForm
{
__published:	// IDE-managed Components
        TSpeedButton *btnTerminate;
        TSpeedButton *btnStart;
        TSpeedButton *btnClose;
        void __fastcall btnTerminateClick(TObject *Sender);
        void __fastcall btnStartClick(TObject *Sender);
        void __fastcall btnCloseClick(TObject *Sender);
private:	// User declarations
        Algorithm *Alg;
        
public:		// User declarations
        __fastcall TfrmCalcProgress(TComponent* Owner);

        void __fastcall Show(Algorithm *a);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCalcProgress *frmCalcProgress;
//---------------------------------------------------------------------------
#endif
