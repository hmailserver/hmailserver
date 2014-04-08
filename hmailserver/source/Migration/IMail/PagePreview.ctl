VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.UserControl PagePreview 
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
      Sorted          =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   3
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Domain"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "User name"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "Account size"
         Object.Width           =   2540
      EndProperty
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
   
   Dim iDomainIdx As Long
   
   For iDomainIdx = 1 To g_oIMailDomains.Count
      
      Dim sDomainName As String
      sDomainName = g_oIMailDomains(iDomainIdx)
      
      Dim oColl As Collection
      Set oColl = GetIMailUsersForDomains(sDomainName)
      
      Dim i As Long
      For i = 1 To oColl.Count
         Dim oUser As IMailUser
         Set oUser = oColl(i)
         
         Dim oListItem As ListItem
         Set oListItem = lvwAccounts.ListItems.Add(, , sDomainName)
         oListItem.SubItems(1) = oUser.m_sName
         oListItem.SubItems(2) = oUser.m_iAccountSize / 1000000 & " MB"
         
      Next i
   Next iDomainIdx
End Function


