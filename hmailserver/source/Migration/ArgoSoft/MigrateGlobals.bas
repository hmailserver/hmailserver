Attribute VB_Name = "MigrateGlobals"
Option Explicit

Public g_oIMailDomains As Collection

Public Function GetArgoSoftDomains() As Collection
   
   On Error GoTo ErrorOccured
   
   Dim oResultColl As Collection
   Set oResultColl = New Collection
   
   
   Dim obLocalDomains As Object
   Set obLocalDomains = CreateObject("MailServerX.LocalDomains")

   
   Dim i As Long
   For i = 0 To obLocalDomains.Count - 1
      Dim obLocalDomain As Object
      Set obLocalDomain = obLocalDomains.Items(i)
      
      Dim sName As String
      sName = obLocalDomain.Name
      
      oResultColl.Add sName
   Next i
   

   Set GetArgoSoftDomains = oResultColl
   
   Exit Function
   
ErrorOccured:
   
   MsgBox Err.Description
   End
End Function

Public Function GetArgoSoftDomain(sName As String) As Object
   
   On Error GoTo ErrorOccured

   Dim obLocalDomains As Object
   Set obLocalDomains = CreateObject("MailServerX.LocalDomains")
   
   Dim i As Long
   For i = 0 To obLocalDomains.Count - 1
      Dim obLocalDomain As Object
      Set obLocalDomain = obLocalDomains.Items(i)
      
      If sName = obLocalDomain.Name Then
         Set GetArgoSoftDomain = obLocalDomain
         Exit Function
      End If
   Next i
   

   Set GetArgoSoftDomain = Nothing
   
   Exit Function
   
ErrorOccured:
   
   MsgBox Err.Description
   End
End Function

Public Function GetArgoSoftAccounts(ByVal sDomainName As String) As Collection
   
   On Error GoTo ErrorOccured
   
   Dim oResultColl As Collection
   Set oResultColl = New Collection
   
   Dim Users, i
   Set Users = CreateObject("MailServerX.Users")
   For i = 0 To Users.Count - 1
     Dim obUser As Object
     Set obUser = Users.Items(i)
     
     If InStr(1, obUser.UserName, "@" & sDomainName) > 0 Then
        Dim oUser As ArgoSoftUser
        Set oUser = New ArgoSoftUser
     
        oUser.m_sName = Mid(obUser.UserName, 1, InStr(1, obUser.UserName, "@") - 1)
        oUser.m_sPassword = obUser.Password
        oUser.m_iAccountSize = obUser.MailboxSize
        
        oResultColl.Add oUser
        
     End If
     
   Next
   
   Set GetArgoSoftAccounts = oResultColl
   
   Exit Function
   
ErrorOccured:
   
   MsgBox Err.Description
   End
End Function
