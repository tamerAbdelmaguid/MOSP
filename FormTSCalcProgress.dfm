inherited frmTSCalcProgress: TfrmTSCalcProgress
  Caption = 'Calculation progress for TS'
  ClientHeight = 307
  ClientWidth = 505
  OnShow = FormShow
  ExplicitWidth = 511
  ExplicitHeight = 331
  PixelsPerInch = 96
  TextHeight = 13
  inherited btnTerminate: TSpeedButton
    Left = 216
    Top = 264
    ExplicitLeft = 216
    ExplicitTop = 264
  end
  inherited btnStart: TSpeedButton
    Left = 104
    Top = 264
    ExplicitLeft = 104
    ExplicitTop = 264
  end
  inherited btnClose: TSpeedButton
    Left = 328
    Top = 264
    ExplicitLeft = 328
    ExplicitTop = 264
  end
  object Label1: TLabel
    Left = 170
    Top = 90
    Width = 63
    Height = 13
    Caption = 'Current Cmax'
  end
  object Label2: TLabel
    Left = 183
    Top = 122
    Width = 50
    Height = 13
    Caption = 'Best Cmax'
  end
  object Label3: TLabel
    Left = 185
    Top = 154
    Width = 48
    Height = 13
    Caption = 'Iteration #'
  end
  object lblCurrentCmax: TLabel
    Left = 250
    Top = 90
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblCurrentCmax'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object lblBestCmax: TLabel
    Left = 250
    Top = 122
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblBestCmax'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object lblIter: TLabel
    Left = 250
    Top = 154
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblIter'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object Label4: TLabel
    Left = 180
    Top = 58
    Width = 53
    Height = 13
    Caption = 'Initial Cmax'
  end
  object lblInitialCmax: TLabel
    Left = 250
    Top = 58
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblInitialCmax'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object Label5: TLabel
    Left = 171
    Top = 26
    Width = 62
    Height = 13
    Caption = 'Lower bound'
  end
  object lblLB: TLabel
    Left = 250
    Top = 26
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblLB'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object Label6: TLabel
    Left = 121
    Top = 186
    Width = 112
    Height = 13
    Caption = 'Improvement Iteration #'
  end
  object lblImpIter: TLabel
    Left = 250
    Top = 186
    Width = 105
    Height = 13
    AutoSize = False
    Caption = 'lblImpIter'
    Color = clWhite
    ParentColor = False
    Transparent = False
  end
  object ProgressBar: TProgressBar
    Left = 24
    Top = 220
    Width = 457
    Height = 25
    TabOrder = 0
  end
end
