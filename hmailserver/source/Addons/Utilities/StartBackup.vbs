' This scripts creates a hMailServer backup using the settings 
' specified in hMailAdmin. You can double-click on the file to
' run the script.
' 
' To use the script you need to update the sAdminPassword
' setting below to reflect your own hMailServer Administrator
' password. If you want to run automatic backups, please 
' start this script using Windows Scheduled Tasks.
'


Const sAdminPassword = "<ADMINISTRATORPASSWORD>"

Dim oApp
Set oApp = CreateObject("hMailServer.Application")

' Authenticate the client.
Call oApp.Authenticate ("Administrator", sAdminPassword)

Call oApp.BackupManager.StartBackup()