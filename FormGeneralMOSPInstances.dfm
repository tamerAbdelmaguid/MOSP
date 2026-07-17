object frmGeneralMOSPInstances: TfrmGeneralMOSPInstances
  Left = 0
  Top = 0
  Caption = 'Generating general MOSP instances'
  ClientHeight = 340
  ClientWidth = 434
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 187
    Top = 133
    Width = 41
    Height = 13
    Caption = '|J| / |W|'
  end
  object Label1: TLabel
    Left = 85
    Top = 102
    Width = 143
    Height = 13
    Caption = 'Number of workstations (|W|)'
  end
  object Label3: TLabel
    Left = 187
    Top = 163
    Width = 39
    Height = 13
    Caption = 'qw level'
  end
  object Label4: TLabel
    Left = 142
    Top = 195
    Width = 86
    Height = 13
    Caption = 'Loading Level (LL)'
  end
  object Label5: TLabel
    Left = 46
    Top = 226
    Width = 182
    Height = 13
    Caption = 'Processing time variability level (PTVL)'
  end
  object Label6: TLabel
    Left = 127
    Top = 258
    Width = 99
    Height = 13
    Caption = 'Number of replicates'
  end
  object btnClose: TBitBtn
    Left = 246
    Top = 300
    Width = 131
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
    ModalResult = 2
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object btnGenerate: TBitBtn
    Left = 67
    Top = 300
    Width = 137
    Height = 25
    Caption = 'Generate Instances'
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
    TabOrder = 1
    OnClick = btnGenerateClick
  end
  object cbxNoOfPCs: TComboBox
    Left = 245
    Top = 99
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 2
    Text = '6'
    Items.Strings = (
      '6'
      '12')
  end
  object cbxnJobsTonWCs: TComboBox
    Left = 245
    Top = 130
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 3
    Text = '2'
    Items.Strings = (
      '2'
      '4'
      '6')
  end
  object cbxqwLevel: TComboBox
    Left = 245
    Top = 160
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 4
    Text = '1 to 3'
    Items.Strings = (
      '1 to 3'
      '4 to 6')
  end
  object cbxLoadingLevel: TComboBox
    Left = 245
    Top = 192
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 5
    Text = '0.4'
    Items.Strings = (
      '0.4'
      '0.7'
      '0.9')
  end
  object cbxPTVL: TComboBox
    Left = 245
    Top = 223
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 6
    Text = '0.95 to 1.05'
    Items.Strings = (
      '0.95 to 1.05'
      '0.75 to 1.25')
  end
  object edtnReplicates: TEdit
    Left = 245
    Top = 255
    Width = 60
    Height = 21
    TabOrder = 7
    Text = '5'
  end
  object rgExperimentType: TRadioGroup
    Left = 85
    Top = 8
    Width = 257
    Height = 73
    Caption = 'Type of Experiment '
    Items.Strings = (
      'Factorial experiments'
      'Taguchi experiments')
    TabOrder = 8
    OnClick = rgExperimentTypeClick
  end
end
