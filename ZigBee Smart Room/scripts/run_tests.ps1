$ErrorActionPreference = "Stop"
$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) { throw "Python 3.10+ is required" }
python -m unittest discover -s tests -v
