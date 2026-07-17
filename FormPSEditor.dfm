inherited frmPSEditor: TfrmPSEditor
  Left = 170
  Top = 13
  Caption = 'Solution Permutations Editor'
  ClientHeight = 736
  ClientWidth = 983
  OldCreateOrder = True
  ExplicitLeft = -106
  ExplicitWidth = 999
  ExplicitHeight = 775
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Top = 241
    Width = 704
    Height = 495
    ExplicitTop = 241
    ExplicitWidth = 704
    ExplicitHeight = 495
  end
  inherited pnlGTAParameters: TPanel
    Width = 983
    Height = 241
    ExplicitWidth = 983
    ExplicitHeight = 241
    inherited Label5: TLabel
      Left = 254
      Top = 225
      Visible = False
      ExplicitLeft = 254
      ExplicitTop = 225
    end
    inherited lblTime: TLabel
      Left = 373
      Top = 225
      Visible = False
      ExplicitLeft = 373
      ExplicitTop = 225
    end
    object btnRunDSG: TSpeedButton [2]
      Left = 889
      Top = 29
      Width = 75
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
      OnClick = btnRunDSGClick
    end
    object btnLoad: TSpeedButton [3]
      Left = 892
      Top = 63
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
      OnClick = btnLoadClick
    end
    object btnSave: TSpeedButton [4]
      Left = 815
      Top = 63
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
      OnClick = btnSaveClick
    end
    object Label1: TLabel [5]
      Left = 23
      Top = 8
      Width = 234
      Height = 13
      Caption = 'Processing sequences of operations on machines'
    end
    object Label2: TLabel [6]
      Left = 656
      Top = 12
      Width = 85
      Height = 13
      Caption = 'Job selection rule:'
    end
    object Label3: TLabel [7]
      Left = 771
      Top = 12
      Width = 109
      Height = 13
      Caption = 'Machine selection rule:'
    end
    object Label4: TLabel [8]
      Left = 350
      Top = 8
      Width = 195
      Height = 13
      Caption = 'Processing order of operations within jobs'
    end
    inherited btnRun: TBitBtn
      Left = 664
      Top = 63
      Hint = 
        'Run processing sequence interpretter to translate the given proc' +
        'essing sequences and orders into scheduled tasks'
      Caption = 'Run PSI'
      OnClick = btnRunClick
      ExplicitLeft = 664
      ExplicitTop = 63
    end
    inherited btnClose: TBitBtn
      Left = 894
      Top = 3
      Width = 65
      Height = 22
      Kind = bkCustom
      Visible = False
      ExplicitLeft = 894
      ExplicitTop = 3
      ExplicitWidth = 65
      ExplicitHeight = 22
    end
    object grdPS: TStringGrid
      Left = 15
      Top = 24
      Width = 306
      Height = 201
      DefaultColWidth = 48
      DefaultRowHeight = 20
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 2
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object cbxJSR: TComboBox
      Left = 656
      Top = 31
      Width = 98
      Height = 21
      Style = csDropDownList
      TabOrder = 3
      Items.Strings = (
        'FCFS'
        'SPT'
        'LPT'
        'MCT'
        'MOR'
        'LOR'
        'MWR'
        'LWR'
        'RANDOM')
    end
    object cbxMSR: TComboBox
      Left = 771
      Top = 31
      Width = 98
      Height = 21
      Style = csDropDownList
      TabOrder = 4
      Items.Strings = (
        'MIN_LOAD'
        'MINCT'
        'RANDOM')
    end
    object grdOO: TStringGrid
      Left = 342
      Top = 24
      Width = 299
      Height = 201
      DefaultColWidth = 48
      DefaultRowHeight = 20
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goTabs]
      TabOrder = 5
      ColWidths = (
        57
        48
        48
        48
        48)
    end
    object pgEditor: TPageControl
      Left = 656
      Top = 106
      Width = 308
      Height = 129
      ActivePage = tbJobPermutation
      TabOrder = 6
      object tbOpRemover: TTabSheet
        Caption = '1 Operation'
        object btnRemoveOp: TSpeedButton
          Left = 225
          Top = 8
          Width = 66
          Height = 25
          Caption = 'Remove'
          Glyph.Data = {
            76010000424D7601000000000000760000002800000020000000100000000100
            0400000000000001000000000000000000001000000010000000000000000000
            800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
            FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
            333333333333333333333333333333333333333FFF33FF333FFF339993370733
            999333777FF37FF377733339993000399933333777F777F77733333399970799
            93333333777F7377733333333999399933333333377737773333333333990993
            3333333333737F73333333333331013333333333333777FF3333333333910193
            333333333337773FF3333333399000993333333337377737FF33333399900099
            93333333773777377FF333399930003999333337773777F777FF339993370733
            9993337773337333777333333333333333333333333333333333333333333333
            3333333333333333333333333333333333333333333333333333}
          NumGlyphs = 2
          OnClick = btnRemoveOpClick
        end
        object Label15: TLabel
          Left = 76
          Top = 13
          Width = 46
          Height = 13
          Caption = 'Operation'
        end
        object Label16: TLabel
          Left = 113
          Top = 45
          Width = 41
          Height = 13
          Caption = 'Machine'
        end
        object Label17: TLabel
          Left = 113
          Top = 73
          Width = 37
          Height = 13
          Caption = 'Position'
        end
        object cbxOps: TComboBox
          Left = 132
          Top = 10
          Width = 77
          Height = 21
          Style = csDropDownList
          TabOrder = 0
        end
        object btnInitialize: TButton
          Left = 17
          Top = 9
          Width = 53
          Height = 24
          Caption = 'Initialize'
          TabOrder = 1
          OnClick = btnInitializeClick
        end
        object btnInsert: TButton
          Left = 241
          Top = 56
          Width = 51
          Height = 25
          Caption = 'Insert'
          TabOrder = 2
          OnClick = btnInsertClick
        end
        object rgRemoveFrom: TRadioGroup
          Left = 10
          Top = 39
          Width = 97
          Height = 56
          Caption = 'Remove from'
          ItemIndex = 0
          Items.Strings = (
            'job (tau)'
            'machine (pi)')
          TabOrder = 3
        end
        object cbxMachines: TComboBox
          Left = 160
          Top = 43
          Width = 68
          Height = 21
          Style = csDropDownList
          TabOrder = 4
          OnChange = cbxMachinesChange
        end
        object edtInPos: TEdit
          Left = 160
          Top = 70
          Width = 65
          Height = 21
          TabOrder = 5
          Text = 'edtInPos'
        end
      end
      object tbMCPermutation: TTabSheet
        Caption = 'M/C Sub-permutation'
        ImageIndex = 1
        object Label6: TLabel
          Left = 69
          Top = 26
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label7: TLabel
          Left = 156
          Top = 26
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label8: TLabel
          Left = 79
          Top = 61
          Width = 79
          Height = 13
          Caption = 'Insertion position'
        end
        object cbxFromMC: TComboBox
          Left = 6
          Top = 22
          Width = 57
          Height = 21
          Style = csDropDownList
          TabOrder = 0
        end
        object edtMCx1: TEdit
          Left = 97
          Top = 23
          Width = 53
          Height = 21
          TabOrder = 1
          Text = 'edtMCx1'
        end
        object edtMCx2: TEdit
          Left = 182
          Top = 23
          Width = 55
          Height = 21
          TabOrder = 2
          Text = 'edtMCx2'
        end
        object btnMCRemove: TButton
          Left = 243
          Top = 22
          Width = 49
          Height = 23
          Caption = 'Remove'
          TabOrder = 3
          OnClick = btnMCRemoveClick
        end
        object edtMCInsertX: TEdit
          Left = 164
          Top = 58
          Width = 65
          Height = 21
          TabOrder = 4
          Text = 'edtMCInsertX'
        end
        object btnMCInsert: TButton
          Left = 243
          Top = 57
          Width = 49
          Height = 23
          Caption = 'Insert'
          TabOrder = 5
          OnClick = btnMCInsertClick
        end
        object cbxToMC: TComboBox
          Left = 6
          Top = 58
          Width = 57
          Height = 21
          Style = csDropDownList
          TabOrder = 6
          OnChange = cbxToMCChange
        end
      end
      object tbJobPermutation: TTabSheet
        Caption = 'Job Sub-permutation'
        ImageIndex = 2
        object Label9: TLabel
          Left = 16
          Top = 45
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label10: TLabel
          Left = 119
          Top = 45
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label11: TLabel
          Left = 45
          Top = 77
          Width = 79
          Height = 13
          Caption = 'Insertion position'
        end
        object cbxJobs: TComboBox
          Left = 88
          Top = 13
          Width = 113
          Height = 21
          Style = csDropDownList
          TabOrder = 0
        end
        object edtJobx1: TEdit
          Left = 44
          Top = 42
          Width = 53
          Height = 21
          TabOrder = 1
          Text = 'edtJobx1'
        end
        object edtJobx2: TEdit
          Left = 145
          Top = 42
          Width = 55
          Height = 21
          TabOrder = 2
          Text = 'edtJobx2'
        end
        object btnJobRemove: TButton
          Left = 216
          Top = 41
          Width = 61
          Height = 23
          Caption = 'Remove'
          TabOrder = 3
          OnClick = btnJobRemoveClick
        end
        object edtJobInsertX: TEdit
          Left = 130
          Top = 74
          Width = 65
          Height = 21
          TabOrder = 4
          Text = 'edtJobInsertX'
        end
        object btnJobInsertX: TButton
          Left = 204
          Top = 72
          Width = 57
          Height = 23
          Caption = 'Insert'
          TabOrder = 5
          OnClick = btnJobInsertXClick
        end
      end
    end
  end
  object pnlAlgParamaeters: TPanel
    Left = 704
    Top = 241
    Width = 279
    Height = 495
    Align = alRight
    TabOrder = 2
    object pgAlgPars: TPageControl
      Left = 1
      Top = 1
      Width = 277
      Height = 493
      ActivePage = tbNfn
      Align = alClient
      TabOrder = 0
      object tbNfn: TTabSheet
        Caption = 'N1/N2'
        object PageControl1: TPageControl
          Left = 1
          Top = 176
          Width = 272
          Height = 297
          ActivePage = tsActVs
          TabOrder = 0
          object tsAprVs: TTabSheet
            Caption = 'Approximate'
            object Label18: TLabel
              Left = 12
              Top = 12
              Width = 92
              Height = 13
              Caption = 'Approximate values'
            end
            object sgAprVs: TStringGrid
              Left = 7
              Top = 42
              Width = 247
              Height = 205
              RowCount = 2
              Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
              TabOrder = 0
              ColWidths = (
                35
                43
                44
                44
                42)
            end
          end
          object tsEstVs: TTabSheet
            Caption = 'Estimated'
            ImageIndex = 1
            object Label19: TLabel
              Left = 13
              Top = 12
              Width = 202
              Height = 13
              Caption = 'Estimated values based on current solution'
            end
            object sgEstVs: TStringGrid
              Left = 7
              Top = 42
              Width = 246
              Height = 205
              RowCount = 2
              Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
              TabOrder = 0
              ColWidths = (
                35
                43
                44
                44
                42)
            end
          end
          object tsActVs: TTabSheet
            Caption = 'Actual'
            ImageIndex = 2
            object Label20: TLabel
              Left = 11
              Top = 13
              Width = 171
              Height = 13
              Caption = 'Actual values by conducting  moves'
            end
            object sgActVs: TStringGrid
              Left = 7
              Top = 42
              Width = 246
              Height = 205
              RowCount = 2
              Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
              TabOrder = 0
              ColWidths = (
                35
                43
                44
                44
                41)
            end
          end
        end
        object sgRemovalValues: TStringGrid
          Left = 32
          Top = 24
          Width = 209
          Height = 129
          ColCount = 2
          FixedRows = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
          TabOrder = 1
          ColWidths = (
            83
            121)
          RowHeights = (
            24
            24
            24
            24
            24)
        end
      end
      object tsBfn: TTabSheet
        Caption = 'B1/B2'
        ImageIndex = 1
        object Label14: TLabel
          Left = 16
          Top = 408
          Width = 176
          Height = 13
          Caption = 'Intersection of Insertion positions sets'
          Visible = False
        end
        object Label13: TLabel
          Left = 16
          Top = 360
          Width = 72
          Height = 13
          Caption = 'Intersection set'
          Visible = False
        end
        object Label12: TLabel
          Left = 24
          Top = 154
          Width = 119
          Height = 13
          Caption = 'Individual operations sets'
          Visible = False
        end
        object grdNbSets: TStringGrid
          Left = 24
          Top = 173
          Width = 225
          Height = 169
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
          TabOrder = 0
          Visible = False
          ColWidths = (
            64
            68
            64
            64
            64)
        end
        object edtSetI: TEdit
          Left = 24
          Top = 427
          Width = 217
          Height = 21
          TabOrder = 1
          Visible = False
        end
        object edtSetS: TEdit
          Left = 24
          Top = 379
          Width = 217
          Height = 21
          TabOrder = 2
          Visible = False
        end
        object sgB1AlphaOmega: TStringGrid
          Left = 29
          Top = 27
          Width = 209
          Height = 105
          ColCount = 2
          RowCount = 4
          FixedRows = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing, goEditing]
          TabOrder = 3
          ColWidths = (
            83
            121)
          RowHeights = (
            24
            24
            24
            24)
        end
      end
    end
  end
end
