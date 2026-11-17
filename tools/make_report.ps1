# 把 logs/run.log 渲染成可视化报告（logs/report.html）
# 用法：powershell -NoProfile -ExecutionPolicy Bypass -File tools\make_report.ps1
param(
    [string]$LogPath = "logs/run.log",
    [string]$OutPath = "logs/report.html"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $LogPath)) {
    Write-Host "[report] 找不到日志：$LogPath ，请先运行 build\ros_sim.exe" -ForegroundColor Yellow
    exit 1
}

$lines = Get-Content -Path $LogPath -Encoding UTF8
$rx = '^\[(?<time>\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\]\[(?<level>INFO|WARN|ERROR)\]\[(?<module>[^\]]+)\] (?<msg>.*)$'

$entries = New-Object System.Collections.ArrayList
foreach ($line in $lines) {
    if ([string]::IsNullOrWhiteSpace($line)) { continue }
    $m = [regex]::Match($line, $rx)
    if ($m.Success) {
        [void]$entries.Add([pscustomobject]@{
            time   = $m.Groups['time'].Value
            level  = $m.Groups['level'].Value
            module = $m.Groups['module'].Value
            msg    = $m.Groups['msg'].Value
        })
    } else {
        [void]$entries.Add([pscustomobject]@{ time = ''; level = 'RAW'; module = ''; msg = $line })
    }
}

function ConvertTo-HtmlSafe([string]$s) {
    if ($null -eq $s) { return '' }
    return $s.Replace('&', '&amp;').Replace('<', '&lt;').Replace('>', '&gt;').Replace('"', '&quot;')
}

$total = $entries.Count
$info  = @($entries | Where-Object { $_.level -eq 'INFO' }).Count
$warn  = @($entries | Where-Object { $_.level -eq 'WARN' }).Count
$errorCount = @($entries | Where-Object { $_.level -eq 'ERROR' }).Count

$modules = @($entries | Where-Object { $_.module } | Select-Object -ExpandProperty module -Unique)
$moduleChips = ($modules | ForEach-Object {
    "<button class=""chip"" data-module=""" + (ConvertTo-HtmlSafe $_) + """>" + (ConvertTo-HtmlSafe $_) + "</button>"
}) -join "`n"

$rows = New-Object System.Text.StringBuilder
foreach ($e in $entries) {
    $cls = switch ($e.level) {
        'INFO'  { 'lv-info' }
        'WARN'  { 'lv-warn' }
        'ERROR' { 'lv-error' }
        default { 'lv-raw' }
    }
    $msg = ConvertTo-HtmlSafe $e.msg
    $msg = [regex]::Replace($msg, '\[(text|temperature)\]', '<span class="tag">[$1]</span>')

    [void]$rows.AppendLine(
        '<li class="row ' + $cls + '" data-level="' + $e.level + '" data-module="' + (ConvertTo-HtmlSafe $e.module) + '">' +
        '<span class="t">' + $e.time + '</span>' +
        '<span class="lv">' + $e.level + '</span>' +
        '<span class="mod">' + (ConvertTo-HtmlSafe $e.module) + '</span>' +
        '<span class="msg">' + $msg + '</span>' +
        '</li>')
}

$generated = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
$source = (Resolve-Path $LogPath).Path

$html = @"
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<title>C++ 模拟 ROS 节点系统 · 运行报告</title>
<style>
  :root {
    --bg: #0f1420; --panel: #161d2b; --line: #232c3d; --text: #e6edf6; --dim: #8b9bb4;
    --info: #4cc2ff; --warn: #ffc857; --error: #ff6b6b; --ok: #4ade80;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0; padding: 32px 24px 64px;
    background: radial-gradient(1200px 600px at 20% -10%, #1b2740 0%, var(--bg) 60%);
    color: var(--text);
    font-family: "Segoe UI", "Microsoft YaHei", system-ui, -apple-system, sans-serif;
  }
  .wrap { max-width: 1180px; margin: 0 auto; }
  h1 { font-size: 26px; margin: 0 0 6px; letter-spacing: .5px; }
  .sub { color: var(--dim); font-size: 13px; margin-bottom: 22px; }
  .sub code { color: #9ecbff; }

  .cards { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 14px; margin-bottom: 22px; }
  .card {
    background: linear-gradient(180deg, #1a2233, #141a27);
    border: 1px solid var(--line); border-radius: 14px; padding: 16px 18px;
  }
  .card .k { color: var(--dim); font-size: 12px; letter-spacing: .8px; text-transform: uppercase; }
  .card .v { font-size: 30px; font-weight: 700; margin-top: 6px; font-variant-numeric: tabular-nums; }
  .card.total .v { color: #9ecbff; }
  .card.info  .v { color: var(--info); }
  .card.warn  .v { color: var(--warn); }
  .card.error .v { color: var(--error); }

  .toolbar { display: flex; flex-wrap: wrap; gap: 10px; align-items: center; margin-bottom: 14px; }
  .chip, .filter {
    border: 1px solid var(--line); background: #141a27; color: var(--dim);
    border-radius: 999px; padding: 7px 14px; font-size: 13px; cursor: pointer;
    transition: all .15s ease;
  }
  .chip:hover, .filter:hover { color: var(--text); border-color: #33415c; }
  .filter.active { background: #1e2a3f; color: var(--text); border-color: #3b5075; }
  input[type=search] {
    flex: 1 1 220px; min-width: 200px; background: #141a27; border: 1px solid var(--line);
    color: var(--text); border-radius: 999px; padding: 8px 14px; outline: none; font-size: 13px;
  }
  input[type=search]:focus { border-color: #3b5075; }

  ul.log { list-style: none; margin: 0; padding: 0; border: 1px solid var(--line); border-radius: 14px; overflow: hidden; }
  li.row {
    display: grid; grid-template-columns: 172px 70px 110px 1fr; gap: 12px; align-items: baseline;
    padding: 10px 16px; border-top: 1px solid var(--line); font-size: 13.5px;
    background: #121824; font-family: "Cascadia Mono", Consolas, "Microsoft YaHei", monospace;
  }
  li.row:first-child { border-top: none; }
  li.row:hover { background: #17202f; }
  li.row .t { color: #6f8199; font-variant-numeric: tabular-nums; white-space: nowrap; }
  li.row .lv { font-weight: 700; font-size: 11px; letter-spacing: .8px; }
  li.row .mod { color: #a8b8cc; }
  li.row .msg { color: #dbe6f3; word-break: break-word; }
  li.row.lv-info  .lv { color: var(--info); }
  li.row.lv-warn  .lv { color: var(--warn); }
  li.row.lv-error .lv { color: var(--error); }
  li.row.lv-warn  { box-shadow: inset 3px 0 0 var(--warn); }
  li.row.lv-error { box-shadow: inset 3px 0 0 var(--error); }
  li.row.lv-info  { box-shadow: inset 3px 0 0 #22304a; }
  .tag { color: #7ee0a1; }
  .hidden { display: none !important; }
  footer { color: #5d6d84; font-size: 12px; margin-top: 18px; text-align: right; }
</style>
</head>
<body>
<div class="wrap">
  <h1>C++ 模拟 ROS 节点系统 · 运行报告</h1>
  <div class="sub">数据来源：<code>$source</code> ｜ 生成时间：$generated ｜ 共 $total 条记录</div>

  <div class="cards">
    <div class="card total"><div class="k">Total</div><div class="v">$total</div></div>
    <div class="card info"><div class="k">Info</div><div class="v">$info</div></div>
    <div class="card warn"><div class="k">Warn</div><div class="v">$warn</div></div>
    <div class="card error"><div class="k">Error</div><div class="v">$errorCount</div></div>
  </div>

  <div class="toolbar">
    <button class="filter active" data-level="all">全部</button>
    <button class="filter" data-level="INFO">INFO</button>
    <button class="filter" data-level="WARN">WARN</button>
    <button class="filter" data-level="ERROR">ERROR</button>
    <input type="search" id="q" placeholder="搜索内容 / 模块…">
  </div>
  <div class="toolbar">
    <button class="chip" data-module="all">全部模块</button>
    $moduleChips
  </div>

  <ul class="log" id="log">
$($rows.ToString())
  </ul>

  <footer>由 tools/make_report.ps1 生成 · 仅用于展示运行日志</footer>
</div>

<script>
  var level = 'all';
  var module = 'all';
  var q = '';

  function apply() {
    document.querySelectorAll('li.row').forEach(function (li) {
      var okLevel  = (level === 'all') || (li.dataset.level === level);
      var okModule = (module === 'all') || (li.dataset.module === module);
      var okText   = !q || li.textContent.toLowerCase().indexOf(q) >= 0;
      li.classList.toggle('hidden', !(okLevel && okModule && okText));
    });
  }

  document.querySelectorAll('.filter').forEach(function (b) {
    b.addEventListener('click', function () {
      document.querySelectorAll('.filter').forEach(function (x) { x.classList.remove('active'); });
      b.classList.add('active');
      level = b.dataset.level;
      apply();
    });
  });

  document.querySelectorAll('.chip').forEach(function (b) {
    b.addEventListener('click', function () {
      module = b.dataset.module;
      apply();
    });
  });

  document.getElementById('q').addEventListener('input', function (e) {
    q = e.target.value.toLowerCase();
    apply();
  });
</script>
</body>
</html>
"@

$dir = Split-Path -Parent $OutPath
if ($dir -and -not (Test-Path $dir)) { New-Item -ItemType Directory -Force -Path $dir | Out-Null }
[System.IO.File]::WriteAllText((Join-Path (Get-Location) $OutPath), $html, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "[report] 已生成 $OutPath（$total 条记录：INFO $info / WARN $warn / ERROR $errorCount）" -ForegroundColor Green
