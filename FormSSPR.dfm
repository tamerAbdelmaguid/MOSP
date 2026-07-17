inherited frmSSPR: TfrmSSPR
  Caption = 'Scatter Search with Path Relinking'
  ClientHeight = 767
  ClientWidth = 851
  ExplicitWidth = 867
  ExplicitHeight = 806
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Top = 217
    Width = 851
    Height = 550
    ExplicitTop = 217
    ExplicitWidth = 851
    ExplicitHeight = 550
  end
  inherited pnlGTAParameters: TPanel
    Width = 851
    Height = 217
    ExplicitWidth = 851
    ExplicitHeight = 217
    inherited Label5: TLabel
      Left = 350
      Top = 164
      ExplicitLeft = 350
      ExplicitTop = 164
    end
    inherited lblTime: TLabel
      Left = 458
      Top = 164
      ExplicitLeft = 458
      ExplicitTop = 164
    end
    object btnSave: TSpeedButton [2]
      Left = 437
      Top = 183
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
    end
    object btnLoad: TSpeedButton [3]
      Left = 73
      Top = 182
      Width = 74
      Height = 25
      Hint = 'Load a solution from file'
      Caption = 'Load'
      Enabled = False
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
    object lblStatus: TLabel [4]
      Left = 98
      Top = 164
      Width = 215
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'lblStatus'
    end
    inherited btnRun: TBitBtn
      Left = 318
      Top = 183
      OnClick = btnRunClick
      ExplicitLeft = 318
      ExplicitTop = 183
    end
    inherited btnClose: TBitBtn
      Left = 531
      Top = 183
      ExplicitLeft = 531
      ExplicitTop = 183
    end
    object pgctrlSSPR: TPageControl
      Left = 1
      Top = 1
      Width = 849
      Height = 157
      ActivePage = tsSSPRRun
      Align = alTop
      TabOrder = 2
      object tsSSPRNbrTest: TTabSheet
        Caption = 'Testing N1, N2, B1 && B2'
        object Label15: TLabel
          Left = 55
          Top = 73
          Width = 46
          Height = 13
          Caption = 'Operation'
        end
        object Label7: TLabel
          Left = 549
          Top = 73
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label6: TLabel
          Left = 462
          Top = 73
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label1: TLabel
          Left = 347
          Top = 72
          Width = 41
          Height = 13
          Caption = 'Machine'
        end
        object Label10: TLabel
          Left = 549
          Top = 30
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label9: TLabel
          Left = 462
          Top = 32
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label2: TLabel
          Left = 347
          Top = 30
          Width = 17
          Height = 13
          Caption = 'Job'
        end
        object cbxOps: TComboBox
          Left = 111
          Top = 70
          Width = 61
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 0
        end
        object btnNSParameters: TBitBtn
          Left = 192
          Top = 68
          Width = 116
          Height = 25
          Caption = 'NS Parameters'
          DoubleBuffered = True
          Enabled = False
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
          OnClick = btnNSParametersClick
        end
        object edtMCx2: TEdit
          Left = 575
          Top = 70
          Width = 55
          Height = 21
          Enabled = False
          TabOrder = 2
          Text = 'edtMCx2'
        end
        object edtMCx1: TEdit
          Left = 490
          Top = 70
          Width = 53
          Height = 21
          Enabled = False
          TabOrder = 3
          Text = 'edtMCx1'
        end
        object cbxFromMC: TComboBox
          Left = 399
          Top = 69
          Width = 57
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 4
        end
        object edtJobx2: TEdit
          Left = 575
          Top = 27
          Width = 55
          Height = 21
          Enabled = False
          TabOrder = 5
          Text = 'edtJobx2'
        end
        object edtJobx1: TEdit
          Left = 490
          Top = 29
          Width = 53
          Height = 21
          Enabled = False
          TabOrder = 6
          Text = 'edtJobx1'
        end
        object cbxJobs: TComboBox
          Left = 399
          Top = 29
          Width = 57
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 7
        end
        object btnB1Parameters: TBitBtn
          Left = 645
          Top = 27
          Width = 116
          Height = 25
          Caption = 'B1 Parameters'
          DoubleBuffered = True
          Enabled = False
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
          TabOrder = 8
          OnClick = btnB1ParametersClick
        end
        object btnB2Parameters: TBitBtn
          Left = 645
          Top = 67
          Width = 116
          Height = 25
          Caption = 'B2 Parameters'
          DoubleBuffered = True
          Enabled = False
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
          TabOrder = 9
          OnClick = btnB2ParametersClick
        end
        object btnEnumerate: TBitBtn
          Left = 93
          Top = 25
          Width = 172
          Height = 25
          Caption = 'Enumerate N1 && N2 moves'
          DoubleBuffered = True
          Enabled = False
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
          TabOrder = 10
          OnClick = btnEnumerateClick
        end
      end
      object tsImprovementTest: TTabSheet
        Caption = 'Testing Improvement'
        ImageIndex = 2
        object Label3: TLabel
          Left = 168
          Top = 8
          Width = 163
          Height = 13
          Caption = 'Best NS moves for current solution'
        end
        object Label4: TLabel
          Left = 560
          Top = 8
          Width = 133
          Height = 13
          Caption = 'History of applied NS moves'
        end
        object btnStartNSTest: TSpeedButton
          Left = 32
          Top = 48
          Width = 89
          Height = 25
          Caption = 'Start'
          Enabled = False
          OnClick = btnStartNSTestClick
        end
        object btnApplyNSMove: TSpeedButton
          Left = 440
          Top = 48
          Width = 89
          Height = 25
          Caption = 'Apply Move'
          OnClick = btnApplyNSMoveClick
        end
        object lstCurrentNSMoves: TListBox
          Left = 168
          Top = 27
          Width = 242
          Height = 89
          ItemHeight = 13
          TabOrder = 0
        end
        object lstAppliedNSMoves: TListBox
          Left = 560
          Top = 27
          Width = 252
          Height = 89
          ItemHeight = 13
          TabOrder = 1
        end
        object chkStrictImprovement: TCheckBox
          Left = 25
          Top = 17
          Width = 121
          Height = 17
          Caption = 'Strict Improvement'
          TabOrder = 2
        end
      end
      object tsSSPRLB: TTabSheet
        Caption = 'Lower Bound'
        ImageIndex = 3
        object btnLB: TSpeedButton
          Left = 88
          Top = 40
          Width = 145
          Height = 33
          Caption = 'Evaluate Lower bound'
          OnClick = btnLBClick
        end
        object Label8: TLabel
          Left = 324
          Top = 25
          Width = 31
          Height = 13
          Caption = 'LB1 = '
        end
        object Label11: TLabel
          Left = 324
          Top = 53
          Width = 31
          Height = 13
          Caption = 'LB2 = '
        end
        object lblLB1: TLabel
          Left = 376
          Top = 25
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB1'
        end
        object lblLB2: TLabel
          Left = 376
          Top = 53
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB2'
        end
        object Label12: TLabel
          Left = 506
          Top = 53
          Width = 25
          Height = 13
          Caption = 'LB = '
        end
        object lblLB: TLabel
          Left = 558
          Top = 53
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB'
        end
        object lblLB3: TLabel
          Left = 376
          Top = 83
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB3'
        end
        object Label14: TLabel
          Left = 324
          Top = 83
          Width = 31
          Height = 13
          Caption = 'LB3 = '
        end
      end
      object tsTabuSearch: TTabSheet
        Caption = 'Testing Tabu Search'
        ImageIndex = 4
        object Label20: TLabel
          Left = 17
          Top = 22
          Width = 46
          Height = 13
          Caption = 'Tabu size'
        end
        object Label21: TLabel
          Left = 49
          Top = 89
          Width = 25
          Height = 13
          Caption = 'LB = '
        end
        object lblTStestLB: TLabel
          Left = 87
          Top = 89
          Width = 50
          Height = 13
          AutoSize = False
          Caption = 'lblLB'
        end
        object btnTStestStart: TSpeedButton
          Left = 40
          Top = 51
          Width = 89
          Height = 25
          Caption = 'Start'
          Enabled = False
          OnClick = btnTStestStartClick
        end
        object Label22: TLabel
          Left = 174
          Top = 8
          Width = 208
          Height = 13
          Caption = 'Best non-tabu NS moves for current solution'
        end
        object btnTStestApplyMove: TSpeedButton
          Left = 441
          Top = 77
          Width = 89
          Height = 25
          Caption = 'Apply Move'
          OnClick = btnTStestApplyMoveClick
        end
        object Label23: TLabel
          Left = 550
          Top = 8
          Width = 104
          Height = 13
          Caption = 'List of tabu NS moves'
        end
        object btnTStestUpdateMoves: TSpeedButton
          Left = 441
          Top = 43
          Width = 89
          Height = 25
          Caption = 'Update Moves'
          OnClick = btnTStestUpdateMovesClick
        end
        object edtTStestTabuSize: TEdit
          Left = 72
          Top = 20
          Width = 41
          Height = 21
          TabOrder = 0
          Text = '5'
        end
        object lstCurrentnonTabuNSMoves: TListBox
          Left = 174
          Top = 25
          Width = 252
          Height = 89
          ItemHeight = 13
          TabOrder = 1
        end
        object lstTabuNSMoves: TListBox
          Left = 544
          Top = 25
          Width = 281
          Height = 89
          ItemHeight = 13
          TabOrder = 2
        end
        object btnSuggestTabuSize: TButton
          Left = 127
          Top = 17
          Width = 29
          Height = 25
          Hint = 'Suggest a value for acceptance threshold'
          Caption = '<--'
          TabOrder = 3
          OnClick = btnSuggestTabuSizeClick
        end
      end
      object tsSSPRRun: TTabSheet
        Caption = 'SSPR Run'
        ImageIndex = 1
        object Label13: TLabel
          Left = 347
          Top = 8
          Width = 73
          Height = 13
          Caption = 'Population Size'
        end
        object Label16: TLabel
          Left = 267
          Top = 34
          Width = 153
          Height = 13
          Caption = 'Distance Acceptance Threshold'
        end
        object Label17: TLabel
          Left = 331
          Top = 60
          Width = 89
          Height = 13
          Caption = 'Maximum tabu size'
        end
        object Label18: TLabel
          Left = 264
          Top = 87
          Width = 156
          Height = 13
          Caption = 'Maximum number of TS iterations'
        end
        object Label19: TLabel
          Left = 564
          Top = 34
          Width = 126
          Height = 13
          Caption = 'Improvement iterations limit'
        end
        object Label24: TLabel
          Left = 552
          Top = 60
          Width = 138
          Height = 13
          Caption = 'Number of stored best moves'
        end
        object Label25: TLabel
          Left = 569
          Top = 86
          Width = 121
          Height = 13
          Caption = 'Number of random moves'
        end
        object Label26: TLabel
          Left = 564
          Top = 7
          Width = 126
          Height = 13
          Caption = 'Number of SSPR iterations'
        end
        object Label27: TLabel
          Left = 386
          Top = 109
          Width = 34
          Height = 13
          Caption = 'Epsilon'
        end
        object Label28: TLabel
          Left = 14
          Top = 8
          Width = 137
          Height = 13
          Caption = 'Select treatment combination'
        end
        object edtPopSize: TEdit
          Left = 432
          Top = 5
          Width = 79
          Height = 21
          TabOrder = 0
          Text = 'edtPopSize'
        end
        object edtDeltaThreshold: TEdit
          Left = 432
          Top = 32
          Width = 79
          Height = 21
          TabOrder = 1
          Text = 'edtDeltaThreshold'
        end
        object edtTabuSize: TEdit
          Left = 432
          Top = 56
          Width = 79
          Height = 21
          TabOrder = 2
          Text = 'edtTabuSize'
        end
        object edtnTSIterations: TEdit
          Left = 432
          Top = 82
          Width = 79
          Height = 21
          TabOrder = 3
          Text = 'edtnTSIterations'
        end
        object edtnImpIterations: TEdit
          Left = 703
          Top = 31
          Width = 89
          Height = 21
          TabOrder = 4
          Text = 'edtnImpIterations'
        end
        object edtTSnBestMoves: TEdit
          Left = 703
          Top = 57
          Width = 89
          Height = 21
          TabOrder = 5
          Text = 'edtTSnBestMoves'
        end
        object edtTSnRandomMoves: TEdit
          Left = 703
          Top = 83
          Width = 89
          Height = 21
          TabOrder = 6
          Text = 'edtTSnRandomMoves'
        end
        object edtnSSPRIterations: TEdit
          Left = 703
          Top = 4
          Width = 87
          Height = 21
          TabOrder = 7
          Text = 'edtnSSPRIterations'
        end
        object edtEpsilon: TEdit
          Left = 432
          Top = 107
          Width = 79
          Height = 21
          TabOrder = 8
          Text = 'edtEpsilon'
        end
        object lstTreatmentCombination: TListBox
          Left = 32
          Top = 29
          Width = 177
          Height = 97
          ItemHeight = 13
          Items.Strings = (
            '1:   2 - 3 - 10 - 10 - 10 - 10'
            '2:   2 - 3 - 10 - 10 - 20 - 20'
            '3:   2 - 3 - 10 - 10 - 40 - 40'
            '4:   2 - 5 - 20 - 20 - 10 - 10'
            '5:   2 - 5 - 20 - 20 - 20 - 20'
            '6:   2 - 5 - 20 - 20 - 40 - 40'
            '7:   2 - 8 - 40 - 40 - 10 - 10'
            '8:   2 - 8 - 40 - 40 - 20 - 20'
            '9:   2 - 8 - 40 - 40 - 40 - 40'
            '10: 3 - 3 - 20 - 40 - 10 - 20 '
            '11: 3 - 3 - 20 - 40 - 20 - 40'
            '12: 3 - 3 - 20 - 40 - 40 - 10'
            '13: 3 - 5 - 40 - 10 - 10 - 20'
            '14: 3 - 5 - 40 - 10 - 20 - 40'
            '15: 3 - 5 - 40 - 10 - 40 - 10'
            '16: 3 - 8 - 10 - 20 - 10 - 20'
            '17: 3 - 8 - 10 - 20 - 20 - 40'
            '18: 3 - 8 - 10 - 20 - 40 - 10'
            '19: 4 - 3 - 40 - 20 - 10 - 40'
            '20: 4 - 3 - 40 - 20 - 20 - 10'
            '21: 4 - 3 - 40 - 20 - 40 - 20'
            '22: 4 - 5 - 10 - 40 - 10 - 40'
            '23: 4 - 5 - 10 - 40 - 20 - 10'
            '24: 4 - 5 - 10 - 40 - 40 - 20'
            '25: 4 - 8 - 20 - 10 - 10 - 40'
            '26: 4 - 8 - 20 - 10 - 20 - 10'
            '27: 4 - 8 - 20 - 10 - 40 - 20')
          TabOrder = 9
          OnClick = lstTreatmentCombinationClick
        end
      end
    end
    object btnConstruct: TButton
      Left = 173
      Top = 183
      Width = 111
      Height = 25
      Caption = 'Test Construction'
      TabOrder = 3
      OnClick = btnConstructClick
    end
  end
end
