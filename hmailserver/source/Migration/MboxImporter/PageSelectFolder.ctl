VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.UserControl PageSelectFolder 
   ClientHeight    =   3570
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   6945
   ScaleHeight     =   3570
   ScaleWidth      =   6945
   Begin MSComctlLib.ListView lvwFiles 
      Height          =   2535
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   6375
      _ExtentX        =   11245
      _ExtentY        =   4471
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      FullRowSelect   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   14737632
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   2
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "File"
         Object.Width           =   7056
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "IMAP folder"
         Object.Width           =   2540
      EndProperty
   End
   Begin VB.CommandButton cmdSelectDirectory 
      Caption         =   "&Select directory"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   1815
   End
End
Attribute VB_Name = "PageSelectFolder"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Implements IWizardPage

Private m_oWizard As Wizard

Private Sub cmdSelectDirectory_Click()
   Dim sFolderName As String
   sFolderName = BrowseForFolder(frmMigrate.hwnd, "Select folder to import")
   
   ShowFileList sFolderName
End Sub

Private Sub ShowFileList(sFolder As String)
   lvwFiles.ListItems.Clear
   
   If Right(sFolder, 1) <> "\" Then
      sFolder = sFolder & "\"
   End If
   
   Dim sName As String
   sName = Dir(sFolder & "*.*")
   
   While Len(sName) > 0
      
      Dim obItem As ListItem
      Set obItem = lvwFiles.ListItems.Add(, , sFolder & sName)
      obItem.SubItems(1) = sName
      
      sName = Dir()
   Wend
   
   g_sSourcePath = sFolder
   
End Sub

Private Function IWizardPage_Initialize(oWizard As Wizard) As Variant
   Set m_oWizard = oWizard
   
End Function

Private Sub cmdBrowse_Click()
   
End Sub

Private Function IWizardPage_Caption() As String
   IWizardPage_Caption = "Select folder containing mbox files"
End Function



Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean

   If g_sSourcePath = "" Then
      MsgBox "You must select source folder.", vbOKOnly
      IWizardPage_OnLeavePage = True
      Exit Function
   End If
   
End Function

Private Function IWizardPage_OnShowPage() As Boolean

End Function


