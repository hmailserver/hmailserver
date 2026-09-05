function Install-ChocoPackage {
    # Installs a Chocolatey package at a pinned version, retrying the download.
    #
    # Both packages the CI build takes from Chocolatey are large downloads from a third-party
    # CDN, which is the one part of the build that fails for reasons that have nothing to do
    # with the code being built. A transient failure there should cost seconds, not a rerun of
    # the whole job, so this retries with a growing delay before giving up.
    #
    # The version is mandatory on purpose. An unpinned "choco install" takes whatever the feed
    # is serving that day, which is both a supply-chain question and a practical one: a new
    # package version can move the files the build then goes looking for.
    param(
        [Parameter(Mandatory = $true)][string]$Package,
        [Parameter(Mandatory = $true)][string]$Version,
        # innosetup needs this: the runner image already carries a newer Inno Setup, and
        # stepping back to the pinned version counts as a downgrade.
        [switch]$AllowDowngrade,
        [int]$Attempts = 3
    )

    $arguments = @('install', $Package, "--version=$Version", '--yes', '--no-progress')
    if ($AllowDowngrade) { $arguments += '--allow-downgrade' }

    for ($attempt = 1; $attempt -le $Attempts; $attempt++) {
        if ($attempt -gt 1) {
            $delay = 15 * ($attempt - 1)
            Write-Host "Retrying in $delay seconds ($attempt of $Attempts)..."
            Start-Sleep -Seconds $delay
        }

        & choco @arguments
        if ($LASTEXITCODE -eq 0) { return }

        Write-Host "choco install $Package $Version failed with exit code $LASTEXITCODE."
    }

    throw "Installing $Package $Version failed after $Attempts attempts."
}
