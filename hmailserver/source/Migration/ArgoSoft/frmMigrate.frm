VERSION 5.00
Begin VB.Form frmMigrate 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "ArgoSoft MailServer to hMailServer"
   ClientHeight    =   5130
   ClientLeft      =   45
   ClientTop       =   360
   ClientWidth     =   7920
   Icon            =   "frmMigrate.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   5130
   ScaleWidth      =   7920
   StartUpPosition =   3  'Windows Default
   Begin ArgoSoftMigrate.Wizard Wizard1 
      Height          =   5055
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   8055
      _ExtentX        =   14208
      _ExtentY        =   8916
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

   Wizard1.AddPage "ArgoSoftMigrate.PageSelectFile"
   Wizard1.AddPage "ArgoSoftMigrate.PagePreview"
   Wizard1.AddPage "ArgoSoftMigrate.PageProgress"
   Wizard1.Start

   
End Sub

Private Sub Form_Resize()
   Wizard1.Width = Me.Width
   Wizard1.Height = Me.Height
End Sub


Private Sub Wizard1_WizardComplete()
      
   
   
   End
End Sub
