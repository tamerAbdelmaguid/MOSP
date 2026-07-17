object frmSSPRMultiRuns: TfrmSSPRMultiRuns
  Left = 0
  Top = 0
  Caption = 'SS/PR multiple runs'
  ClientHeight = 628
  ClientWidth = 654
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label13: TLabel
    Left = 120
    Top = 24
    Width = 72
    Height = 13
    Caption = 'Population Size'
  end
  object Label16: TLabel
    Left = 40
    Top = 50
    Width = 150
    Height = 13
    Caption = 'Distance Acceptance Threshold'
  end
  object Label17: TLabel
    Left = 104
    Top = 76
    Width = 90
    Height = 13
    Caption = 'Maximum tabu size'
  end
  object Label18: TLabel
    Left = 37
    Top = 103
    Width = 159
    Height = 13
    Caption = 'Maximum number of TS iterations'
  end
  object Label19: TLabel
    Left = 343
    Top = 51
    Width = 133
    Height = 13
    Caption = 'Improvement iterations limit'
  end
  object Label24: TLabel
    Left = 331
    Top = 77
    Width = 142
    Height = 13
    Caption = 'Number of stored best moves'
  end
  object Label25: TLabel
    Left = 350
    Top = 101
    Width = 123
    Height = 13
    Caption = 'Number of random moves'
  end
  object Label26: TLabel
    Left = 343
    Top = 24
    Width = 126
    Height = 13
    Caption = 'Number of SSPR iterations'
  end
  object Label27: TLabel
    Left = 159
    Top = 125
    Width = 33
    Height = 13
    Caption = 'Epsilon'
  end
  object lblLB: TLabel
    Left = 231
    Top = 235
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
    Left = 137
    Top = 235
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
    Left = 350
    Top = 235
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
    Left = 430
    Top = 235
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
    Left = 51
    Top = 315
    Width = 135
    Height = 13
    Caption = 'SSPR improvement progress'
  end
  object Label9: TLabel
    Left = 51
    Top = 349
    Width = 106
    Height = 13
    Caption = 'Construction progress'
  end
  object Label2: TLabel
    Left = 51
    Top = 278
    Width = 24
    Height = 13
    Caption = 'Runs'
  end
  object btnTerminate: TSpeedButton
    Left = 378
    Top = 175
    Width = 89
    Height = 25
    Caption = 'Terminate'
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000130B0000130B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333FFFFF3333333333999993333333333F77777FFF333333999999999
      33333337777FF377FF3333993370739993333377FF373F377FF3399993000339
      993337777F777F3377F3393999707333993337F77737333337FF993399933333
      399377F3777FF333377F993339903333399377F33737FF33377F993333707333
      399377F333377FF3377F993333101933399377F333777FFF377F993333000993
      399377FF3377737FF7733993330009993933373FF3777377F7F3399933000399
      99333773FF777F777733339993707339933333773FF7FFF77333333999999999
      3333333777333777333333333999993333333333377777333333}
    NumGlyphs = 2
    OnClick = btnTerminateClick
  end
  object Label1: TLabel
    Left = 403
    Top = 392
    Width = 48
    Height = 13
    Caption = 'Makespan'
  end
  object Label3: TLabel
    Left = 512
    Top = 392
    Width = 22
    Height = 13
    Caption = 'Time'
  end
  object edtPopSize: TEdit
    Left = 205
    Top = 21
    Width = 79
    Height = 21
    TabOrder = 0
    Text = 'edtPopSize'
  end
  object edtDeltaThreshold: TEdit
    Left = 205
    Top = 48
    Width = 79
    Height = 21
    TabOrder = 1
    Text = 'edtDeltaThreshold'
  end
  object edtTabuSize: TEdit
    Left = 205
    Top = 72
    Width = 79
    Height = 21
    TabOrder = 2
    Text = 'edtTabuSize'
  end
  object edtnTSIterations: TEdit
    Left = 205
    Top = 98
    Width = 79
    Height = 21
    TabOrder = 3
    Text = 'edtnTSIterations'
  end
  object edtnImpIterations: TEdit
    Left = 486
    Top = 46
    Width = 89
    Height = 21
    TabOrder = 4
    Text = 'edtnImpIterations'
  end
  object edtTSnBestMoves: TEdit
    Left = 486
    Top = 72
    Width = 89
    Height = 21
    TabOrder = 5
    Text = 'edtTSnBestMoves'
  end
  object edtTSnRandomMoves: TEdit
    Left = 486
    Top = 98
    Width = 89
    Height = 21
    TabOrder = 6
    Text = 'edtTSnRandomMoves'
  end
  object edtnSSPRIterations: TEdit
    Left = 486
    Top = 19
    Width = 89
    Height = 21
    TabOrder = 7
    Text = 'edtnSSPRIterations'
  end
  object edtEpsilon: TEdit
    Left = 205
    Top = 123
    Width = 79
    Height = 21
    TabOrder = 8
    Text = 'edtEpsilon'
  end
  object grdRuns: TStringGrid
    Left = 94
    Top = 392
    Width = 259
    Height = 228
    ColCount = 4
    RowCount = 31
    TabOrder = 9
    ColWidths = (
      53
      62
      57
      56)
  end
  object rgNRuns: TRadioGroup
    Left = 134
    Top = 153
    Width = 121
    Height = 71
    Caption = 'Number of runs '
    ItemIndex = 1
    Items.Strings = (
      '10 runs'
      '30 runs'
      'L27')
    TabOrder = 10
  end
  object btnClose: TBitBtn
    Left = 486
    Top = 175
    Width = 75
    Height = 25
    Caption = 'Close'
    DoubleBuffered = True
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00388888888877
      F7F787F8888888888333333F00004444400888FFF444448888888888F333FF8F
      000033334D5007FFF4333388888888883338888F0000333345D50FFFF4333333
      338F888F3338F33F000033334D5D0FFFF43333333388788F3338F33F00003333
      45D50FEFE4333333338F878F3338F33F000033334D5D0FFFF43333333388788F
      3338F33F0000333345D50FEFE4333333338F878F3338F33F000033334D5D0FFF
      F43333333388788F3338F33F0000333345D50FEFE4333333338F878F3338F33F
      000033334D5D0EFEF43333333388788F3338F33F0000333345D50FEFE4333333
      338F878F3338F33F000033334D5D0EFEF43333333388788F3338F33F00003333
      4444444444333333338F8F8FFFF8F33F00003333333333333333333333888888
      8888333F00003333330000003333333333333FFFFFF3333F00003333330AAAA0
      333333333333888888F3333F00003333330000003333333333338FFFF8F3333F
      0000}
    ModalResult = 11
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 11
  end
  object btnRun: TBitBtn
    Left = 285
    Top = 175
    Width = 75
    Height = 25
    Caption = 'Execute'
    DoubleBuffered = True
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      0400000000000001000000000000000000001000000010000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555550FF0559
      1950555FF75F7557F7F757000FF055591903557775F75557F77570FFFF055559
      1933575FF57F5557F7FF0F00FF05555919337F775F7F5557F7F700550F055559
      193577557F7F55F7577F07550F0555999995755575755F7FFF7F5570F0755011
      11155557F755F777777555000755033305555577755F75F77F55555555503335
      0555555FF5F75F757F5555005503335505555577FF75F7557F55505050333555
      05555757F75F75557F5505000333555505557F777FF755557F55000000355557
      07557777777F55557F5555000005555707555577777FF5557F55553000075557
      0755557F7777FFF5755555335000005555555577577777555555}
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 12
    OnClick = btnRunClick
  end
  object pbSSPRIterations: TProgressBar
    Left = 203
    Top = 309
    Width = 394
    Height = 25
    TabOrder = 13
  end
  object pbConstruction: TProgressBar
    Left = 203
    Top = 343
    Width = 394
    Height = 25
    TabOrder = 14
  end
  object pbRun: TProgressBar
    Left = 94
    Top = 269
    Width = 503
    Height = 25
    TabOrder = 15
  end
  object memoMakespan: TMemo
    Left = 399
    Top = 411
    Width = 77
    Height = 209
    Lines.Strings = (
      'memoMakesp'
      'an')
    TabOrder = 16
  end
  object memoTime: TMemo
    Left = 512
    Top = 411
    Width = 77
    Height = 209
    Lines.Strings = (
      'Memo1')
    TabOrder = 17
  end
end
