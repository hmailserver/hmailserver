VERSION 5.00
Begin VB.UserControl PageSelectFile 
   ClientHeight    =   3570
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   6945
   ScaleHeight     =   3570
   ScaleWidth      =   6945
   Begin VB.ListBox lstArgoSoftDomains 
      Height          =   1635
      Left            =   720
      Style           =   1  'Checkbox
      TabIndex        =   2
      Top             =   1680
      Width           =   2655
   End
   Begin VB.Label lblSelectIMail 
      Caption         =   "Select ArgoSoft domains"
      Height          =   255
      Left            =   720
      TabIndex        =   1
      Top             =   1440
      Width           =   2175
   End
   Begin VB.Label lblInfo 
      Caption         =   $"PageSelectFile.ctx":0000
      Height          =   615
      Left            =   720
      TabIndex        =   0
      Top             =   360
      Width           =   4575
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
   
   Dim oColl As Collection
   Set oColl = GetArgoSoftDomains
   
   Dim i As Long
   For i = 1 To oColl.Count
      lstArgoSoftDomains.AddItem oColl(i)
   Next i
End Function

Private Sub cmdBrowse_Click()
   
End Sub

Private Function IWizardPage_Caption() As String
   IWizardPage_Caption = "Select domain to migrate"
End Function



Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean

   Set g_oIMailDomains = New Collection
   Dim i As Long
   For i = 0 To lstArgoSoftDomains.ListCount - 1
      If lstArgoSoftDomains.Selected(i) = True Then
         g_oIMailDomains.Add lstArgoSoftDomains.List(i)
      End If
      
   Next i


   If g_oIMailDomains.Count = 0 Then
      MsgBox "You must select at least one domain to migrate."
      IWizardPage_OnLeavePage = True
   End If
End Function

Private Function IWizardPage_OnShowPage() As Boolean

End Function


