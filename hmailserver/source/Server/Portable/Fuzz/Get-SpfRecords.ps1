#Requires -Version 7.0
<#
.SYNOPSIS
   Looks up the SPF record of every domain in a list and writes what it finds to
   a tab separated file.

.DESCRIPTION
   The fuzzers in this directory seed from the conformance suite, which is a test
   fixture rather than the real world. This collects the other kind of corpus: the
   records that domains actually publish, which is also the only thing that can
   say whether the parser rejects something the internet relies on.

   The input is a list of domains, one per line, with an optional header line -
   the format the Cloudflare Radar top sites export is in.

   The output has one line per record found, and one line per domain that has
   none, so that a run can be resumed and so that the shape of the whole list is
   kept rather than only its successes:

      <domain> TAB <status> TAB <record>

   where status is one of

      ok         exactly one v=spf1 record
      multiple   more than one, which RFC 7208 section 4.5 makes a permerror -
                 each one is on its own line
      none       the domain answered, and published no v=spf1 record
      nxdomain   the domain does not exist
      error:<x>  the lookup failed; x is the resolver's error id

   Tabs, newlines and backslashes in a record are escaped as \t, \n, \r and \\,
   so a line is always a line. Nothing else is altered - a record is written
   exactly as published, including any leading space or odd casing, because that
   is the input the parser has to handle.

   Multi string TXT records are concatenated without a separator, as section 3.3
   requires.

.PARAMETER InputPath
   The list of domains. One per line; a first line of "domain" is skipped.

.PARAMETER OutputPath
   Where to write. If the file exists its domains are read first and skipped,
   so an interrupted run continues where it stopped.

.PARAMETER Concurrency
   How many lookups are in flight at once. 64 is polite to a local resolver and
   keeps a 100k list to well under an hour; a public resolver may rate limit
   long before that.

.PARAMETER Limit
   Stop after this many domains from the input. 0 means all of them. Use a small
   number for a trial run.

.PARAMETER Server
   Resolvers to ask, instead of the ones the machine is configured with.

.PARAMETER Retries
   How many times to ask again after a timeout or a server failure. Name errors
   and empty answers are answers, and are never retried.

.PARAMETER BatchSize
   How many domains are resolved between writes. A batch is also the unit of
   progress and of resumption, so a smaller one loses less to an interruption
   and costs a little more in file writes.

.EXAMPLE
   .\Get-SpfRecords.ps1 -InputPath top-100000.csv -OutputPath spf-100k.tsv

.EXAMPLE
   .\Get-SpfRecords.ps1 -InputPath top-100000.csv -OutputPath trial.tsv -Limit 500
#>
[CmdletBinding()]
param(
   [Parameter(Mandatory = $true)]
   [string] $InputPath,

   [Parameter(Mandatory = $true)]
   [string] $OutputPath,

   [ValidateRange(1, 512)]
   [int] $Concurrency = 64,

   [ValidateRange(0, [int]::MaxValue)]
   [int] $Limit = 0,

   [string[]] $Server,

   [ValidateRange(0, 10)]
   [int] $Retries = 2,

   [ValidateRange(1, 100000)]
   [int] $BatchSize = 2000
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# Resolve-DnsName is in a module marked as Windows PowerShell only. It runs fine
# here, and loading it natively is what keeps the parallel lookups parallel -
# without this PowerShell reaches it through a compatibility session instead.
Import-Module DnsClient -SkipEditionCheck

$InputPath = (Resolve-Path -LiteralPath $InputPath).Path

# A relative output path has to be made absolute here, and against PowerShell's
# location rather than .NET's. The two disagree: .NET keeps the directory the
# process started in, so a StreamWriter opened on a relative path writes
# somewhere other than where the cmdlets above it look.
$OutputPath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputPath)

Write-Host "Reading $InputPath"

$domains = [System.Collections.Generic.List[string]]::new()
$seen = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)

foreach ($line in [System.IO.File]::ReadLines($InputPath))
{
   # The export has a single "domain" column. Take the first field of any line,
   # so a file that carries a rank alongside it works too.
   $domain = $line.Split(',')[0].Trim().Trim('"')

   if ($domain.Length -eq 0 -or $domain -eq 'domain')
   {
      continue
   }

   if ($seen.Add($domain))
   {
      $domains.Add($domain)
   }
}

if ($Limit -gt 0 -and $domains.Count -gt $Limit)
{
   $domains.RemoveRange($Limit, $domains.Count - $Limit)
}

Write-Host "$($domains.Count) domains"

# A previous run's output is the record of what no longer needs asking. Both the
# domains that answered and the ones that failed are kept: a failure is a result,
# and asking a dead domain again on every resume would cost more than it tells.
$done = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)

if (Test-Path -LiteralPath $OutputPath)
{
   foreach ($line in [System.IO.File]::ReadLines($OutputPath))
   {
      $tab = $line.IndexOf("`t")

      if ($tab -gt 0)
      {
         [void] $done.Add($line.Substring(0, $tab))
      }
   }

   Write-Host "$($done.Count) already in $OutputPath, resuming"
}
else
{
   $directory = Split-Path -Parent $OutputPath

   if ($directory -and -not (Test-Path -LiteralPath $directory))
   {
      [void] (New-Item -ItemType Directory -Path $directory)
   }
}

$pending = @($domains | Where-Object { -not $done.Contains($_) })

Write-Host "$($pending.Count) to look up, $Concurrency at a time"

$lookup = {
   $domain = $_

   if (-not (Get-Module -Name DnsClient))
   {
      Import-Module DnsClient -SkipEditionCheck
   }

   $server = $using:Server
   $retries = $using:Retries

   $arguments = @{
      Name        = $domain
      Type        = 'TXT'
      DnsOnly     = $true
      NoHostsFile = $true
      ErrorAction = 'Stop'
   }

   if ($server)
   {
      $arguments['Server'] = $server
   }

   $status = $null
   $answers = $null

   for ($attempt = 0; $attempt -le $retries; $attempt++)
   {
      try
      {
         $answers = @(Resolve-DnsName @arguments)
         $status = $null
         break
      }
      catch
      {
         # The resolver says which kind of failure this was in the error id, for
         # example DNS_ERROR_RCODE_NAME_ERROR. Keep that: a domain that does not
         # exist is a different thing from one that could not be reached, and
         # only the second is worth asking again.
         $id = ($_.FullyQualifiedErrorId -split ',')[0]

         if ($id -eq 'DNS_ERROR_RCODE_NAME_ERROR')
         {
            $status = 'nxdomain'
            break
         }

         $status = "error:$id"

         if ($attempt -lt $retries)
         {
            Start-Sleep -Milliseconds (200 * ($attempt + 1))
         }
      }
   }

   $escape = {
      param([string] $text)

      $text.Replace('\', '\\').Replace("`t", '\t').Replace("`r", '\r').Replace("`n", '\n')
   }

   if ($status)
   {
      return "$domain`t$status`t"
   }

   # A TXT query can be answered with the CNAME chain that led to the records, so
   # take only what was asked for. Section 3.3: the strings of one record are
   # joined with nothing between them.
   $records = @(
      $answers |
         Where-Object { $_.QueryType -eq 'TXT' } |
         ForEach-Object { ($_.Strings -join '') }
   )

   $spf = @($records | Where-Object { $_ -match '^\s*v=spf1(\s|$)' })

   if ($spf.Count -eq 0)
   {
      return "$domain`tnone`t"
   }

   $status = if ($spf.Count -eq 1) { 'ok' } else { 'multiple' }

   $spf | ForEach-Object { "$domain`t$status`t$(& $escape $_)" }
}

$writer = [System.IO.StreamWriter]::new($OutputPath, $true, [System.Text.UTF8Encoding]::new($false))
$started = Get-Date
$completed = 0

try
{
   for ($offset = 0; $offset -lt $pending.Count; $offset += $BatchSize)
   {
      $count = [Math]::Min($BatchSize, $pending.Count - $offset)
      $batch = $pending[$offset..($offset + $count - 1)]

      $lines = $batch | ForEach-Object -Parallel $lookup -ThrottleLimit $Concurrency

      foreach ($line in $lines)
      {
         $writer.WriteLine($line)
      }

      $writer.Flush()

      $completed += $count
      $elapsed = (Get-Date) - $started
      $rate = $completed / [Math]::Max($elapsed.TotalSeconds, 1)
      $left = [TimeSpan]::FromSeconds(($pending.Count - $completed) / [Math]::Max($rate, 0.001))

      Write-Host ("{0}/{1}  {2:n0}/s  {3:hh\:mm\:ss} left" -f
         $completed, $pending.Count, $rate, $left)
   }
}
finally
{
   $writer.Dispose()
}

# What the run found, which is also the first thing worth knowing about the data:
# a large "none" is expected, and a large "error" means the resolver gave up
# rather than that the domains did.
$summary = [System.Collections.Generic.Dictionary[string, int]]::new()

foreach ($line in [System.IO.File]::ReadLines($OutputPath))
{
   $fields = $line.Split("`t")

   if ($fields.Count -lt 2)
   {
      continue
   }

   $status = if ($fields[1].StartsWith('error:')) { 'error' } else { $fields[1] }

   $summary[$status] = 1 + $(if ($summary.ContainsKey($status)) { $summary[$status] } else { 0 })
}

Write-Host ''
Write-Host "Wrote $OutputPath"

foreach ($entry in $summary.GetEnumerator() | Sort-Object -Property Value -Descending)
{
   Write-Host ("  {0,-10} {1,8:n0}" -f $entry.Key, $entry.Value)
}
