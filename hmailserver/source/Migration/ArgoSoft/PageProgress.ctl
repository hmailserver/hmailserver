VERSION 5.00
Begin VB.UserControl PageProgress 
   ClientHeight    =   1485
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   4800
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ScaleHeight     =   1485
   ScaleWidth      =   4800
   Begin VB.Label lblProgress 
      Height          =   255
      Left            =   240
      TabIndex        =   1
      Top             =   480
      Width           =   3855
   End
   Begin VB.Label lblProgressCaption 
      Caption         =   "Progress:"
      Height          =   255
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   3855
   End
End
Attribute VB_Name = "PageProgress"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Implements IWizardPage

Private m_oAccounts As Object


Private m_oWizard As Wizard

Private Function IWizardPage_Initialize(oWizard As Wizard) As Variant
   Set m_oWizard = oWizard
End Function

Private Function IWizardPage_Caption() As String
   IWizardPage_Caption = "Start"
End Function

Private Function IWizardPage_OnLeavePage(iButton As Button) As Boolean
End Function

Private Function IWizardPage_OnShowPage() As Boolean
   DoEvents
   
   m_oWizard.EnableNext (False)
   m_oWizard.EnablePrevious (False)
   
   DoImport
   
   m_oWizard.EnableNext (True)
   m_oWizard.EnablePrevious (True)
   
End Function

Private Sub DoImport()

   Dim obApp As Object
   Set obApp = CreateObject("hMailServer.Application")
   
   Dim obDomains As Object
   Dim obDomain As Object
   Set obDomains = obApp.Domains
   
   Dim iDomainIdx As Long
   
   For iDomainIdx = 1 To g_oIMailDomains.Count
      
      Dim sDomainName As String
      sDomainName = g_oIMailDomains(iDomainIdx)
      
      Dim oColl As Collection
      Set oColl = GetArgoSoftAccounts(sDomainName)
         
      Set obDomain = GetDomain(obDomains, sDomainName)
      
      If obDomain Is Nothing Then
         Set obDomain = obDomains.Add
         obDomain.Name = sDomainName
         obDomain.Active = True
         obDomain.Save
      End If
      
      
      Set m_oAccounts = obDomain.Accounts
      
      Dim i As Long
      For i = 1 To oColl.Count
         Dim oUser As ArgoSoftUser
         Set oUser = oColl(i)
         
         ShowProgress "Importing " & sDomainName & ", " & oUser.m_sName & " (" & i & "/" & oColl.Count & ")"
         
         AddAccount sDomainName, oUser.m_sName, oUser.m_sPassword, oUser.m_iAccountSize / 1000000
      Next i
   Next iDomainIdx
   
End Sub

Private Function GetDomain(obDomains As Object, sName As String) As Object
   On Error GoTo ErrorOccured
   
   Set GetDomain = obDomains.ItemByName(sName)
   
   Exit Function
ErrorOccured:
End Function

Private Sub ShowProgress(sCaption As String)
   lblProgress.Caption = sCaption
   DoEvents
End Sub


Private Function AddAccount(sDomainName As String, sEmail As String, sPassword As String, lQuotaMB As Long) As Boolean
   
   Dim oAccount As Object
   Set oAccount = GetAccount(sEmail & "@" & sDomainName)
   
   If oAccount Is Nothing Then
      Set oAccount = m_oAccounts.Add()
   End If
   
   oAccount.Address = sEmail & "@" & sDomainName
   oAccount.Password = sPassword
   oAccount.Active = True
   oAccount.MaxSize = lQuotaMB
   oAccount.Save
   
   AddAccount = True
   
End Function

Private Function GetAccount(sEmail As String) As Object
   On Error GoTo ErrorOccured
   Dim oAccount As Object
   Set oAccount = m_oAccounts.ItemByAddress(sEmail)
   
   Set GetAccount = oAccount

   On Error GoTo 0
   Exit Function

ErrorOccured:
   
   Set GetAccount = Nothing
   On Error GoTo 0
   
End Function

