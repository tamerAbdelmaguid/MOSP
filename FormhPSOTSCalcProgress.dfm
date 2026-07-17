inherited frmhPSOTSCalcProgress: TfrmhPSOTSCalcProgress
  Caption = 'Hybrid PSO-TS Calculation Progress...'
  ClientHeight = 272
  ExplicitHeight = 296
  PixelsPerInch = 96
  TextHeight = 13
  inherited btnTerminate: TSpeedButton
    Top = 232
    ExplicitTop = 232
  end
  inherited btnStart: TSpeedButton
    Top = 232
    ExplicitTop = 232
  end
  inherited btnClose: TSpeedButton
    Top = 232
    ExplicitTop = 232
  end
  inherited Label1: TLabel
    Top = 7
    Visible = False
    ExplicitTop = 7
  end
  inherited Label2: TLabel
    Top = 90
    ExplicitTop = 90
  end
  inherited Label3: TLabel
    Left = 160
    Width = 73
    Caption = 'PSO Iteration #'
    ExplicitLeft = 160
    ExplicitWidth = 73
  end
  inherited lblCurrentCmax: TLabel
    Top = 7
    Visible = False
    ExplicitTop = 7
  end
  inherited lblBestCmax: TLabel
    Top = 90
    ExplicitTop = 90
  end
  inherited lblIter: TLabel
    Caption = 'lblIterPSO'
  end
  inherited Label6: TLabel
    Left = 110
    Top = 8
    Visible = False
    ExplicitLeft = 110
    ExplicitTop = 8
  end
  inherited lblImpIter: TLabel
    Left = 239
    Top = 8
    Visible = False
    ExplicitLeft = 239
    ExplicitTop = 8
  end
  object Label7: TLabel [15]
    Left = 168
    Top = 122
    Width = 65
    Height = 13
    Caption = 'TS Iteration #'
  end
  object lblIterTS: TLabel [16]
    Left = 250
    Top = 122
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblIterTS'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  inherited ProgressBar: TProgressBar
    Top = 188
    ExplicitTop = 188
  end
end
