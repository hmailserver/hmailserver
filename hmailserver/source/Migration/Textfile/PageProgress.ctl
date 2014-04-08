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
   ImportFromFile g_sTextFile
End Sub

Private Sub ShowProgress(sCaption As String)
   lblProgress.Caption = sCaption
   DoEvents
End Sub
Private Function ImportFromFile(sFilename As String)

   Dim oDomain As Object
   Set oDomain = m_obApp.Domains.ItemByName(g_sDomain)
   
   Set m_oAccounts = oDomain.accounts

   Dim iFreeFile As Long
   iFreeFile = FreeFile()
   
   Open sFilename For Input As #iFreeFile
   
   ShowProgress "Counting accounts"
   
   ' First count number of rows.
   Dim iNumberOfLines As Long
   Dim sLine As String
   While EOF(iFreeFile) = False
      Line Input #iFreeFile, sLine
      
      If Left(sLine, 1) <> "#" And Trim(sLine) <> "" Then
         If UBound(Split(sLine, ",")) = 2 Then
            iNumberOfLines = iNumberOfLines + 1
         End If
      End If
   Wend
   
   Close #iFreeFile
   
   Open sFilename For Input As #iFreeFile
   
   Dim iNoOfImported As Long
   While EOF(iFreeFile) = False
      Line Input #iFreeFile, sLine
      
      If ImportFromLine(sLine) = True Then
         iNoOfImported = iNoOfImported + 1
      End If
      
      Dim sMessage As String
      sMessage = CStr(iNoOfImported) & " of " & CStr(iNumberOfLines) & " imported."
      
      ShowProgress sMessage
      
   Wend
      
   Close #iFreeFile
   
End Function

Private Function ImportFromLine(sLine As String) As Boolean
   ImportFromLine = False
   
   If Trim(sLine) = "" Then Exit Function
   If Left(sLine, 1) = "#" Then Exit Function
   
   Dim sParams() As String
   sParams = Split(sLine, ",")
   
   If UBound(sParams) < 1 Then
      Exit Function
   End If
   
   AddAccount sParams(0), sParams(1), CLng(sParams(2))
   
   ImportFromLine = True
End Function

Private Function AddAccount(sEmail As String, sPassword As String, lQuotaMB As Long) As Boolean
   
   Dim oAccount As Object
   Set oAccount = GetAccount(sEmail)
   
   If oAccount Is Nothing Then
      Set oAccount = m_oAccounts.Add()
   End If
   
   oAccount.Address = sEmail & "@" & g_sDomain
   oAccount.Password = sPassword
   oAccount.Active = True
   oAccount.MaxSize = lQuotaMB
   oAccount.Save
   
   AddAccount = True
   
End Function

Private Function GetAccount(sEmail As String) As Object
   On Error GoTo ErrorOccured
   Dim oAccount As Object
   Set oAccount = m_oAccounts.ItemByAddress(sEmail & "@" & g_sDomain)
   
   Set GetAccount = oAccount

   On Error GoTo 0
   Exit Function

ErrorOccured:
   
   Set GetAccount = Nothing
   On Error GoTo 0
   
End Function
