Attribute VB_Name = "MigrateGlobals"
Option Explicit

Private Const HKEY_LOCAL_MACHINE = &H80000002
Private Declare Function RegOpenKeyEx Lib "advapi32.dll" _
            Alias "RegOpenKeyExA" _
            (ByVal hKey As Long, _
            ByVal lpSubKey As String, _
            ByVal ulOptions As Long, _
            ByVal samDesired As Long, phkResult As Long) As Long
Declare Function RegQueryValueExNULL Lib "advapi32.dll" Alias _
   "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As _
   String, ByVal lpReserved As Long, lpType As Long, ByVal lpData _
   As Long, lpcbData As Long) As Long
Declare Function RegQueryValueExString Lib "advapi32.dll" Alias _
   "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As _
   String, ByVal lpReserved As Long, lpType As Long, ByVal lpData _
   As String, lpcbData As Long) As Long
   
Private Declare Function RegCloseKey Lib "advapi32.dll" _
         (ByVal hKey As Long) As Long

Private Declare Function RegEnumKey Lib "advapi32.dll" Alias "RegEnumKeyA" _
         (ByVal hKey As Long, _
         ByVal dwIndex As Long, _
         ByVal lpName As String, _
         ByVal cbName As Long) As Long
         
Private Declare Function RegQueryValueEx Lib "advapi32" Alias _
      "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As String, _
      ByVal lpReserved As Long, ByRef lpType As Long, ByVal lpData As String, _
      ByRef lpcbData As Long) As Long
         
Declare Function RegQueryValueExLong Lib "advapi32.dll" Alias _
   "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As _
   String, ByVal lpReserved As Long, lpType As Long, lpData As _
   Long, lpcbData As Long) As Long


Private Const SYNCHRONIZE = &H100000
Private Const STANDARD_RIGHTS_READ = &H20000
Private Const STANDARD_RIGHTS_WRITE = &H20000
Private Const STANDARD_RIGHTS_EXECUTE = &H20000
Private Const STANDARD_RIGHTS_REQUIRED = &HF0000
Private Const STANDARD_RIGHTS_ALL = &H1F0000
Private Const KEY_QUERY_VALUE = &H1
Private Const KEY_SET_VALUE = &H2
Private Const KEY_CREATE_SUB_KEY = &H4
Private Const KEY_ENUMERATE_SUB_KEYS = &H8
Private Const KEY_NOTIFY = &H10
Private Const KEY_CREATE_LINK = &H20
Public Const REG_SZ = 1
Public Const REG_EXPAND_SZ = 2
Public Const REG_BINARY = 3
Public Const REG_DWORD = 4
Private Const IMAIL_REGKEY As String = "SOFTWARE\Ipswitch\IMail\Domains"
Private Const ERROR_SUCCESS = 0&
Private Const KEY_READ = ((STANDARD_RIGHTS_READ Or _
                        KEY_QUERY_VALUE Or _
                        KEY_ENUMERATE_SUB_KEYS Or _
                        KEY_NOTIFY) And _
                        (Not SYNCHRONIZE))

Public g_oIMailDomains As Collection
Public m_obApp As Object

Public Function GetIMailDomains() As Collection
   
   On Error GoTo ErrorOccured
   
   Dim oResultColl As Collection
   Set oResultColl = New Collection
   
   Dim lResult As Long
   
   Dim lKeyHandle As Long
   lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _
                           IMAIL_REGKEY, _
                           0&, _
                           KEY_READ, _
                           lKeyHandle)
                              
   If lResult <> ERROR_SUCCESS Then
      Err.Raise 1001, 0, "Could not read list of IMail domains from registry"
   End If
   
   ' Enumerate sub keys
   Dim lDomainIdx As Long
   Dim sValue As String
   Do
      sValue = String(255, 0)
      
      lResult = RegEnumKey(lKeyHandle, _
                               lDomainIdx, _
                               ByVal sValue, _
                               255)
                               
      If InStr(sValue, Chr$(0)) > 0 Then
         sValue = Left(sValue, InStr(sValue, Chr$(0)) - 1)
      End If
      
      If Len(sValue) > 0 Then
      
         ' Try to open the Users sub key.
         Dim lUsersSubKey As Long
         Dim lUserResult As Long
         lUserResult = RegOpenKeyEx(lKeyHandle, sValue & "\Users", 0, KEY_READ, lUsersSubKey)
      
         If lUserResult = ERROR_SUCCESS Then
            oResultColl.Add sValue
            RegCloseKey lUsersSubKey
         End If
      End If
      
      lDomainIdx = lDomainIdx + 1
                               
   Loop While lResult = ERROR_SUCCESS
   
   
   RegCloseKey lKeyHandle
   
   Set GetIMailDomains = oResultColl
   
   Exit Function
   
ErrorOccured:
   
   MsgBox Err.Description
   End
End Function

Public Function GetIMailUsersForDomains(ByVal sDomainName As String) As Collection
   
   On Error GoTo ErrorOccured
   
   Dim oResultColl As Collection
   Set oResultColl = New Collection
   
   Dim lResult As Long
   
   Dim lKeyHandle As Long
   lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _
                           IMAIL_REGKEY & "\" & sDomainName & "\Users", _
                           0&, _
                           KEY_READ, _
                           lKeyHandle)
                              
   If lResult <> ERROR_SUCCESS Then
      Err.Raise 1001, 0, "Could not read list of IMail domains from registry"
   End If
   
   ' Enumerate users
   Dim lDomainIdx As Long
   Dim sValue As String
   Do
      sValue = String(255, 0)
      
      lResult = RegEnumKey(lKeyHandle, _
                               lDomainIdx, _
                               ByVal sValue, _
                               255)
                               
      If InStr(sValue, Chr$(0)) > 0 Then
         sValue = Left(sValue, InStr(sValue, Chr$(0)) - 1)
      End If
      
      If Len(sValue) > 0 Then
         ' Open the subkey for this user.
         Dim lUserSubKey As Long
         Dim lUserResult As Long
         lUserResult = RegOpenKeyEx(lKeyHandle, sValue, 0, KEY_READ, lUserSubKey)
         
         If lResult <> ERROR_SUCCESS Then
            Err.Raise 1001, 0, "Failed to open sub key for user"
         End If
         
         If sValue = "_aliases" Then
         
         Else
            ' We have found a user, read settings for this one.
            Dim oUser As IMailUser
            Set oUser = New IMailUser
          
            oUser.m_sName = sValue
            oUser.m_iAccountSize = CLng(QueryValueEx(lUserSubKey, "MaxSize"))
            oUser.m_sPassword = DecodeIMailPassword(sValue, QueryValueEx(lUserSubKey, "Password"))
         
            oResultColl.Add oUser
          
            RegCloseKey lUserResult
         End If
      
      End If
      
      lDomainIdx = lDomainIdx + 1
                               
   Loop While lResult = ERROR_SUCCESS
   
   
   RegCloseKey lKeyHandle
   
   Set GetIMailUsersForDomains = oResultColl
   
   Exit Function
   
ErrorOccured:
   
   MsgBox Err.Description
   End
End Function

Function QueryValueEx(ByVal lhKey As Long, ByVal szValueName As String) As Variant
       Dim cch As Long
       Dim lrc As Long
       Dim lType As Long
       Dim lValue As Long
       Dim sValue As String

       Dim vRetVal As Variant

       On Error GoTo QueryValueExError

       ' Determine the size and type of data to be read
       lrc = RegQueryValueExNULL(lhKey, szValueName, 0&, lType, 0&, cch)
       If lrc <> ERROR_SUCCESS Then Error 5

       Select Case lType
           ' For strings
           Case REG_SZ:
               sValue = String(cch, 0)
               lrc = RegQueryValueExString(lhKey, szValueName, 0&, lType, sValue, cch)
               If lrc = ERROR_SUCCESS Then
                   vRetVal = Left$(sValue, cch - 1)
               Else
                   vRetVal = Empty
               End If
           ' For DWORDS
           Case REG_DWORD:
            lrc = RegQueryValueExLong(lhKey, szValueName, 0&, lType, lValue, cch)
               If lrc = ERROR_SUCCESS Then vRetVal = lValue
         Case Else
               'all other data types not supported
               lrc = -1
       End Select

QueryValueExExit:
       QueryValueEx = vRetVal
       Exit Function

QueryValueExError:
       Resume QueryValueExExit
   End Function


Private Function DecodeIMailPassword(sUsername As String, sEncoded As String) As String
   Dim sUserNameDiffs(255) As Long
   Dim iFirstCharAscii As Long
   Dim iFirstDiffFromA As Long
   
   sUserNameDiffs(0) = 0
   
   Dim i As Long
   For i = 1 To Len(sUsername)
      Dim sChar As String
      sChar = Mid(sUsername, i, 1)
      
      If i = 1 Then
         iFirstCharAscii = Asc(sChar)
         iFirstDiffFromA = Asc(sChar) - Asc("a")
      Else
         sUserNameDiffs(i - 1) = iFirstCharAscii - Asc(sChar)
      End If
   Next i
   
   Dim sPassword As String
   
   Dim iDiffIdx As Long
   iDiffIdx = 0
   

   
   For i = 1 To Len(sEncoded) Step 2
      Dim sPart As String
      sPart = Mid(sEncoded, i, 2)
      
      Dim lValue As Long
      lValue = LookupMap1(sPart)
      lValue = lValue + sUserNameDiffs(iDiffIdx)
      
      Dim sAsc As String
      sAsc = LookupMap2(iFirstDiffFromA, lValue)
      
      iDiffIdx = iDiffIdx + 1
      sPassword = sPassword & sAsc
   Next i
   
   DecodeIMailPassword = sPassword
   
End Function

Private Function LookupMap1(sPwdPart As String) As Long
   Dim lAscValue As Long
   lAscValue = CLng("&H" + sPwdPart)
   lAscValue = lAscValue - 97
   
   LookupMap1 = lAscValue
End Function

Private Function LookupMap2(lFirstDiffFromA As Long, lValue As Long) As String
   Dim sRetVal As String
   sRetVal = Chr(lValue - lFirstDiffFromA)
   
   LookupMap2 = sRetVal
End Function


