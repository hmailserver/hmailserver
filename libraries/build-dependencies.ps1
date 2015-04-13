Function BuildBoost($boostPath)
{
    $bjampath = join-path -path $boostpath -childpath "bjam.exe"
    $boostStageLibPath = Join-Path -Path $boostPath -ChildPath "stage/lib"
    $boostStageLib32Path = Join-Path -Path $boostPath -ChildPath "stage/lib32"
    $boostStageLib64Path = Join-Path -Path $boostPath -ChildPath "stage/lib64"

    If (Test-Path $boostStageLib32Path){
        Remove-Item $boostStageLib32Path -Recurse
    }

    If (Test-Path $boostStageLib64Path){
        Remove-Item $boostStageLib64Path -Recurse
    }

    Write-Host "Bootstrapping Boost"
    $bootstrapPath = Join-Path -Path $boostPath -ChildPath "bootstrap.bat"

    pushd $boostPath
    
    cmd /c $bootstrapPath
    $bootstrapResult = $LastExitCode;
        
	Write-Host ".bat execution completed"
	
	if ($bootstrapResult -ne 0)
    {
        Write-Host "Boost bootstrap failed with error code $bootstrapResult."
        return -1;
    }
    
    popd

    $bjamcleanprocess = start-process $bjampath -argumentlist "--clean" -workingdirectory $boostpath -passthru -wait -NoNewWindow
    $bjamcleanresult = $bjamcleanprocess.exitcode

    if ($bjamcleanresult -ne 0)
    {
        Throw "boost bjam clean failed with error code $bjamcleanresult."
    }

    Write-Host "Building Boost 32bit"
    $bjam32Process = Start-Process $bjamPath -ArgumentList "--toolset=msvc-12.0 --build-type=complete" -WorkingDirectory $boostPath -PassThru -Wait -NoNewWindow
    $bjam32Result = $bjam32Process.ExitCode

    if ($bjam32Result -ne 0)
    {
        Throw "Boost bjam failed with error code $bjam32Result."
    }

    Rename-Item $boostStageLibPath $boostStageLib32Path

    Write-Host "Building Boost 64bit"
    $bjam64Process = Start-Process $bjamPath -ArgumentList "--toolset=msvc-12.0 --build-type=complete address-model=64" -WorkingDirectory $boostPath -PassThru -Wait -NoNewWindow
    $bjam64Result = $bjam64Process.ExitCode

    if ($bjam64Result -ne 0)
    {
        Throw "Boost bjam failed with error code $bjam64Result."
        
    }

    Rename-Item $boostStageLibPath $boostStageLib64Path
}



$libsPath = $env:hMailServerLibs

if ([string]::isnullorempty($libspath))
{
    Throw "The environment variable hMailServerLibs was not found. Please create it."
}

if (!(Test-Path $libsPath))
{
    Throw "The environment variable hMailServerLibs was found, but the folder it was pointing at ($libsPath) was not. Please create it."
}

$boostPath = Join-Path -Path $libsPath -ChildPath "boost_1_57_0"

if (!(Test-Path $boostPath))
{
    Throw "Boost was not found in the expected path ($boostPath)"
}

BuildBoost($boostPath);





