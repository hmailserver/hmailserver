@echo off

C:
cd \nunit

if /I "%~1"=="IncludeStress" (
    nunit3-console.exe RegressionTests.dll --where "cat == Stress or cat != Stress" --labels=OnOutputOnly --out=TestResult.log
) else (
    nunit3-console.exe RegressionTests.dll --labels=OnOutputOnly --out=TestResult.log
)