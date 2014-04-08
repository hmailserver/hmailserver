Option Explicit

Dim oApp
Set oApp = CreateObject("hMailServer.Application")

' BEGIN: Authenticate the client.
Dim sAdminPwd
sAdminPwd = InputBox("Enter your main hMailServer administrator password.", "hMailServer")
Call oApp.Authenticate ("Administrator", sAdminPwd)
' END: Authenticate the client.

dim sInput
sInput = Inputbox("Enter encrypted password", "hMailServer")

dim sOutput
sOutput = oApp.Utilities.BlowfishDecrypt(sInput)

MsgBox sOutput

Set oApp = Nothing