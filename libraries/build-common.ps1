<#
.SYNOPSIS
    Shared build logging helpers for the hMailServer library build scripts.

.DESCRIPTION
    Dot-sourced by build-openssl.ps1 and build-pgsql.ps1. Provides a single build
    log (mirrored to a file next to the calling script and to the console) plus a
    helper to run a native build step with its stdout+stderr captured to that log.

    Usage from a build script:

        . (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")
        Start-BuildLog -LogPath (Join-Path $PSScriptRoot "build-openssl.log") -Title "OpenSSL 3.5.7 build log"
        Write-Log "Doing something"
        Invoke-BuildStep "Compiling" { nmake install_sw }
        if ($LastExitCode -ne 0) { Throw "..." }

    The log path/encoding are held in script scope. Because this file is dot-sourced,
    those variables and the functions live in the caller's script scope, so
    Start-BuildLog and the helpers all share the same state.
#>

# All log writes use this one encoding. Under Windows PowerShell 5.1 the various
# file cmdlets default to *different* encodings (Set-Content/Add-Content ->
# ANSI, Tee-Object -FilePath -> UTF-16LE), so mixing them produces a log where
# some lines render with a NUL between every character. Pin everything to UTF-8.
$script:BuildLogEncoding = "UTF8"
$script:BuildLogPath = $null

# Initialize the build log: record its path and write the header line. Call once
# before Write-Log / Invoke-BuildStep.
function Start-BuildLog
{
    param(
        [Parameter(Mandatory = $true)][string]$LogPath,
        [Parameter(Mandatory = $true)][string]$Title
    )
    $script:BuildLogPath = $LogPath
    Set-Content -Path $LogPath -Value "$Title - started $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -Encoding $script:BuildLogEncoding
}

# Write a message to both the console and the log file.
function Write-Log
{
    param([string]$Message)
    Write-Host $Message
    Add-Content -Path $script:BuildLogPath -Value $Message -Encoding $script:BuildLogEncoding
}

# Run a build step, mirroring its stdout+stderr to the console and the log file.
# The step's native exit code is left in $LastExitCode for the caller to check.
function Invoke-BuildStep
{
    param(
        [string]$Description,
        [scriptblock]$Command
    )
    Write-Log $Description
    Add-Content -Path $script:BuildLogPath -Value "----- $Description -----" -Encoding $script:BuildLogEncoding
    # Merge the step's stderr into the output stream so it is logged too. Native
    # tools (nmake, the compiler invoked by build.pl) legitimately write
    # progress/warnings to stderr; under $ErrorActionPreference='Stop' a
    # 2>&1-redirected stderr line is otherwise turned into a terminating
    # NativeCommandError before we can inspect the exit code. Force Continue for
    # just this pipeline; the caller still gates on $LastExitCode.
    #
    # We deliberately do NOT use 'Tee-Object -FilePath' here: on Windows
    # PowerShell 5.1 it has no -Encoding switch and always writes UTF-16LE, which
    # corrupts a log the rest of the script writes as UTF-8. Instead echo each
    # line to the console and append it to the log with the shared encoding.
    #
    # The 2>&1 stream carries stdout lines as plain strings but stderr lines as
    # ErrorRecords (native tools such as cl.exe write the current source file
    # name to stderr). Casting such a record to [string] yields the useless text
    # "System.Management.Automation.RemoteException"; the real stderr text is in
    # its .Exception.Message, so pull that out explicitly.
    #
    # Write through a single StreamWriter held open for the whole step rather than
    # an Add-Content call per line: these builds emit many thousands of lines and a
    # per-line open/seek/close is needless disk churn. AutoFlush keeps the log
    # tailable live (Get-Content -Wait). UTF8Encoding($false) => no BOM, matching
    # the UTF-8 the rest of the script writes.
    $prevEAP = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $writer = New-Object System.IO.StreamWriter($script:BuildLogPath, $true, (New-Object System.Text.UTF8Encoding($false)))
    $writer.AutoFlush = $true
    try
    {
        & $Command 2>&1 | ForEach-Object {
            if ($_ -is [System.Management.Automation.ErrorRecord])
            {
                $line = $_.Exception.Message
            }
            else
            {
                $line = [string]$_
            }
            Write-Host $line
            $writer.WriteLine($line)
        }
    }
    finally
    {
        $writer.Close()
        $ErrorActionPreference = $prevEAP
    }
}
