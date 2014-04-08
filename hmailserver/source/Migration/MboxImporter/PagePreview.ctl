VERSION 5.00
Begin VB.UserControl PagePreview 
   ClientHeight    =   3360
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   8325
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ScaleHeight     =   3360
   ScaleWidth      =   8325
   Begin VB.Label lblSourceDirectory 
      Caption         =   "-"
      Height          =   255
      Left            =   360
      TabIndex        =   5
      Top             =   480
      Width           =   3015
   End
   Begin VB.Label lblSourceDirLabel 
      Caption         =   "Source directory"
      Height          =   255
      Left            =   360
      TabIndex        =   4
      Top             =   240
      Width           =   3015
   End
   Begin VB.Label lblToAccount 
      Caption         =   "-"
      Height          =   255
      Left            =   360
      TabIndex        =   3
      Top             =   2280
      Width           =   3015
   End
   Begin VB.Label lblToAccountLabel 
      Caption         =   "To account:"
      Height          =   255
      Left            =   360
      TabIndex        =   2
      Top             =   2040
      Width           =   3015
   End
   Begin VB.Label lblNoOfFiles 
      Caption         =   "-"
      Height          =   255
      Left            =   360
      TabIndex        =   1
      Top             =   1440
      Width           =   2895
   End
   Begin VB.Label lblNumberOfFiles 
      Caption         =   "Number of files:"
      Height          =   255
      Left            =   360
      TabIndex        =   0
      Top             =   1200
      Width           =   2895
   End
End
Attribute VB_Name = "PagePreview"
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

Private Function IWizardPage_Caption() As String
   IWizardPage_Caption = "Preview"
End Function

Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean

End Function



Private Function IWizardPage_OnShowPage() As Boolean

   lblSourceDirectory.Caption = g_sSourcePath
   lblNoOfFiles.Caption = GetNoOfFilesInDirectory(g_sSourcePath)
   lblToAccount.Caption = g_sDestinationAccount
   
End Function


Private Sub UserControl_Initialize()
   ' Load account list

End Sub

