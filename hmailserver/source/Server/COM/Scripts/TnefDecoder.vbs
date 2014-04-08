   Const g_sTempPath = "C:\Temp\TempTnef"
   Const g_sTnefPath = "C:\temp\tnefjava\lib\tnef.jar"
   Const g_sCmdPath  = "C:\WINDOWS\system32\cmd.exe"

   Sub OnDeliverMessage(obMessage)
      ExtractTNEFAttachments(obMessage)
   End Sub
  
   Sub ExtractTNEFAttachments(obMessage)
      ' Iterates over all attachments and checks where
      ' there's a TNEF-attachment we should decode.
      
      Dim obAttachments
      Set obAttachments = obMessage.Attachments
      
      For i = 1 to obAttachments.Count 
         dim obAttachment
         set obAttachment = obAttachments.Item(i-1)
         
         If LCase(obAttachment.FileName) = "winmail.dat" Then
            Call ExtractTNEF(obMessage, obAttachment)
         End If
      Next
   End Sub
   
   Sub ExtractTNEF(obMessage, obAttachment)
      ' Extracts the TNEF attachments from a winmail.dat
      
      ' Generate a random string. Used for directory name.
      Dim obUtilities
      Set obUtilities = CreateObject("hMailServer.Utilities")
      Dim sGUID 
      sGUID = obUtilities.GenerateGUID
      Set obUtilities = Nothing
      
      ' Generate GUID-based temporary directory name
      Dim sOutPath
      sOutPath = g_sTempPath & "\" & sGUID
      
      ' Create the directory
      Dim obFS 
      Set obFS = CreateObject("Scripting.FileSystemObject")
      Call obFS.CreateFolder(sOutPath)
            
      ' Extract the file to a temporary directory.
      Dim sAttachmentFile
      sAttachmentFile = sOutPath & "\winmail.dat"
      Call obAttachment.SaveAs(sAttachmentFile)
      
      ' Create an output directory for the extracted
      ' attachments.
      Dim sAttachmentsDirctory
      sAttachmentsDirctory = sOutPath & "\output"
      Call obFS.CreateFolder(sAttachmentsDirctory)
      
      ' Run tnef-decoder.
      Dim sCommandLine
      sCommandLine = "java -classpath " & g_sTnefPath & " net.freeutils.tnef.TNEF " & sAttachmentFile & " " & sAttachmentsDirctory
      Call RunCommand(sCommandLine)
      
      ' Check the output directory for extracted files
      Dim obFiles
      Set obFiles = obFS.GetFolder(sAttachmentsDirctory).Files
      
      ' Go through the output directory, and see if theres
      ' any extracted ifles      
      Dim obFile
      For Each obFile in obFiles
        ' Add this as an attachment to the message,
        ' if it's not named winmail.dat. No point
        ' in adding itself...
        
        Call obMessage.Attachments.Add(obFile.Path)
      Next
      
      If obFiles.Count > 0 Then
         ' Delete the original attachment.
         obAttachment.Delete         
         
         ' Since the message has been modified, we need to save it.
         obMessage.Save  
      End If
      
      Set obFiles = Nothing
      
      ' Delete the temporary folder
      Call obFS.DeleteFolder(sOutPath)
   End Sub

   Function RunCommand(sCommand)
      ' Runs a command
      Dim obShell
      Set obShell = CreateObject("WScript.Shell")
      
      ' Run the command
      obShell.Run sCommand, 0, True
      
      Set obShell = Nothing
   End Function
