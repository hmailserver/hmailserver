function Find-MsBuild {
    # The range covers VS2019 and VS2022. MSBuild 17 builds the v142 projects hMailServer
    # uses just as well as MSBuild 16 does, provided the v142 toolset is installed - which
    # matters because VS2022 is what the GitHub Actions windows-2022 image ships, with no
    # VS2019 at all.
    param(
        [string]$VsWhereMinVersion = '[16.0,18.0)'
    )

    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    $msbuild = $null
    if (Test-Path $vswhere) {
        try {
            $msbuild = & $vswhere -version $VsWhereMinVersion -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
        } catch {
            $msbuild = $null
        }
    } else {
        Write-Verbose "vswhere not found at $vswhere"
    }

    if (-not $msbuild) {
        $msbuildCmd = Get-Command msbuild.exe -ErrorAction SilentlyContinue
        if ($msbuildCmd) { $msbuild = $msbuildCmd.Source }
    }

    return $msbuild
}
