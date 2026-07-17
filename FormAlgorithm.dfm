object frmAlgorithm: TfrmAlgorithm
  Left = 125
  Top = 102
  ClientHeight = 594
  ClientWidth = 774
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlSolution: TPanel
    Left = 0
    Top = 208
    Width = 774
    Height = 386
    Align = alClient
    TabOrder = 0
  end
  object pnlGTAParameters: TPanel
    Left = 0
    Top = 0
    Width = 774
    Height = 208
    Align = alTop
    TabOrder = 1
    object Label5: TLabel
      Left = 290
      Top = 144
      Width = 102
      Height = 13
      Caption = 'Computational Time ='
    end
    object lblTime: TLabel
      Left = 397
      Top = 144
      Width = 33
      Height = 13
      Caption = 'lblTime'
    end
    object btnRun: TBitBtn
      Left = 224
      Top = 167
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
      TabOrder = 0
    end
    object btnClose: TBitBtn
      Left = 456
      Top = 167
      Width = 75
      Height = 25
      Caption = 'Close'
      DoubleBuffered = True
      Kind = bkClose
      ParentDoubleBuffered = False
      TabOrder = 1
      OnClick = btnCloseClick
    end
  end
end
