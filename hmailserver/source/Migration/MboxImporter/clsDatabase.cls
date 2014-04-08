VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsDatabase"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Private m_oDBConn As ADODB.Connection

Private m_sHost As String
Private m_lPort As Long
Private m_sUsername As String
Private m_sPassword As String
Private m_sDatabase As String

Private Sub LoadDBSettings()
   Dim obApp As Object
   Set obApp = CreateObject("hMailServer.Application")
   
   Dim sIniFile As String
   sIniFile = obApp.InitializationFile
   
   m_sUsername = GetProfString(sIniFile, "Database", "Username", "")
   m_sPassword = GetProfString(sIniFile, "Database", "Password", "")
   m_sDatabase = GetProfString(sIniFile, "Database", "Database", "")
   m_sHost = GetProfString(sIniFile, "Database", "Server", "")
   m_lPort = CLng(GetProfString(sIniFile, "Database", "Port", ""))
   
   Dim sPWEncryption As String
   sPWEncryption = GetProfString(sIniFile, "Database", "Passwordencryption", "")
   
   If sPWEncryption = "1" Then
      ' The password needs to be decrypted.
      
      m_sPassword = obApp.Utilities.BlowfishDecrypt(m_sPassword)
   End If
   
   
End Sub

Public Sub OpenConnection()

   LoadDBSettings
   
   Set m_oDBConn = New ADODB.Connection
   
   m_oDBConn.CursorLocation = adUseClient

   m_oDBConn.Open "DRIVER={MySQL ODBC 3.51 Driver};" & _
                 "SERVER=" & m_sHost & ";" & _
                 "PORT=" & m_lPort & ";" & _
                 "DATABASE=" & m_sDatabase & ";" & _
                 "USER=" & m_sUsername & ";" & _
                 "PASSWORD=" & m_sPassword & ";"
                 
   m_oDBConn.Execute "USE " & m_sDatabase
End Sub
Public Function GetAccountID(sName As String)
   Dim sSQL As String
   sSQL = "select accountid from hm_accounts where accountaddress = '" & sName & "'"
   
   Dim oRS As ADODB.Recordset
   Set oRS = New ADODB.Recordset
   
   oRS.Open sSQL, m_oDBConn, adOpenStatic, adLockReadOnly
   GetAccountID = oRS.Fields("accountid").Value
   oRS.Close
End Function

Public Function GetIMAPFolderID(iAccountID, sFolderName As String) As Long
   
   Dim sSQL As String
   sSQL = "select folderid from hm_imapfolders where folderaccountid = " & iAccountID & " and foldername = '" & sFolderName & "' and folderparentid = -1"
   
   Dim oRS As ADODB.Recordset
   Set oRS = New ADODB.Recordset
   
   Dim lFolderID As Long
   
   oRS.Open sSQL, m_oDBConn, adOpenStatic, adLockReadOnly
   If oRS.RecordCount > 0 Then
      lFolderID = oRS.Fields("folderid")
   Else
      lFolderID = 0
   End If
   oRS.Close
   
   If lFolderID > 0 Then
      GetIMAPFolderID = lFolderID
      Exit Function
   End If
   
   ' Create it.
   Dim sInsertSQL As String
   sInsertSQL = "insert into hm_imapfolders (folderaccountid, folderparentid, foldername, folderissubscribed) values (" & iAccountID & ", -1, '" & sFolderName & "', 1)"
   m_oDBConn.Execute sInsertSQL

   ' Fetch the new ID
   oRS.Open sSQL, m_oDBConn, adOpenStatic, adLockReadOnly
   If oRS.RecordCount > 0 Then
      lFolderID = oRS.Fields("folderid")
   Else
      lFolderID = 0
   End If
   oRS.Close

   If lFolderID > 0 Then
      GetIMAPFolderID = lFolderID
      Exit Function
   Else
      MsgBox "Failed to create IMAP folder " & sFolderName
      End
   End If
      
End Function


Public Function GetAccounts() As Collection
   Dim sSQL As String
   sSQL = "select accountid, accountaddress from hm_accounts order by accountaddress asc"
   
   Dim oRetColl As Collection
   Set oRetColl = New Collection
   
   Dim oRS As ADODB.Recordset
   Set oRS = New ADODB.Recordset
   
   oRS.Open sSQL, m_oDBConn, adOpenStatic, adLockReadOnly
   While oRS.EOF = False
   
      Dim sAddress As String
      sAddress = oRS.Fields("accountaddress")
      
      oRetColl.Add (sAddress)
   
      oRS.MoveNext
   Wend
   
   oRS.Close
   
   Set GetAccounts = oRetColl
End Function

Public Sub AddMessage(lAccountID As Long, lFolderID As Long, sFileName As String, sFrom As String, sCreateTime As String)
   
   Dim lFileSize As Long
   lFileSize = FileLen(sFileName)
   
   sFileName = Replace(sFileName, "\", "\\")
   sFrom = Replace(sFrom, "\", "\\")
   
   Dim sSQL As String
   sSQL = "insert into hm_messages (messageaccountid, messagefolderid, messagefilename, messagetype, messagefrom, messagesize, messagecreatetime) values "
   sSQL = sSQL & "(" & lAccountID & ", " & lFolderID & ", '" & sFileName & "', 2, '" & sFrom & "', " & lFileSize & ", '" & sCreateTime & "')"
      
   m_oDBConn.Execute sSQL
End Sub

Private Sub Class_Initialize()
   OpenConnection
   
End Sub
