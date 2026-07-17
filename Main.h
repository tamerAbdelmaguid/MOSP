//----------------------------------------------------------------------------
#ifndef MainH
#define MainH
//----------------------------------------------------------------------------
#include "FormProblem.h"
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Messages.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <ToolWin.hpp>
//----------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *FileNewItem;
	TMenuItem *FileOpenItem;
	TMenuItem *FileCloseItem;
	TMenuItem *Window1;
	TMenuItem *Help1;
	TMenuItem *N1;
	TMenuItem *FileExitItem;
	TMenuItem *WindowCascadeItem;
	TMenuItem *WindowTileItem;
	TMenuItem *WindowArrangeItem;
	TMenuItem *HelpAboutItem;
	TOpenDialog *OpenDialog;
	TMenuItem *FileSaveItem;
	TMenuItem *FileSaveAsItem;
	TMenuItem *Edit1;
	TMenuItem *CutItem;
	TMenuItem *CopyItem;
	TMenuItem *PasteItem;
	TMenuItem *WindowMinimizeItem;
        TStatusBar *StatusBar;
        TActionList *ActionList1;
        TEditCut *EditCut1;
        TEditCopy *EditCopy1;
        TEditPaste *EditPaste1;
        TAction *FileNew1;
        TAction *FileSave1;
        TAction *FileExit1;
        TAction *FileOpen1;
        TAction *FileSaveAs1;
        TWindowCascade *WindowCascade1;
        TWindowTileHorizontal *WindowTileHorizontal1;
        TWindowArrange *WindowArrangeAll1;
        TWindowMinimizeAll *WindowMinimizeAll1;
        TAction *HelpAbout1;
        TWindowClose *FileClose1;
        TWindowTileVertical *WindowTileVertical1;
        TMenuItem *WindowTileItem2;
        TToolBar *ToolBar2;
        TToolButton *btnOpen;
        TToolButton *btnSave;
        TToolButton *ToolButton3;
        TToolButton *btnCut;
        TToolButton *btnCopy;
        TToolButton *btnPaste;
        TToolButton *ToolButton7;
        TToolButton *ToolButton8;
        TToolButton *btnNew;
        TToolButton *ToolButton10;
        TToolButton *ToolButton11;
        TImageList *ImageList1;
        TSaveDialog *SaveDialog;
	TSaveDialog *SaveDATDialog;
        TMenuItem *Exportto;
	TMenuItem *CPLEXDatFile;
        TMenuItem *mnuSolve;
        TOpenDialog *OpenDialogTestProblem;
        TMenuItem *mnuPSEditor;
	TMenuItem *GenerateMattasProblems1;
	TMenuItem *abuSearchAbdelmaguidetal20141;
	TMenuItem *HybridParticleSwarmOptimizationTabuSearch1;
	TMenuItem *mnuDissimilaritymeasure;
	TMenuItem *ScatterSearchwithPathRelinking1;
	TMenuItem *GenerategeneralMOSPinstances1;
	TMenuItem *SSPRmultipleruns1;
        void __fastcall FileNew1Execute(TObject *Sender);
        void __fastcall FileOpen1Execute(TObject *Sender);
        void __fastcall HelpAbout1Execute(TObject *Sender);
        void __fastcall FileExit1Execute(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FileSaveItemClick(TObject *Sender);
        void __fastcall FileSaveAsItemClick(TObject *Sender);
        void __fastcall CPLEXDatFileClick(TObject *Sender);
        void __fastcall mnuPSEditorClick(TObject *Sender);
	void __fastcall GenerateMattasProblems1Click(TObject *Sender);
	void __fastcall abuSearchAbdelmaguidetal20141Click(TObject *Sender);
	void __fastcall HybridParticleSwarmOptimizationTabuSearch1Click(TObject *Sender);
	void __fastcall mnuDissimilaritymeasureClick(TObject *Sender);
	void __fastcall ScatterSearchwithPathRelinking1Click(TObject *Sender);
	void __fastcall GenerategeneralMOSPinstances1Click(TObject *Sender);
	void __fastcall SSPRmultipleruns1Click(TObject *Sender);

private:
	void __fastcall CreateMDIChild(const AnsiString Name);
public:
	virtual __fastcall TMainForm(TComponent *Owner);
        void __fastcall UpdateWorkingDirectory(String newDir);

        String WorkingDirectory;
        TfrmProblem *CurrentChild;
};
//----------------------------------------------------------------------------
extern TMainForm *MainForm;
extern TfrmProblem *__fastcall MDIChildCreate(void);
//----------------------------------------------------------------------------
#endif    
