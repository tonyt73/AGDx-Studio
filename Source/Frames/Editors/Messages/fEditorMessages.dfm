inherited frmEditorMessages: TfrmEditorMessages
  Width = 1204
  Height = 824
  Font.Height = -13
  Font.Name = 'Roboto'
  ParentFont = False
  OnResize = FrameResize
  ExplicitWidth = 1204
  ExplicitHeight = 824
  object panCharacterSet: TPanel
    Left = 0
    Top = 0
    Width = 1204
    Height = 61
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    TabOrder = 0
    inline fFramesView: TMultiImageViewFrame
      Left = 0
      Top = 0
      Width = 1204
      Height = 61
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      HorzScrollBar.ParentColor = False
      HorzScrollBar.Smooth = True
      HorzScrollBar.Style = ssHotTrack
      HorzScrollBar.Tracking = True
      VertScrollBar.Tracking = True
      VertScrollBar.Visible = False
      Align = alTop
      AutoScroll = True
      DoubleBuffered = True
      Ctl3D = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Roboto'
      Font.Style = []
      ParentCtl3D = False
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      ExplicitWidth = 1204
      inherited panImages: TPanel
        Width = 225
        Height = 50
        StyleElements = [seFont, seClient, seBorder]
        ExplicitWidth = 225
        ExplicitHeight = 50
      end
    end
  end
  object panContainer: TPanel
    Left = 0
    Top = 61
    Width = 1204
    Height = 763
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object Splitter1: TSplitter
      Left = 521
      Top = 0
      Width = 6
      Height = 763
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      MinSize = 38
      ResizeStyle = rsUpdate
    end
    object Panel1: TPanel
      Left = 0
      Top = 0
      Width = 521
      Height = 763
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alLeft
      BevelOuter = bvNone
      Constraints.MinWidth = 200
      TabOrder = 0
      inline ecMessageEditor: TfrmEditorCode
        Left = 0
        Top = 0
        Width = 521
        Height = 763
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Roboto'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnEnter = FrameEnter
        ExplicitWidth = 521
        ExplicitHeight = 763
        inherited evEditor: TLMDEditView
          Width = 521
          Height = 704
          CompletionSettings.ItemHeight = 14
          OnStatusChanged = OnStatusChanged
          ExplicitWidth = 521
          ExplicitHeight = 704
          GutterBarsEmpty = False
        end
        inherited sbStatus: TStatusBar
          Top = 744
          Width = 521
          ExplicitTop = 744
          ExplicitWidth = 521
        end
        inherited ToolBar1: TToolBar
          Width = 521
          ExplicitWidth = 521
        end
        inherited lmdDocument: TLMDEditDocument
          ActiveSyntaxScheme = 'TXT'
        end
        inherited vilToolbar: TVirtualImageList
          Width = 30
          Height = 30
        end
      end
    end
    object Panel2: TPanel
      Left = 527
      Top = 0
      Width = 677
      Height = 763
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alClient
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 1
      object sbxView: TScrollBox
        Left = 0
        Top = 0
        Width = 677
        Height = 763
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        HorzScrollBar.Smooth = True
        HorzScrollBar.Tracking = True
        VertScrollBar.Smooth = True
        VertScrollBar.Tracking = True
        Align = alClient
        DoubleBuffered = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'Small Fonts'
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 0
        OnMouseWheel = sbxViewMouseWheel
        object imgView: TImage
          Left = -2
          Top = -2
          Width = 523
          Height = 555
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Stretch = True
        end
      end
    end
  end
  object tmrRefreshView: TTimer
    Interval = 100
    OnTimer = tmrRefreshViewTimer
    Left = 588
    Top = 400
  end
end
