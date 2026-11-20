# ============================================================
# 一键上传脚本 - 本地代码修改完成后，快速提交并推送到远端仓库
#
# 用法（在 lhe7909 项目根目录下）：
#   1) .\git_push.ps1                -> 自动生成提交说明并推送
#   2) .\git_push.ps1 "新增XX功能"   -> 使用自定义提交说明并推送
# ============================================================

param(
    [string]$Message = ""
)

$git = 'D:\Git\bin\git.exe'
if (-not (Test-Path $git)) { $git = 'git' }

# 切换到脚本所在目录（项目根目录）
Set-Location -Path $PSScriptRoot

# 提交说明：未指定则自动生成
$Message = $Message.Trim()
if (-not $Message) {
    $Message = "Update: $(Get-Date -Format 'yyyy-MM-dd HH:mm')"
}

# 1. 暂存所有修改
Write-Host "`n[1/3] 暂存所有修改..." -ForegroundColor Cyan
& $git add -A

# 2. 检查是否有变更
$status = & $git status --porcelain
if (-not $status) {
    Write-Host "`n没有检测到任何代码变更，无需提交。`n" -ForegroundColor Yellow
    exit 0
}
& $git status --short

# 3. 提交
Write-Host "[2/3] 提交代码: $Message" -ForegroundColor Cyan
& $git commit -m $Message

# 4. 推送
Write-Host "[3/3] 推送到远端 dev-update 分支..." -ForegroundColor Cyan
& $git push origin dev-update

Write-Host "`n上传完成！如需合并到 main 主分支，请在 GitLab 发起 MR：`n" -ForegroundColor Green
Write-Host "  http://10.16.12.62/democode/lhe7909/-/merge_requests/new?merge_request%5Bsource_branch%5D=dev-update`n" -ForegroundColor Green
