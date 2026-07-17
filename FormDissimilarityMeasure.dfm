object frmDissimilarityMeasure: TfrmDissimilarityMeasure
  Left = 0
  Top = 0
  Caption = 'Distance Measure'
  ClientHeight = 721
  ClientWidth = 1169
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label5: TLabel
    Left = 24
    Top = 700
    Width = 85
    Height = 13
    Caption = 'Job dissimilarity ='
  end
  object lblJDissmM: TLabel
    Left = 120
    Top = 700
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblJDissmM'
    Color = clHighlightText
    ParentColor = False
  end
  object Label6: TLabel
    Left = 248
    Top = 700
    Width = 126
    Height = 13
    Caption = 'Workstation dissimilarity ='
  end
  object lblWDissmM: TLabel
    Left = 383
    Top = 700
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblWDissmM'
    Color = clHighlightText
    ParentColor = False
  end
  object Label7: TLabel
    Left = 522
    Top = 700
    Width = 52
    Height = 13
    Caption = 'Distance ='
  end
  object lblDistance: TLabel
    Left = 586
    Top = 700
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblDistance'
    Color = clHighlightText
    ParentColor = False
  end
  object lblDistance_Apr: TLabel
    Left = 586
    Top = 676
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblDistance_Apr'
    Color = clHighlightText
    ParentColor = False
  end
  object Label12: TLabel
    Left = 522
    Top = 676
    Width = 52
    Height = 13
    Caption = 'Distance ='
  end
  object lblWDissmM_Apr: TLabel
    Left = 383
    Top = 676
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblWDissmM_Apr'
    Color = clHighlightText
    ParentColor = False
  end
  object Label10: TLabel
    Left = 248
    Top = 676
    Width = 126
    Height = 13
    Caption = 'Workstation dissimilarity ='
  end
  object lblJDissmM_Apr: TLabel
    Left = 120
    Top = 676
    Width = 104
    Height = 13
    AutoSize = False
    Caption = 'lblJDissmM_Apr'
    Color = clHighlightText
    ParentColor = False
  end
  object Label8: TLabel
    Left = 24
    Top = 676
    Width = 85
    Height = 13
    Caption = 'Job dissimilarity ='
  end
  object pnlSol1: TPanel
    Left = 0
    Top = 0
    Width = 1169
    Height = 334
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 23
      Top = 8
      Width = 234
      Height = 13
      Caption = 'Processing sequences of operations on machines'
    end
    object Label4: TLabel
      Left = 23
      Top = 168
      Width = 201
      Height = 13
      Caption = 'Processing order of operations within jobs'
    end
    object btnSave1: TSpeedButton
      Left = 329
      Top = 136
      Width = 64
      Height = 25
      Hint = 'Save the current solution (if feasible and complete) to file'
      Caption = 'Save'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333330070
        7700333333337777777733333333008088003333333377F73377333333330088
        88003333333377FFFF7733333333000000003FFFFFFF77777777000000000000
        000077777777777777770FFFFFFF0FFFFFF07F3333337F3333370FFFFFFF0FFF
        FFF07F3FF3FF7FFFFFF70F00F0080CCC9CC07F773773777777770FFFFFFFF039
        99337F3FFFF3F7F777F30F0000F0F09999937F7777373777777F0FFFFFFFF999
        99997F3FF3FFF77777770F00F000003999337F773777773777F30FFFF0FF0339
        99337F3FF7F3733777F30F08F0F0337999337F7737F73F7777330FFFF0039999
        93337FFFF7737777733300000033333333337777773333333333}
      NumGlyphs = 2
      OnClick = btnSave1Click
    end
    object btnLoad1: TSpeedButton
      Left = 328
      Top = 97
      Width = 66
      Height = 25
      Hint = 'Load a solution from file'
      Caption = 'Load'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
        555555FFFFFFFFFF55555000000000055555577777777775FFFF00B8B8B8B8B0
        0000775F5555555777770B0B8B8B8B8B0FF07F75F555555575F70FB0B8B8B8B8
        B0F07F575FFFFFFFF7F70BFB0000000000F07F557777777777570FBFBF0FFFFF
        FFF07F55557F5FFFFFF70BFBFB0F000000F07F55557F777777570FBFBF0FFFFF
        FFF075F5557F5FFFFFF750FBFB0F000000F0575FFF7F777777575700000FFFFF
        FFF05577777F5FF55FF75555550F00FF00005555557F775577775555550FFFFF
        0F055555557F55557F755555550FFFFF00555555557FFFFF7755555555000000
        0555555555777777755555555555555555555555555555555555}
      NumGlyphs = 2
      OnClick = btnLoad1Click
    end
    object btnRunDSG1: TSpeedButton
      Left = 328
      Top = 19
      Width = 65
      Height = 25
      Hint = 
        'Run dense schedule generator to construct new solution using the' +
        ' selected job and machine selection rules'
      Caption = 'Run DSG'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0033333333B333
        333B33FF33337F3333F73BB3777BB7777BB3377FFFF77FFFF77333B000000000
        0B3333777777777777333330FFFFFFFF07333337F33333337F333330FFFFFFFF
        07333337F3FF3FFF7F333330F00F000F07333337F77377737F333330FFFFFFFF
        07333FF7F3FFFF3F7FFFBBB0F0000F0F0BB37777F7777373777F3BB0FFFFFFFF
        0BBB3777F3FF3FFF77773330F00F000003333337F773777773333330FFFF0FF0
        33333337F3FF7F37F3333330F08F0F0B33333337F7737F77FF333330FFFF003B
        B3333337FFFF77377FF333B000000333BB33337777777F3377FF3BB3333BB333
        3BB33773333773333773B333333B3333333B7333333733333337}
      NumGlyphs = 2
      OnClick = btnRunDSG1Click
    end
    object grdPS1: TStringGrid
      Left = 15
      Top = 24
      Width = 299
      Height = 129
      DefaultColWidth = 48
      DefaultRowHeight = 20
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 0
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object grdOO1: TStringGrid
      Left = 15
      Top = 184
      Width = 299
      Height = 129
      DefaultColWidth = 48
      DefaultRowHeight = 20
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 1
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object btnRunPSI1: TBitBtn
      Left = 330
      Top = 57
      Width = 64
      Height = 25
      Hint = 
        'Run processing sequence interpretter to translate the given proc' +
        'essing sequences and orders into scheduled tasks'
      Caption = 'Run PSI'
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
      TabOrder = 2
      OnClick = btnRunPSI1Click
    end
    object pnlGantt1: TPanel
      Left = 408
      Top = 1
      Width = 760
      Height = 332
      Align = alRight
      TabOrder = 3
    end
  end
  object pnlSol2: TPanel
    Left = 0
    Top = 334
    Width = 1169
    Height = 331
    Align = alTop
    TabOrder = 1
    object Label2: TLabel
      Left = 23
      Top = 168
      Width = 201
      Height = 13
      Caption = 'Processing order of operations within jobs'
    end
    object Label3: TLabel
      Left = 23
      Top = 8
      Width = 234
      Height = 13
      Caption = 'Processing sequences of operations on machines'
    end
    object btnSave2: TSpeedButton
      Left = 329
      Top = 136
      Width = 64
      Height = 25
      Hint = 'Save the current solution (if feasible and complete) to file'
      Caption = 'Save'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333330070
        7700333333337777777733333333008088003333333377F73377333333330088
        88003333333377FFFF7733333333000000003FFFFFFF77777777000000000000
        000077777777777777770FFFFFFF0FFFFFF07F3333337F3333370FFFFFFF0FFF
        FFF07F3FF3FF7FFFFFF70F00F0080CCC9CC07F773773777777770FFFFFFFF039
        99337F3FFFF3F7F777F30F0000F0F09999937F7777373777777F0FFFFFFFF999
        99997F3FF3FFF77777770F00F000003999337F773777773777F30FFFF0FF0339
        99337F3FF7F3733777F30F08F0F0337999337F7737F73F7777330FFFF0039999
        93337FFFF7737777733300000033333333337777773333333333}
      NumGlyphs = 2
      OnClick = btnSave2Click
    end
    object btnLoad2: TSpeedButton
      Left = 328
      Top = 97
      Width = 66
      Height = 25
      Hint = 'Load a solution from file'
      Caption = 'Load'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
        555555FFFFFFFFFF55555000000000055555577777777775FFFF00B8B8B8B8B0
        0000775F5555555777770B0B8B8B8B8B0FF07F75F555555575F70FB0B8B8B8B8
        B0F07F575FFFFFFFF7F70BFB0000000000F07F557777777777570FBFBF0FFFFF
        FFF07F55557F5FFFFFF70BFBFB0F000000F07F55557F777777570FBFBF0FFFFF
        FFF075F5557F5FFFFFF750FBFB0F000000F0575FFF7F777777575700000FFFFF
        FFF05577777F5FF55FF75555550F00FF00005555557F775577775555550FFFFF
        0F055555557F55557F755555550FFFFF00555555557FFFFF7755555555000000
        0555555555777777755555555555555555555555555555555555}
      NumGlyphs = 2
      OnClick = btnLoad2Click
    end
    object btnRunDSG2: TSpeedButton
      Left = 328
      Top = 19
      Width = 65
      Height = 25
      Hint = 
        'Run dense schedule generator to construct new solution using the' +
        ' selected job and machine selection rules'
      Caption = 'Run DSG'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0033333333B333
        333B33FF33337F3333F73BB3777BB7777BB3377FFFF77FFFF77333B000000000
        0B3333777777777777333330FFFFFFFF07333337F33333337F333330FFFFFFFF
        07333337F3FF3FFF7F333330F00F000F07333337F77377737F333330FFFFFFFF
        07333FF7F3FFFF3F7FFFBBB0F0000F0F0BB37777F7777373777F3BB0FFFFFFFF
        0BBB3777F3FF3FFF77773330F00F000003333337F773777773333330FFFF0FF0
        33333337F3FF7F37F3333330F08F0F0B33333337F7737F77FF333330FFFF003B
        B3333337FFFF77377FF333B000000333BB33337777777F3377FF3BB3333BB333
        3BB33773333773333773B333333B3333333B7333333733333337}
      NumGlyphs = 2
      OnClick = btnRunDSG2Click
    end
    object grdOO2: TStringGrid
      Left = 15
      Top = 184
      Width = 299
      Height = 129
      DefaultColWidth = 48
      DefaultRowHeight = 20
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 0
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object grdPS2: TStringGrid
      Left = 15
      Top = 24
      Width = 299
      Height = 129
      DefaultColWidth = 48
      DefaultRowHeight = 20
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 1
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object pnlGantt2: TPanel
      Left = 408
      Top = 1
      Width = 760
      Height = 329
      Align = alRight
      TabOrder = 2
    end
    object btnRunPSI2: TBitBtn
      Left = 330
      Top = 57
      Width = 64
      Height = 25
      Hint = 
        'Run processing sequence interpretter to translate the given proc' +
        'essing sequences and orders into scheduled tasks'
      Caption = 'Run PSI'
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
      TabOrder = 3
      OnClick = btnRunPSI2Click
    end
  end
  object btnClose: TBitBtn
    Left = 1075
    Top = 679
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
    TabOrder = 2
  end
  object btnDissmM: TButton
    Left = 700
    Top = 695
    Width = 145
    Height = 25
    Caption = 'Unverified Distance'
    TabOrder = 3
    OnClick = btnDissmMClick
  end
  object btnAprDistance: TButton
    Left = 700
    Top = 671
    Width = 145
    Height = 25
    Caption = 'Estimated Distance'
    TabOrder = 4
    OnClick = btnAprDistanceClick
  end
end
