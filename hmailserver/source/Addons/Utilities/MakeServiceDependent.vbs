option explicit

Dim oApp
Set oApp = CreateObject("hMailServer.Application")

' BEGIN: Authenticate the client.
Dim sAdminPwd
sAdminPwd = InputBox("Enter your main hMailServer administrator password.", "hMailServer")
Call oApp.Authenticate ("Administrator", sAdminPwd)
' END: Authenticate the client.

dim oUtilities
set oUtilities = oApp.Utilities

Dim sName
sName = InputBox("Enter the name of the service hMailServer should be dependent on", "hMailServer")

If sName <> "" Then
	oUtilities.MakeDependent(sName)
End If

MsgBox "Done"