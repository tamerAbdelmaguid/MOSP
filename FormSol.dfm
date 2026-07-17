object frmSolution: TfrmSolution
  Left = 169
  Top = 173
  Anchors = []
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 484
  ClientWidth = 688
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000000D
    DDDDD0CCCCCCCC09999999999000000DDDDDD0CCCCCCCC09999999999000000D
    DDDDD0CCCCCCCC09999999999000000DDDDDD0CCCCCCCC099999999990000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000009
    999990DDDDDDDDD0CCCCCCC000000009999990DDDDDDDDD0CCCCCCC000000009
    999990DDDDDDDDD0CCCCCCC000000009999990DDDDDDDDD0CCCCCCC000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000FFFF
    FFFFFFFFFFFFFFFFFFFFDFFFFFFF80000003DFFFFFFFDFFFFFFFDFFFFFFFDFFF
    FFFFDFFFFFFFC0402007C0402007C0402007C0402007DFFFFFFFDFFFFFFFDFFF
    FFFFDFFFFFFFDFFFFFFFDFFFFFFFC040101FC040101FC040101FC040101FDFFF
    FFFFDFFFFFFFDFFFFFFFDFFFFFFFDFFFFFFFDFFFFFFFFFFFFFFFFFFFFFFF}
  OldCreateOrder = False
  Position = poDefault
  WindowState = wsMaximized
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pgSolution: TPageControl
    Left = 0
    Top = 0
    Width = 688
    Height = 381
    ActivePage = tbTabular
    Align = alClient
    BiDiMode = bdLeftToRight
    ParentBiDiMode = False
    TabOrder = 0
    object tbGantt: TTabSheet
      Caption = 'Gantt Chart'
      object chrtJSSPGantt: TChart
        Left = 0
        Top = 0
        Width = 680
        Height = 353
        BackWall.Brush.Color = clWhite
        BackWall.Brush.Style = bsClear
        Gradient.EndColor = 8454143
        Legend.DividingLines.Visible = True
        Legend.LegendStyle = lsValues
        Legend.TextStyle = ltsPlain
        Legend.Visible = False
        Title.AdjustFrame = False
        Title.Text.Strings = (
          '')
        Title.Visible = False
        LeftAxis.Axis.Width = 1
        LeftAxis.Axis.Visible = False
        LeftAxis.Grid.SmallDots = True
        LeftAxis.LabelsSeparation = 0
        LeftAxis.LabelStyle = talText
        LeftAxis.MinorTickCount = 0
        LeftAxis.MinorTickLength = 0
        LeftAxis.StartPosition = 7.000000000000000000
        LeftAxis.EndPosition = 93.000000000000000000
        LeftAxis.TickLength = 5
        RightAxis.LabelStyle = talText
        View3D = False
        Align = alClient
        BevelOuter = bvNone
        Color = clWhite
        PopupMenu = mnuGANTT
        TabOrder = 0
        AutoSize = True
        PrintMargins = (
          15
          24
          15
          24)
        object Gantt: TGanttSeries
          Marks.Arrow.Visible = True
          Marks.Callout.Brush.Color = clBlack
          Marks.Callout.Arrow.Visible = True
          Marks.Clip = True
          Marks.Frame.Visible = False
          Marks.Transparent = True
          Marks.Visible = False
          SeriesColor = clWhite
          ShowInLegend = False
          Title = 'Gantt'
          ClickableLine = False
          Pointer.HorizSize = 10
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.VertSize = 8
          Pointer.Visible = True
          XValues.DateTime = False
          XValues.Name = 'Start'
          XValues.Order = loAscending
          YValues.Name = 'Y'
          YValues.Order = loNone
          ConnectingPen.Visible = False
          StartValues.DateTime = False
          StartValues.Name = 'Start'
          StartValues.Order = loAscending
          EndValues.Name = 'End'
          EndValues.Order = loNone
          NextTask.Name = 'NextTask'
          NextTask.Order = loNone
        end
        object CPaths: TArrowSeries
          Marks.Arrow.Visible = True
          Marks.Callout.Brush.Color = clBlack
          Marks.Callout.Arrow.Visible = True
          Marks.Frame.Visible = False
          Marks.Transparent = True
          Marks.Visible = False
          Title = 'CPaths'
          ClickableLine = False
          Pointer.InflateMargins = False
          Pointer.Style = psRectangle
          Pointer.Visible = True
          XValues.Name = 'X'
          XValues.Order = loAscending
          YValues.Name = 'Y'
          YValues.Order = loNone
          EndXValues.Name = 'EndX'
          EndXValues.Order = loNone
          EndYValues.Name = 'EndY'
          EndYValues.Order = loNone
          StartXValues.Name = 'X'
          StartXValues.Order = loAscending
          StartYValues.Name = 'Y'
          StartYValues.Order = loNone
        end
      end
    end
    object tbTabular: TTabSheet
      Caption = 'Tabular Representation'
      ImageIndex = 1
      object pbTables: TPageControl
        Left = 0
        Top = 0
        Width = 680
        Height = 353
        ActivePage = tbNetwork
        Align = alClient
        TabOrder = 0
        object tsMachines: TTabSheet
          Caption = 'Machines'
          object Label2: TLabel
            Left = 16
            Top = 24
            Width = 77
            Height = 13
            Caption = 'List of Machines'
          end
          object Label5: TLabel
            Left = 160
            Top = 24
            Width = 278
            Height = 13
            Caption = 'Detailes of scheduled operations on the selected machines'
          end
          object grdMachineOps: TStringGrid
            Left = 160
            Top = 48
            Width = 489
            Height = 249
            Color = clWhite
            ColCount = 4
            DefaultColWidth = 115
            FixedColor = clScrollBar
            FixedCols = 0
            TabOrder = 0
          end
          object lstMachines: TListBox
            Left = 16
            Top = 48
            Width = 121
            Height = 257
            ItemHeight = 13
            TabOrder = 1
            OnClick = lstMachinesClick
          end
        end
        object tsJobs: TTabSheet
          Caption = 'Jobs'
          ImageIndex = 1
          object Label8: TLabel
            Left = 16
            Top = 24
            Width = 53
            Height = 13
            Caption = 'List of Jobs'
          end
          object Label9: TLabel
            Left = 160
            Top = 24
            Width = 244
            Height = 13
            Caption = 'Detailes of scheduled operations of the selected job'
          end
          object lstJobs: TListBox
            Left = 16
            Top = 48
            Width = 121
            Height = 257
            ItemHeight = 13
            TabOrder = 0
            OnClick = lstJobsClick
          end
          object grdJobOps: TStringGrid
            Left = 160
            Top = 48
            Width = 489
            Height = 249
            Color = clWhite
            ColCount = 4
            DefaultColWidth = 115
            FixedColor = clScrollBar
            FixedCols = 0
            TabOrder = 1
          end
        end
        object tbNetwork: TTabSheet
          Caption = 'Network'
          ImageIndex = 2
          object Label11: TLabel
            Left = 56
            Top = 8
            Width = 27
            Height = 13
            Caption = 'Jobs'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label14: TLabel
            Left = 368
            Top = 8
            Width = 55
            Height = 13
            Caption = 'Machines'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label15: TLabel
            Left = 24
            Top = 152
            Width = 48
            Height = 13
            Caption = 'Network'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object grdJobs: TStringGrid
            Left = 56
            Top = 24
            Width = 241
            Height = 113
            ColCount = 3
            TabOrder = 0
            ColWidths = (
              64
              74
              76)
          end
          object grdMachines: TStringGrid
            Left = 368
            Top = 24
            Width = 241
            Height = 113
            ColCount = 3
            TabOrder = 1
            ColWidths = (
              64
              74
              76)
            RowHeights = (
              24
              24
              24
              24
              24)
          end
          object grdNetwork: TStringGrid
            Left = 17
            Top = 168
            Width = 624
            Height = 154
            ColCount = 12
            TabOrder = 2
            ColWidths = (
              64
              48
              48
              47
              45
              47
              47
              51
              49
              50
              48
              38)
          end
        end
      end
    end
  end
  object pnlOFs: TPanel
    Left = 0
    Top = 381
    Width = 688
    Height = 103
    Align = alBottom
    BevelOuter = bvLowered
    TabOrder = 1
    object Label1: TLabel
      Left = 44
      Top = 9
      Width = 59
      Height = 13
      Alignment = taRightJustify
      Caption = 'Makespan ='
    end
    object lblMakespan: TLabel
      Left = 113
      Top = 9
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMakespan'
      Color = clHighlightText
      ParentColor = False
    end
    object lblMeanFlowTime: TLabel
      Left = 113
      Top = 29
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMeanFlowTime'
      Color = clHighlightText
      ParentColor = False
    end
    object Label4: TLabel
      Left = 23
      Top = 29
      Width = 80
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean flow time ='
    end
    object lblMeanLateness: TLabel
      Left = 113
      Top = 49
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMeanLateness'
      Color = clHighlightText
      ParentColor = False
    end
    object Label6: TLabel
      Left = 25
      Top = 49
      Width = 78
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean lateness ='
    end
    object Label7: TLabel
      Left = 21
      Top = 71
      Width = 81
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean tardiness ='
    end
    object lblMeanTardiness: TLabel
      Left = 112
      Top = 71
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMeanTardiness'
      Color = clHighlightText
      ParentColor = False
    end
    object lblMaxLateness: TLabel
      Left = 344
      Top = 12
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMaxLateness'
      Color = clHighlightText
      ParentColor = False
    end
    object Label10: TLabel
      Left = 239
      Top = 12
      Width = 95
      Height = 13
      Alignment = taRightJustify
      Caption = 'Maximum lateness ='
    end
    object lblMaxTardiness: TLabel
      Left = 344
      Top = 32
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMaxTardiness'
      Color = clHighlightText
      ParentColor = False
    end
    object Label12: TLabel
      Left = 236
      Top = 32
      Width = 98
      Height = 13
      Alignment = taRightJustify
      Caption = 'Maximum tardiness ='
    end
    object Label13: TLabel
      Left = 228
      Top = 52
      Width = 106
      Height = 13
      Alignment = taRightJustify
      Caption = 'Number of tardy jobs ='
    end
    object lblNTardyJobs: TLabel
      Left = 344
      Top = 52
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblNTardyJobs'
      Color = clHighlightText
      ParentColor = False
    end
    object lblMWLateness: TLabel
      Left = 344
      Top = 72
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMWLateness'
      Color = clHighlightText
      ParentColor = False
    end
    object Label16: TLabel
      Left = 210
      Top = 72
      Width = 124
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean weighted lateness ='
    end
    object lblMWTardiness: TLabel
      Left = 586
      Top = 13
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMWTardiness'
      Color = clHighlightText
      ParentColor = False
    end
    object Label18: TLabel
      Left = 449
      Top = 13
      Width = 127
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean weighted tardiness ='
    end
    object lblMWFlowTime: TLabel
      Left = 586
      Top = 33
      Width = 80
      Height = 13
      AutoSize = False
      Caption = 'lblMWFlowTime'
      Color = clHighlightText
      ParentColor = False
    end
    object Label3: TLabel
      Left = 450
      Top = 33
      Width = 126
      Height = 13
      Alignment = taRightJustify
      Caption = 'Mean weighted flow time ='
    end
    object btnSaveSol: TSpeedButton
      Left = 640
      Top = 59
      Width = 28
      Height = 28
      Hint = 'Save Current Solution'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        333333FFFFFFFFFFFFF33000077777770033377777777777773F000007888888
        00037F3337F3FF37F37F00000780088800037F3337F77F37F37F000007800888
        00037F3337F77FF7F37F00000788888800037F3337777777337F000000000000
        00037F3FFFFFFFFFFF7F00000000000000037F77777777777F7F000FFFFFFFFF
        00037F7F333333337F7F000FFFFFFFFF00037F7F333333337F7F000FFFFFFFFF
        00037F7F333333337F7F000FFFFFFFFF00037F7F333333337F7F000FFFFFFFFF
        00037F7F333333337F7F000FFFFFFFFF07037F7F33333333777F000FFFFFFFFF
        0003737FFFFFFFFF7F7330099999999900333777777777777733}
      NumGlyphs = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = btnSaveSolClick
    end
    object btnLoadSol: TSpeedButton
      Left = 608
      Top = 59
      Width = 28
      Height = 28
      Hint = 'Load a solution from file'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
        5555555555555555555555555555555555555555555555555555555555555555
        555555555555555555555555555555555555555FFFFFFFFFF555550000000000
        55555577777777775F55500B8B8B8B8B05555775F555555575F550F0B8B8B8B8
        B05557F75F555555575F50BF0B8B8B8B8B0557F575FFFFFFFF7F50FBF0000000
        000557F557777777777550BFBFBFBFB0555557F555555557F55550FBFBFBFBF0
        555557F555555FF7555550BFBFBF00055555575F555577755555550BFBF05555
        55555575FFF75555555555700007555555555557777555555555555555555555
        5555555555555555555555555555555555555555555555555555}
      NumGlyphs = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = btnLoadSolClick
    end
  end
  object dlgSaveSol: TSaveDialog
    Filter = 'MOSP Solution (*.mosp.sol)|*.mosp.sol|All files (*.*)|*.*'
    Left = 544
    Top = 437
  end
  object dlgLoadSol: TOpenDialog
    Filter = 'JSSP Solution (*.mosp.sol)|*.mosp.sol|All files (*.*)|*.*'
    Left = 488
    Top = 437
  end
  object mnuGANTT: TPopupMenu
    Left = 432
    Top = 440
    object ExportWMF1: TMenuItem
      Caption = 'Export WMF...'
      OnClick = ExportWMF1Click
    end
  end
  object dlgSaveWMF: TSaveDialog
    Filter = 'Windows metafile|*.wmf'
    Left = 424
    Top = 392
  end
end
