function Find-MsBuild {
    # The range covers VS2019 through VS2026. MSBuild 17 and 18 build the v142 projects
    # hMailServer uses just as well as MSBuild 16 does, provided the v142 toolset is
    # installed - which matters because the GitHub Actions runner ships one Visual Studio
    # only: VS2026 on windows-2025-vs2026, with the v142 build tools beside it.
    param(
        [string]$VsWhereMinVersion = '[16.0,19.0)'
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
