VERSION 5.00
Begin VB.UserControl PageSelectDomain 
   ClientHeight    =   1980
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   4800
   ScaleHeight     =   1980
   ScaleWidth      =   4800
   Begin VB.ComboBox cboDomains 
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   315
      Left            =   120
      Style           =   2  'Dropdown List
      TabIndex        =   0
      Top             =   360
      Width           =   3975
   End
   Begin VB.Label lblDomain 
      Caption         =   "Domain"
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
      TabIndex        =   1
      Top             =   120
      Width           =   2415
   End
End
Attribute VB_Name = "PageSelectDomain"
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
   IWizardPage_Caption = "Select domain to import to"
End Function

Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean
   If cboDomains.Text = "" Then
      MsgBox "You must select a domain to import the user accounts to."
      IWizardPage_OnLeavePage = True
   End If
   
   g_sDomain = cboDomains.Text
   
End Function



Private Function IWizardPage_OnShowPage() As Boolean

End Function


Private Sub UserControl_Initialize()

   
   Dim obDomains As Object
   Dim obDomain As Object
   Set obDomains = m_obApp.Domains
   
   If obDomains Is Nothing Then Exit Sub
   Dim iDomainIdx As Long
   For iDomainIdx = 0 To obDomains.Count - 1
      cboDomains.AddItem obDomains(iDomainIdx).Name
   Next iDomainIdx
   
End Sub
