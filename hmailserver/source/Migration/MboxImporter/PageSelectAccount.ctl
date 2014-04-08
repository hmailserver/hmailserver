VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.UserControl PageSelectAccount 
   ClientHeight    =   2820
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   8970
   ScaleHeight     =   2820
   ScaleWidth      =   8970
   Begin MSComctlLib.ListView lvwAccounts 
      Height          =   2175
      Left            =   240
      TabIndex        =   0
      Top             =   480
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   3836
      View            =   3
      LabelEdit       =   1
      Sorted          =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   14737632
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   1
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Account"
         Object.Width           =   7056
      EndProperty
   End
End
Attribute VB_Name = "PageSelectAccount"
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
   IWizardPage_Caption = "Select destination account"
End Function

Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean
   
   Dim oSelectedItem As ListItem
   Set oSelectedItem = lvwAccounts.SelectedItem
   
   If oSelectedItem Is Nothing Then
      MsgBox "You must select destination account.", vbOKOnly
      IWizardPage_OnLeavePage = True
      Exit Function
   End If
   
   g_sDestinationAccount = oSelectedItem.Text
End Function



Private Function IWizardPage_OnShowPage() As Boolean
   ' Show account list
End Function


Private Sub UserControl_Initialize()
   ' Load account list
   Dim oDB As clsDatabase
   Set oDB = New clsDatabase
   
   Dim oAccounts As Collection
   Set oAccounts = oDB.GetAccounts
   
   Dim i As Long
   For i = 1 To oAccounts.Count
      Dim sAccount As String
      sAccount = oAccounts(i)
      
      Call lvwAccounts.ListItems.Add(, , sAccount)
   Next i
End Sub
