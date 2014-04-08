VERSION 5.00
Begin VB.Form frmMigrate 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "hMailServer text file importer"
   ClientHeight    =   4395
   ClientLeft      =   45
   ClientTop       =   360
   ClientWidth     =   7920
   Icon            =   "frmMigrate.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   4395
   ScaleWidth      =   7920
   StartUpPosition =   3  'Windows Default
   Begin TFMigrate.Wizard Wizard1 
      Height          =   4815
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   8055
      _ExtentX        =   14208
      _ExtentY        =   8493
   End
End
Attribute VB_Name = "frmMigrate"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Private Sub Form_Load()
   
   LoadLanguage
   
   Set m_obApp = CreateObject("hMailServer.Application")
   If AuthenticateUser(m_obApp) = False Then
      End
   End If
   
   Wizard1.AddPage "TFMigrate.PageSelectFile"
   Wizard1.AddPage "TFMigrate.PageSelectDomain"
   Wizard1.AddPage "TFMigrate.PageProgress"
   Wizard1.Start
   
   
   'GenerateTestFile
   
End Sub

Private Sub Form_Resize()
   Wizard1.Width = Me.Width
   Wizard1.Height = Me.Height
End Sub


Private Sub Wizard1_WizardComplete()
      
   
   
   End
End Sub


Private Sub GenerateTestFile()
   Open "C:\test.txt" For Output As #1
   Dim i As Long
   Randomize Timer
   For i = 1 To 100
      Dim sAccountName As String
      sAccountName = "HE" & Rnd(i)
      sAccountName = Replace(sAccountName, ",", "A")
      
      Dim i2 As Long
      i2 = CLng(Rnd(10) * 1000)
      
      Print #1, sAccountName & ",TESTPWD," & i2
   Next i
   
   Close #1
End Sub
