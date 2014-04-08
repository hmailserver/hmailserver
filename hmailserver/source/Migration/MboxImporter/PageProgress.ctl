VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.UserControl PageProgress 
   ClientHeight    =   3075
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   8355
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ScaleHeight     =   3075
   ScaleWidth      =   8355
   Begin MSComctlLib.ProgressBar progressFiles 
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   1200
      Width           =   6255
      _ExtentX        =   11033
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin MSComctlLib.ProgressBar progressCurrentFile 
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   2640
      Width           =   6255
      _ExtentX        =   11033
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.Label lblCurrentTask 
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   1920
      Width           =   4575
   End
   Begin VB.Label lblCurrentTaskLabel 
      Caption         =   "Current task"
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   1680
      Width           =   3615
   End
   Begin VB.Label lblCurrentFileProgress 
      Caption         =   "Current file"
      Height          =   255
      Left            =   240
      TabIndex        =   4
      Top             =   2400
      Width           =   1455
   End
   Begin VB.Label lblFileProgress 
      Caption         =   "File progress"
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   960
      Width           =   3135
   End
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

Private m_oWizard As Wizard
Private m_oDBConn As clsDatabase

Private m_lDestinationAccountID As Long

Private m_sDestinationDirectory As String

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
   
   MsgBox "Import completed succesfully"
   
   End
   
End Function

Private Sub CreateDestinationFolder()
   On Error GoTo Exception
   Dim obApp As Object
   Set obApp = CreateObject("hMailServer.Application")
   
   Dim sIniFile As String
   sIniFile = obApp.InitializationFile
   
   ' Build the path to the destination directory
   m_sDestinationDirectory = GetProfString(sIniFile, "Directories", "DataFolder", "")
   If Right(m_sDestinationDirectory, 1) <> "\" Then
      m_sDestinationDirectory = m_sDestinationDirectory & "\"
   End If
   
   Dim sDomainName As String
   Dim sAccountName As String
   sAccountName = Mid(g_sDestinationAccount, 1, InStr(1, g_sDestinationAccount, "@") - 1)
   sDomainName = Mid(g_sDestinationAccount, InStr(1, g_sDestinationAccount, "@") + 1)
   
   On Error Resume Next
   
   Dim sDataDir As String
   m_sDestinationDirectory = m_sDestinationDirectory & sDomainName
   MkDir (m_sDestinationDirectory)
   
   m_sDestinationDirectory = m_sDestinationDirectory & "\" & sAccountName
   MkDir (m_sDestinationDirectory)
   
   On Error GoTo 0
   
   Exit Sub
Exception:
   MsgBox "Failed to create destination folder", vbCritical
   End
End Sub

Private Sub DoImport()
   DoEvents
   
   CreateDestinationFolder
   
   Dim lNoOfFiles As Long
   lNoOfFiles = GetNoOfFilesInDirectory(g_sSourcePath)
   
   progressFiles.Min = 0
   progressFiles.Max = lNoOfFiles
   
   DoEvents
   
   Set m_oDBConn = New clsDatabase
   m_lDestinationAccountID = m_oDBConn.GetAccountID(g_sDestinationAccount)
   
   If m_lDestinationAccountID = 0 Then
      MsgBox "Failed to determine destination account id", vbOKOnly
      Exit Sub
   End If
   
   IterateOverFiles g_sSourcePath
   
   DoEvents
End Sub

Private Sub IterateOverFiles(sFolder As String)
   Dim sName As String
   sName = Dir(g_sSourcePath & "*.*")
   
   While Len(sName) > 0
     
      Dim sFileName As String
      sFileName = g_sSourcePath & sName
      
      Call ParseFile(sName, sFileName)
         
      sName = Dir()
      
      progressFiles.Value = progressFiles.Value + 1
      
      DoEvents
   Wend
End Sub

Private Sub ParseFile(sFileName As String, sFullPath As String)
   
   On Error GoTo Exception
   
   lblCurrentTask.Caption = "Reading file..."
   
   Dim sFileContents As String
   sFileContents = ReadFile(sFullPath)
   
   lblCurrentTask.Caption = "Parsing list of messages..."
   
   Dim oMessages As Collection
   Set oMessages = CreateMessageList(sFileContents)
   
   lblCurrentTask.Caption = "Saving messages..."
   SaveMessages sFileName, oMessages
   
   Exit Sub
Exception:

   MsgBox "Failed to parse " & sFileName & vbNewLine & Err.Description, vbCritical
   End
End Sub

Private Sub SaveMessages(sIMAPFolderName As String, oMessages As Collection)
   ' First locate the folder for this user.
   On Error GoTo Exception
   
   Dim lFolderID As Long
   lFolderID = m_oDBConn.GetIMAPFolderID(m_lDestinationAccountID, sIMAPFolderName)
   
   progressCurrentFile.Max = oMessages.Count
   
   Dim i As Long
   For i = 1 To oMessages.Count
      
      progressCurrentFile.Max = i
      DoEvents
      
      Dim obParsedMessage As clsParsedMessage
      Set obParsedMessage = oMessages(i)
            
      Dim sOutputFilename As String
      sOutputFilename = GetUniqueFilename()
      
      SaveMessage obParsedMessage, lFolderID, sOutputFilename
      DoEvents
      
   Next i
   
   Exit Sub
   
Exception:
   
   MsgBox "An error occured while saving messages" & vbNewLine & Err.Description, vbCritical
   End
   
End Sub

Private Sub SaveMessage(oMessage As clsParsedMessage, lIMAPFolderID As Long, sFileName As String)
   On Error GoTo Exception
   Dim iFreeFile As Long
   iFreeFile = FreeFile()
   Open sFileName For Output As #iFreeFile
   Print #iFreeFile, oMessage.m_sMessageText
   Close #iFreeFile
   
   Dim lFileSize As Long
   lFileSize = FileLen(sFileName)
   
   ' Insert message into the database
   m_oDBConn.AddMessage m_lDestinationAccountID, lIMAPFolderID, sFileName, oMessage.m_sFrom, oMessage.m_sInternalDate
   
   Exit Sub
   
Exception:
   
   MsgBox "An error occured while saving an message" & vbNewLine & Err.Description, vbCritical
   End
   
End Sub

Private Function GetUniqueFilename() As String
   On Error GoTo Exception
   
   Dim sGUID As String
   sGUID = GetGUID()
      
   GetUniqueFilename = m_sDestinationDirectory & "\" & sGUID & ".eml"
   
   Exit Function
Exception:
   
   MsgBox "An error occured while creating unique filename." & vbNewLine & Err.Description, vbCritical
   End
End Function

Private Function CreateMessageList(sFileContents As String) As Collection

   On Error GoTo Exception
   
   ' Parse the file and create a list of messages.
   
   Dim oMsgColl As Collection
   Set oMsgColl = New Collection
   
   Dim lCurrentStartPos As Long
   lCurrentStartPos = 1
   
   progressCurrentFile.Min = 0
   progressCurrentFile.Max = Len(sFileContents)
   
   Dim lMessageEndPos As Long
   
   While True
      
      DoEvents
      
      If lMessageEndPos > 0 Then
         lCurrentStartPos = lMessageEndPos + 2
      End If
      
      lMessageEndPos = InStr(lCurrentStartPos + 1, sFileContents, vbLf & vbLf & "From ", vbBinaryCompare)
      
      If lMessageEndPos = 0 Then
         Debug.Print oMsgColl.Count
         
         Set CreateMessageList = oMsgColl
         
         Exit Function
      End If
      
      ' Copy current message.
      Dim sMessageText As String
      If lMessageEndPos = 0 Then
         sMessageText = Mid(sFileContents, lCurrentStartPos)
      Else
         Dim lMessageLength As Long
         lMessageLength = lMessageEndPos - lCurrentStartPos
         sMessageText = Mid(sFileContents, lCurrentStartPos, lMessageLength)
      End If
      DoEvents
      
      Dim obMessage As clsParsedMessage
      Set obMessage = CreateLocalRepresentation(sMessageText)
      
      oMsgColl.Add obMessage

      progressCurrentFile.Value = lMessageEndPos
      
      DoEvents
   Wend
   
   Set CreateMessageList = oMsgColl
   
   Exit Function
   
Exception:

   MsgBox "Failed to create message list" & vbNewLine & Err.Description, vbCritical
   End
      
End Function

Private Function CreateLocalRepresentation(sMessage As String) As clsParsedMessage

   On Error GoTo Exception

   Dim lSecondLineStart As Long
   lSecondLineStart = InStr(1, sMessage, vbLf) - 1
   
   Dim sFirstLine As String
   sFirstLine = Mid(sMessage, 1, lSecondLineStart)
   
   ' From MAILER-DAEMON Mon Sep 12 18:57:53 2005
   Dim obMessage As clsParsedMessage
   Set obMessage = New clsParsedMessage
   obMessage.m_sFrom = GetFromAddress(sFirstLine)
   obMessage.m_sInternalDate = GetInternalDate(sFirstLine)
   
   
   ' Remove first line
   sMessage = Mid(sMessage, lSecondLineStart + 2)
   
   sMessage = Replace(sMessage, vbCrLf, vbLf)
   sMessage = Replace(sMessage, vbLf, vbCrLf)
   
   ' Replace dot on empty line with doubledot
   sMessage = Replace(sMessage, vbCrLf & "." & vbCrLf, vbCrLf & ".." & vbCrLf)
   
   
   obMessage.m_sMessageText = sMessage
   
   Set CreateLocalRepresentation = obMessage
   
   Exit Function
   
Exception:
   
   MsgBox "Failed to create local representation of message." & vbNewLine & Err.Description, vbCritical
   End
   
End Function

Private Function GetInternalDate(sFirstLine As String) As String
   On Error GoTo Exception
   Dim lFromAddressStart As Long
   lFromAddressStart = 6
   
   Dim lFromAddressEnd As Long
   lFromAddressEnd = InStr(lFromAddressStart, sFirstLine, " ")
   
   If lFromAddressEnd = 0 Then
      GetInternalDate = ""
      Exit Function
   End If
   
   Dim sDateStamp As String
   sDateStamp = Mid(sFirstLine, lFromAddressEnd + 1)
   
   sDateStamp = Trim(sDateStamp)
   
   sDateStamp = GetSystemDateFromAscTime(sDateStamp)
   
   GetInternalDate = sDateStamp
   Exit Function
Exception:
   MsgBox "An error occured while parsing asciidate for " & sFirstLine & vbNewLine & Err.Description, vbCritical
   End
End Function

Private Function GetSystemDateFromAscTime(sAscTime As String)
   ' Mon Sep 12 18:57:53 2005
   
   On Error GoTo Exception
   
   Dim sYear As String
   sYear = Right(sAscTime, 4)
   
   Dim sMonth As String
   sMonth = Mid(sAscTime, 5, 3)
   sMonth = GetMonthValue(sMonth)
   
   Dim sDay As String
   sDay = Mid(sAscTime, 9, 2)
   If Left(sDay, 1) = " " Then Mid(sDay, 1, 1) = "0"
   
   Dim sHours As String
   sHours = Mid(sAscTime, 12, 8)
   
   Dim sFullTimeStamp As String
   sFullTimeStamp = sYear & "-" & sMonth & "-" & sDay & " " & sHours
   
   GetSystemDateFromAscTime = sFullTimeStamp
   
   Exit Function
Exception:
   MsgBox "An error occured while creating an system date for " & sAscTime & vbNewLine & Err.Description, vbCritical
   End
End Function

Private Function GetMonthValue(sMonthName As String) As String
   Select Case sMonthName
      Case "Jan"
         GetMonthValue = "01"
      Case "Feb"
         GetMonthValue = "02"
      Case "Mar"
         GetMonthValue = "03"
      Case "Apr"
         GetMonthValue = "04"
      Case "May"
         GetMonthValue = "05"
      Case "Jun"
         GetMonthValue = "06"
      Case "Jul"
         GetMonthValue = "07"
      Case "Aug"
         GetMonthValue = "08"
      Case "Sep"
         GetMonthValue = "09"
      Case "Oct"
         GetMonthValue = "10"
      Case "Nov"
         GetMonthValue = "11"
      Case "Dec"
         GetMonthValue = "12"
   End Select

End Function

Private Function GetFromAddress(sFirstLine As String) As String
   ' From MAILER-DAEMON Mon Sep 12 18:57:53 2005
   
   On Error GoTo Exception
   
   Dim lFromAddressStart As Long
   lFromAddressStart = 6
   
   Dim lFromAddressEnd As Long
   lFromAddressEnd = InStr(lFromAddressStart, sFirstLine, " ")
   
   Dim lFromAddressLength As Long
   lFromAddressLength = lFromAddressEnd - lFromAddressStart
   
   If lFromAddressLength < 0 Then
      GetFromAddress = ""
      Exit Function
   End If
   
   Dim sFromAddress As String
   sFromAddress = Mid(sFirstLine, lFromAddressStart, lFromAddressLength)
   
   GetFromAddress = sFromAddress
   
   Exit Function
Exception:

   MsgBox "Failed to create address for " & sFirstLine & vbNewLine & Err.Description, vbCritical
   End
End Function

Private Function ReadFile(sFile As String) As String
   On Error GoTo Exception
   
   Dim iFreeFile As Long
   iFreeFile = FreeFile()
   
   Dim lFileSize As Long
   lFileSize = FileLen(sFile)
   
   Open sFile For Binary As #iFreeFile
   Dim sBytes() As Byte
   ReDim sBytes(1 To lFileSize)
   Get iFreeFile, 1, sBytes
   Close #iFreeFile
   
   ' Create normal text string
   Dim sText As String
   sText = Space(lFileSize)
   
   Dim i As Long
   For i = 1 To lFileSize
      Mid(sText, i, 1) = Chr(sBytes(i))
   Next i
   
   ReadFile = sText
   Exit Function
   
Exception:

   MsgBox "Failed to read file " & sFile & vbNewLine & Err.Description, vbCritical
   End
End Function

Private Sub ShowProgress(sCaption As String)
   lblProgress.Caption = sCaption
   DoEvents
End Sub

