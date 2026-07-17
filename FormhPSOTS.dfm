inherited frmhPSOTS: TfrmhPSOTS
  Caption = 
    'Hybrid particle swarm optimization - tabu search approach (hPSOT' +
    'S)'
  ExplicitWidth = 320
  ExplicitHeight = 240
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Top = 195
    Height = 459
    ExplicitTop = 195
    ExplicitHeight = 459
  end
  inherited pnlGTAParameters: TPanel
    Height = 195
    ExplicitHeight = 195
    inherited Label5: TLabel
      Left = 353
      Top = 140
      ExplicitLeft = 353
      ExplicitTop = 140
    end
    inherited lblTime: TLabel
      Left = 461
      Top = 140
      ExplicitLeft = 461
      ExplicitTop = 140
    end
    inherited Label1: TLabel
      Left = 86
      Top = 80
      Width = 156
      Caption = 'Maximum number of TS iterations'
      ExplicitLeft = 86
      ExplicitTop = 80
      ExplicitWidth = 156
    end
    inherited Label2: TLabel
      Left = 36
      Top = 108
      ExplicitLeft = 36
      ExplicitTop = 108
    end
    inherited Label3: TLabel
      Left = 528
      Top = 108
      ExplicitLeft = 528
      ExplicitTop = 108
    end
    inherited Label4: TLabel
      Left = 415
      Top = 76
      ExplicitLeft = 415
      ExplicitTop = 76
    end
    object Label6: TLabel [6]
      Left = 95
      Top = 16
      Width = 164
      Height = 13
      Caption = 'Maximum number of PSO iterations'
    end
    object Label7: TLabel [7]
      Left = 543
      Top = 16
      Width = 91
      Height = 13
      Caption = 'Number of particles'
    end
    object Label8: TLabel [8]
      Left = 39
      Top = 47
      Width = 34
      Height = 13
      Caption = 'Omega'
    end
    object Label9: TLabel [9]
      Left = 240
      Top = 47
      Width = 35
      Height = 13
      Caption = 'delta_0'
    end
    object Label10: TLabel [10]
      Left = 640
      Top = 47
      Width = 18
      Height = 13
      Caption = 'c_2'
    end
    object Label11: TLabel [11]
      Left = 448
      Top = 47
      Width = 18
      Height = 13
      Caption = 'c_1'
    end
    inherited btnRun: TBitBtn
      Left = 273
      Top = 159
      TabOrder = 10
      ExplicitLeft = 273
      ExplicitTop = 159
    end
    inherited btnClose: TBitBtn
      Left = 505
      Top = 159
      TabOrder = 11
      ExplicitLeft = 505
      ExplicitTop = 159
    end
    inherited edtnIterations: TEdit
      Left = 256
      Top = 77
      TabOrder = 6
      ExplicitLeft = 256
      ExplicitTop = 77
    end
    inherited edtMaxTime: TEdit
      Left = 256
      Top = 105
      TabOrder = 8
      ExplicitLeft = 256
      ExplicitTop = 105
    end
    inherited edtTabuSize: TEdit
      Left = 630
      Top = 105
      TabOrder = 9
      ExplicitLeft = 630
      ExplicitTop = 105
    end
    inherited edtnImpIterations: TEdit
      Left = 630
      Top = 73
      TabOrder = 7
      ExplicitLeft = 630
      ExplicitTop = 73
    end
    object edtnPSOIterations: TEdit
      Left = 273
      Top = 13
      Width = 121
      Height = 21
      TabOrder = 0
      Text = 'edtnPSOIterations'
    end
    object edtnParticles: TEdit
      Left = 647
      Top = 13
      Width = 121
      Height = 21
      TabOrder = 1
      Text = 'edtnParticles'
    end
    object edtOmega: TEdit
      Left = 87
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 2
      Text = 'edtOmega'
    end
    object edtDelta0: TEdit
      Left = 289
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 3
      Text = 'edtDelta0'
    end
    object edtc2: TEdit
      Left = 671
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 5
      Text = 'edtc2'
    end
    object edtc1: TEdit
      Left = 479
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 4
      Text = 'edtc1'
    end
  end
end
