---
name: regression-first
description: >-
  Reproduce a bug with a failing regression test before fixing it, then confirm the
  fix turns it green. Covers both ways of running the suite: building and running it
  locally on Windows, and driving it through the GitHub Actions build when there is
  no Windows toolchain.
argument-hint: "Which bug are we fixing?"
disable-model-invocation: true
---

AGENTS.md asks for a regression test with every bug fix, written first and confirmed
failing before the fix, so the test is known to reproduce the issue. This is how to
actually do that.

Four steps: write the test, prove it fails, write the fix, prove it passes. Step 2 is
the one that gets skipped, and skipping it is what produces tests that pass no matter
what the server does.

## 1. Write the test

Put it in `hmailserver/test/RegressionTests/`, next to the existing tests for the same
protocol or area. Name it for the behaviour being asserted, not for the bug number.

Assert the specific response, not just that something failed - `StringAssert.Contains`
with the exact reply the server should send. Copy only the setup the test genuinely
needs; settings pasted in from a neighbouring test are noise, and settings left changed
can leak into later tests.

Add the tests that stop the fix from being "fixed" the wrong way. A test that a command
is now refused should be accompanied by one showing it is still accepted where it
should be, or the bug can be closed by breaking the feature.

## 2. Prove it fails

Check the platform. The repository only builds on Windows.

### On Windows - run it locally

Run `build/grant-service-control.ps1` once, elevated, if you have not already; it lets
the tests stop and start the service without a UAC prompt.

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build/build-tests.ps1
hmailserver\test\RegressionTests\packages\NUnit.ConsoleRunner.<version>\tools\nunit3-console.exe ^
  hmailserver\test\RegressionTests\bin\x64\Debug\RegressionTests.dll ^
  --where "class == RegressionTests.<Area>.<Fixture>"
```

The tests drive a live installed hMailServer over COM, so they test whatever is
currently registered - not your working tree. Before trusting a result after a server
change, run `build/build.ps1` and then `build/post-build.ps1`.

Because red comes back in seconds here, there is no reason to push a deliberately
failing commit. Verify red locally and push once. Keeping the tests in their own commit
is still worth doing for review, but that is a separate decision from proving the
repro.

### Anywhere else - drive it through CI

There is no Windows toolchain, so GitHub Actions is the only thing that can run
anything. Commit the tests alone and push.

- `build.yml` builds `master` and the `claude/**`, `feature/**`, `techdebt/**` and
  `bugfix/**` branches. It deliberately does not build pull requests, so a fork PR gets
  no CI at all - push to a branch in this repository.
- Budget about 20 minutes: ~13 minutes to build and package, ~6 to install and run the
  suite.
- Schedule a check-in past that with `send_later` and end the turn. Never poll with
  `sleep`.
- **Any push cancels the run in flight.** Do not push anything at all - not a comment
  fix, not a typo - while waiting on a result that matters, or the 20 minutes is spent
  for nothing and starts over.

Read the outcome from the `Tests / Regression tests` job. Its `Summarise the NUnit
result` step prints a Total/Passed/Failed/Skipped table and, when anything failed, a
`Failed tests` list with each assertion message; `get_job_logs` with `tail_lines` of
about 75 reaches it in one call.

### Then check the red is the right red

A failure is not a repro on its own.

- Exactly the new tests failed, and nothing else. Other failures mean the tests broke
  something incidental, and that has to be understood before continuing.
- Each failed on its assertion, with the message you expected - not on a timeout, a
  connection error, or a setup exception.

## 3. Write the fix

Keep it to what the failure needs. Fix the cause rather than the symptom: if the same
mistake is reachable from several call sites, guard the shared code rather than patching
the one path the test happens to exercise.

Never skip, disable or quarantine a test to get to green.

## 4. Prove it passes

Re-run the same way. Green means zero failures **and** the passed count up by exactly
the number of tests that were red - if it moved by more or less, something else changed
and needs explaining.

## Watch for test infrastructure that hides the red

Changing server behaviour can force edits to shared test helpers - `SMTPClientSimulator`,
`IMAPClientSimulator`, `TestSetup`, or an existing test that relied on the old
behaviour. Those edits are dangerous: made together with the fix, they can quietly turn
the new tests green for the wrong reason.

Put them in the tests-only commit and confirm they are no-ops against the unfixed
server. If a helper change cannot be a no-op, the behaviour change is larger than the
bug fix and deserves saying so explicitly.
