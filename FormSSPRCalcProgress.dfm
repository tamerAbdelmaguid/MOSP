inherited frmSSPRCalcProgress: TfrmSSPRCalcProgress
  Caption = 'SS/PR Calculation Progress'
  ClientHeight = 628
  ClientWidth = 644
  OnClose = FormClose
  OnShow = FormShow
  ExplicitWidth = 650
  ExplicitHeight = 657
  PixelsPerInch = 96
  TextHeight = 13
  inherited btnTerminate: TSpeedButton
    Left = 277
    Top = 595
    ExplicitLeft = 277
    ExplicitTop = 595
  end
  inherited btnStart: TSpeedButton
    Left = 135
    Top = 595
    ExplicitLeft = 135
    ExplicitTop = 595
  end
  inherited btnClose: TSpeedButton
    Left = 428
    Top = 593
    ExplicitLeft = 428
    ExplicitTop = 593
  end
  object lblLB: TLabel
    Left = 229
    Top = 19
    Width = 65
    Height = 16
    AutoSize = False
    Caption = 'lblLB'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object Label5: TLabel
    Left = 135
    Top = 19
    Width = 83
    Height = 16
    Caption = 'Lower bound'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object Label10: TLabel
    Left = 348
    Top = 19
    Width = 71
    Height = 16
    Caption = 'Best Cmax'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object lblbestCmax: TLabel
    Left = 428
    Top = 19
    Width = 78
    Height = 16
    AutoSize = False
    Caption = 'lblbestCmax'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object Label15: TLabel
    Left = 40
    Top = 61
    Width = 135
    Height = 13
    Caption = 'SSPR improvement progress'
  end
  object pnlTabuSearch: TPanel
    Left = 136
    Top = 342
    Width = 378
    Height = 241
    BevelOuter = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 141
      Top = 74
      Width = 63
      Height = 13
      Caption = 'Current Cmax'
    end
    object Label2: TLabel
      Left = 131
      Top = 106
      Width = 75
      Height = 13
      Caption = 'Best local Cmax'
    end
    object Label3: TLabel
      Left = 156
      Top = 138
      Width = 48
      Height = 13
      Caption = 'Iteration #'
    end
    object lblTSCurrentCmax: TLabel
      Left = 221
      Top = 74
      Width = 105
      Height = 13
      AutoSize = False
      Caption = 'lblTSCurrentCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object lblTSBestCmax: TLabel
      Left = 221
      Top = 106
      Width = 105
      Height = 13
      AutoSize = False
      Caption = 'lblTSBestCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object lblTSIter: TLabel
      Left = 221
      Top = 138
      Width = 105
      Height = 13
      AutoSize = False
      Caption = 'lblTSIter'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label4: TLabel
      Left = 151
      Top = 42
      Width = 53
      Height = 13
      Caption = 'Initial Cmax'
    end
    object lblTSInitialCmax: TLabel
      Left = 221
      Top = 42
      Width = 105
      Height = 13
      AutoSize = False
      Caption = 'lblTSInitialCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label6: TLabel
      Left = 32
      Top = 170
      Width = 174
      Height = 13
      Caption = 'No. of iterations with no improvement'
    end
    object lblTSImpIter: TLabel
      Left = 221
      Top = 170
      Width = 105
      Height = 13
      AutoSize = False
      Caption = 'lblTSImpIter'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label7: TLabel
      Left = 97
      Top = 8
      Width = 181
      Height = 16
      Caption = 'Tabu Search local iterations'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object pbTabuSearch: TProgressBar
      Left = 16
      Top = 204
      Width = 345
      Height = 25
      TabOrder = 0
    end
  end
  object pnlConstruction: TPanel
    Left = 13
    Top = 110
    Width = 260
    Height = 217
    TabOrder = 1
    object Label8: TLabel
      Left = 24
      Top = 77
      Width = 168
      Height = 13
      Caption = 'List of Initially constructed solutions:'
    end
    object Label9: TLabel
      Left = 17
      Top = 27
      Width = 102
      Height = 13
      Caption = 'Construction progress'
    end
    object Label11: TLabel
      Left = 64
      Top = 3
      Width = 125
      Height = 16
      Caption = 'Construction phase'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object grdInitialSolutions: TStringGrid
      Left = 16
      Top = 96
      Width = 217
      Height = 106
      ColCount = 3
      TabOrder = 0
    end
    object pbConstruction: TProgressBar
      Left = 17
      Top = 46
      Width = 216
      Height = 25
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 293
    Top = 110
    Width = 332
    Height = 217
    TabOrder = 2
    object Label12: TLabel
      Left = 21
      Top = 77
      Width = 193
      Height = 13
      Caption = 'List of selected solutions in each iteration'
    end
    object Label13: TLabel
      Left = 59
      Top = 27
      Width = 114
      Height = 13
      Caption = 'Path relinking iteration #'
    end
    object Label14: TLabel
      Left = 92
      Top = 3
      Width = 154
      Height = 16
      Caption = 'Path relinking iterations'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object lblPRIter: TLabel
      Left = 189
      Top = 27
      Width = 86
      Height = 13
      AutoSize = False
      Caption = 'lblPRIter'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object sgPRIterations: TStringGrid
      Left = 21
      Top = 96
      Width = 292
      Height = 106
      FixedCols = 0
      TabOrder = 0
      ColWidths = (
        39
        54
        53
        58
        59)
    end
    object pbPRProgress: TProgressBar
      Left = 31
      Top = 46
      Width = 266
      Height = 25
      TabOrder = 1
    end
  end
  object pbSSPRIterations: TProgressBar
    Left = 192
    Top = 56
    Width = 376
    Height = 25
    TabOrder = 3
  end
end
