$RunCount = 0;

do
{
    echo "Ran tests $RunCount times...";
    Get-Date

    ..\..\..\libraries\nunit-2.6.3\nunit-console-x86.exe .\Bin\Debug\RegressionTests.dll /config=Release /labels /stoponerror /out=TestResult.log
    $RunCount++;
    
}
while ($LASTEXITCODE -eq 0)