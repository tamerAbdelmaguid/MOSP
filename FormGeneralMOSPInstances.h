//---------------------------------------------------------------------------

#ifndef FormGeneralMOSPInstancesH
#define FormGeneralMOSPInstancesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmGeneralMOSPInstances : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *btnClose;
	TBitBtn *btnGenerate;
	TComboBox *cbxNoOfPCs;
	TLabel *Label2;
	TLabel *Label1;
	TComboBox *cbxnJobsTonWCs;
	TLabel *Label3;
	TComboBox *cbxqwLevel;
	TLabel *Label4;
	TComboBox *cbxLoadingLevel;
	TLabel *Label5;
	TComboBox *cbxPTVL;
	TLabel *Label6;
	TEdit *edtnReplicates;
	TRadioGroup *rgExperimentType;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnGenerateClick(TObject *Sender);
	void __fastcall rgExperimentTypeClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmGeneralMOSPInstances(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGeneralMOSPInstances *frmGeneralMOSPInstances;
//---------------------------------------------------------------------------
#endif
