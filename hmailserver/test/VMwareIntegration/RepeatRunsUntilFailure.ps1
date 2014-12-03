cd .\VMWareIntegration.Console\Bin\Debug\
$RunCount = 0;

do
{
    echo "Ran tests $RunCount times...";
    Get-Date
    
    .\Console.exe "C:\temp\hMailServer-5.6.1-B2151.exe" "C:\temp\hMailServer-5.6.1-B2151.exe-%TIMESTAMP%.log"
    
    $RunCount++;
}
while ($LASTEXITCODE -eq 0)