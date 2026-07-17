//---------------------------------------------------------------------------

#ifndef FormDissimilarityMeasureH
#define FormDissimilarityMeasureH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include "FormSol.h"
#include "Problem.h"
#include "MOSPSolver.h"
#include "PSI.h"
#include "SSPR.h"
//---------------------------------------------------------------------------
class TfrmDissimilarityMeasure : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlSol1;
	TPanel *pnlSol2;
	TStringGrid *grdPS1;
	TLabel *Label1;
	TLabel *Label4;
	TStringGrid *grdOO1;
	TStringGrid *grdOO2;
	TLabel *Label2;
	TStringGrid *grdPS2;
	TLabel *Label3;
	TPanel *pnlGantt2;
	TBitBtn *btnRunPSI1;
	TSpeedButton *btnSave1;
	TSpeedButton *btnLoad1;
	TPanel *pnlGantt1;
	TSpeedButton *btnRunDSG1;
	TBitBtn *btnRunPSI2;
	TSpeedButton *btnSave2;
	TSpeedButton *btnLoad2;
	TSpeedButton *btnRunDSG2;
	TBitBtn *btnClose;
	TLabel *Label5;
	TLabel *lblJDissmM;
	TLabel *Label6;
	TLabel *lblWDissmM;
	TButton *btnDissmM;
	TLabel *Label7;
	TLabel *lblDistance;
	TButton *btnAprDistance;
	TLabel *lblDistance_Apr;
	TLabel *Label12;
	TLabel *lblWDissmM_Apr;
	TLabel *Label10;
	TLabel *lblJDissmM_Apr;
	TLabel *Label8;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRunDSG1Click(TObject *Sender);
	void __fastcall btnRunDSG2Click(TObject *Sender);
	void __fastcall btnLoad1Click(TObject *Sender);
	void __fastcall btnSave1Click(TObject *Sender);
	void __fastcall btnLoad2Click(TObject *Sender);
	void __fastcall btnSave2Click(TObject *Sender);
	void __fastcall btnRunPSI1Click(TObject *Sender);
	void __fastcall btnRunPSI2Click(TObject *Sender);
	void __fastcall btnDissmMClick(TObject *Sender);
	void __fastcall btnAprDistanceClick(TObject *Sender);
private:	// User declarations
	TfrmSolution *frmSol1;
	TfrmSolution *frmSol2;

	MOSP_Solution *LoadedSol1;
	MOSP_Solution *LoadedSol2;

	MOSP_DSG *DSG1;
	MOSP_DSG *DSG2;
	MOSP_VPS *VPS1;
	MOSP_VOO *VOO1;
	MOSP_PSI *PSI1;
	MOSP_VPS *VPS2;
	MOSP_VOO *VOO2;
	MOSP_PSI *PSI2;

	MOSP_NSTester *NSTester1, *NSTester2;
	MOSP_VectorSol *Vec1, *Vec2;

	MOSP_SSPR_Solution *SSPR_Sol1, *SSPR_Sol2;
	MOSP_SSPR *SSPR_Alg;

public:		// User declarations
	MOSP_Problem *Problem;

	__fastcall TfrmDissimilarityMeasure(TComponent* Owner);

	void __fastcall DisplayVPS(TStringGrid *grdPS, MOSP_VPS *vps);
	void __fastcall UpdateVPS1(MOSP_Solution *Sol); // this also updates VPS
	void __fastcall UpdateVPS2(MOSP_Solution *Sol); // this also updates VPS
	bool __fastcall ReadVPS(TStringGrid *grdPS, int vpsno);

	void __fastcall DisplayVOO(TStringGrid *grdOO, MOSP_VOO *voo);
	void __fastcall UpdateVOO1(MOSP_Solution *Sol); // this also updates VPS
	void __fastcall UpdateVOO2(MOSP_Solution *Sol); // this also updates VPS
	bool __fastcall ReadVOO(TStringGrid *grdOO, int voono);

	bool __fastcall InterpretVectors1();
	bool __fastcall InterpretVectors2();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDissimilarityMeasure *frmDissimilarityMeasure;
//---------------------------------------------------------------------------
#endif
