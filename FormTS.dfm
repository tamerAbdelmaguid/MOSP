inherited frmTS: TfrmTS
  Caption = 'Tabu Search based on Abdelmaguid, Shalaby and Awwad (2014)'
  ClientHeight = 654
  ClientWidth = 838
  ExplicitWidth = 854
  ExplicitHeight = 690
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Top = 153
    Width = 838
    Height = 501
    ExplicitTop = 153
    ExplicitWidth = 838
    ExplicitHeight = 501
  end
  inherited pnlGTAParameters: TPanel
    Width = 838
    Height = 153
    ExplicitWidth = 838
    ExplicitHeight = 153
    inherited Label5: TLabel
      Left = 370
      Top = 81
      ExplicitLeft = 370
      ExplicitTop = 81
    end
    inherited lblTime: TLabel
      Left = 478
      Top = 81
      ExplicitLeft = 478
      ExplicitTop = 81
    end
    object Label1: TLabel [2]
      Left = 120
      Top = 16
      Width = 139
      Height = 13
      Caption = 'Maximum number of iterations'
    end
    object Label2: TLabel [3]
      Left = 53
      Top = 44
      Width = 206
      Height = 13
      Caption = 'Maximum computational time (Milli Seconds)'
    end
    object Label3: TLabel [4]
      Left = 545
      Top = 44
      Width = 89
      Height = 13
      Caption = 'Maximum tabu size'
    end
    object Label4: TLabel [5]
      Left = 432
      Top = 17
      Width = 202
      Height = 13
      Caption = 'Maximum number of improvement iterations'
    end
    inherited btnRun: TBitBtn
      Left = 288
      Top = 111
      OnClick = btnRunClick
      ExplicitLeft = 288
      ExplicitTop = 111
    end
    inherited btnClose: TBitBtn
      Left = 520
      Top = 111
      ExplicitLeft = 520
      ExplicitTop = 111
    end
    object edtnIterations: TEdit
      Left = 273
      Top = 13
      Width = 121
      Height = 21
      TabOrder = 2
      Text = 'edtnIterations'
    end
    object edtMaxTime: TEdit
      Left = 273
      Top = 41
      Width = 121
      Height = 21
      TabOrder = 3
      Text = 'edtMaxTime'
    end
    object edtTabuSize: TEdit
      Left = 647
      Top = 41
      Width = 121
      Height = 21
      TabOrder = 4
      Text = 'edtTabuSize'
    end
    object edtnImpIterations: TEdit
      Left = 647
      Top = 14
      Width = 121
      Height = 21
      TabOrder = 5
      Text = 'edtnImpIterations'
    end
  end
end
