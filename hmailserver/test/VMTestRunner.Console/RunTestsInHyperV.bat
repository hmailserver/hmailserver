@echo off

C:
cd \nunit

set ASSEMBLY=%~1
if "%ASSEMBLY%"=="" set ASSEMBLY=RegressionTests.dll

if /I "%~2"=="IncludeStress" (
    nunit3-console.exe %ASSEMBLY% --where "(cat == Stress or cat != Stress) and cat != Details" --labels=OnOutputOnly --out=TestResult.log
) else (
    nunit3-console.exe %ASSEMBLY% --labels=OnOutputOnly --out=TestResult.log
)
