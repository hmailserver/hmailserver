VERSION 5.00
Begin VB.UserControl PageSelectFile 
   ClientHeight    =   1050
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   4965
   ScaleHeight     =   1050
   ScaleWidth      =   4965
   Begin VB.CommandButton cmdBrowse 
      Caption         =   "..."
      Height          =   285
      Left            =   3840
      TabIndex        =   2
      Top             =   360
      Width           =   735
   End
   Begin VB.TextBox txtTextFile 
      Height          =   285
      Left            =   120
      TabIndex        =   1
      Top             =   360
      Width           =   3615
   End
   Begin VB.Label lblFile 
      Caption         =   "Text file:"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3615
   End
End
Attribute VB_Name = "PageSelectFile"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Implements IWizardPage

Private m_oWizard As Wizard

Private Function IWizardPage_Initialize(oWizard As Wizard) As Variant
   Set m_oWizard = oWizard
End Function

Private Sub cmdBrowse_Click()
   txtTextFile.Text = ShowFileOpen(frmMigrate, "*.txt")
End Sub

Private Function IWizardPage_Caption() As String
   IWizardPage_Caption = "Select file to import"
End Function



Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean
   If txtTextFile.Text = "" Then
      MsgBox "You must enter the name of the file to import."
      IWizardPage_OnLeavePage = True
   End If
   
   g_sTextFile = txtTextFile.Text
End Function

Private Function IWizardPage_OnShowPage() As Boolean

End Function

