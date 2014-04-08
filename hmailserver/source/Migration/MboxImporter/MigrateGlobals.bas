Attribute VB_Name = "MigrateGlobals"
Option Explicit

Public g_sSourcePath As String
Public g_sDestinationAccount As String

Private Declare Function CoCreateGuid Lib "OLE32.DLL" (pGuid As GUID) As Long

Private Type GUID
   Data1 As Long
   Data2 As Integer
   Data3 As Integer
   Data4(7) As Byte
End Type

Public Function GetNoOfFilesInDirectory(sDirectory As String) As Long
   Dim sName As String
   sName = Dir(g_sSourcePath & "*.*")
   
   Dim lNoOfFiles As Long
   lNoOfFiles = 0
   
   While Len(sName) > 0
      lNoOfFiles = lNoOfFiles + 1
      sName = Dir()
   Wend
   
   GetNoOfFilesInDirectory = lNoOfFiles
End Function


Public Function GetGUID() As String
   '(c) 2000 Gus Molina
   
   Dim udtGUID As GUID
   
   If (CoCreateGuid(udtGUID) = 0) Then
   
   GetGUID = _
   String(8 - Len(Hex$(udtGUID.Data1)), "0") & Hex$(udtGUID.Data1) & _
   String(4 - Len(Hex$(udtGUID.Data2)), "0") & Hex$(udtGUID.Data2) & _
   String(4 - Len(Hex$(udtGUID.Data3)), "0") & Hex$(udtGUID.Data3) & _
   IIf((udtGUID.Data4(0) < &H10), "0", "") & Hex$(udtGUID.Data4(0)) & _
   IIf((udtGUID.Data4(1) < &H10), "0", "") & Hex$(udtGUID.Data4(1)) & _
   IIf((udtGUID.Data4(2) < &H10), "0", "") & Hex$(udtGUID.Data4(2)) & _
   IIf((udtGUID.Data4(3) < &H10), "0", "") & Hex$(udtGUID.Data4(3)) & _
   IIf((udtGUID.Data4(4) < &H10), "0", "") & Hex$(udtGUID.Data4(4)) & _
   IIf((udtGUID.Data4(5) < &H10), "0", "") & Hex$(udtGUID.Data4(5)) & _
   IIf((udtGUID.Data4(6) < &H10), "0", "") & Hex$(udtGUID.Data4(6)) & _
   IIf((udtGUID.Data4(7) < &H10), "0", "") & Hex$(udtGUID.Data4(7))
End If

End Function
         

