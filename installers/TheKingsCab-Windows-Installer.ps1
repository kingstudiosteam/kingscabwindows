# The King's Cab - Windows PowerShell Installer
# Self-contained installer that works without additional tools

param(
    [switch]$Uninstall,
    [switch]$Silent
)

$PluginName = "The King's Cab"
$Version = "1.0.0"
$Company = "King Studios"

# Define installation paths
$VST3Path = "$env:ProgramFiles\Common Files\VST3"
$AAXPath = "$env:ProgramFiles\Common Files\Avid\Audio\Plug-Ins"
$AppDataPath = "$env:APPDATA\King Studios\$PluginName"

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$PluginDir = Join-Path $ScriptDir "plugins"

function Write-Status {
    param($Message, $Color = "Green")
    if (-not $Silent) {
        Write-Host "🎛️  $Message" -ForegroundColor $Color
    }
}

function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Install-Plugin {
    Write-Status "Installing The King's Cab Plugin..." "Cyan"
    
    # Check for administrator privileges
    if (-not (Test-Administrator)) {
        Write-Status "ERROR: Administrator privileges required!" "Red"
        Write-Status "Please run PowerShell as Administrator and try again." "Yellow"
        exit 1
    }
    
    # Create directories
    Write-Status "Creating plugin directories..."
    New-Item -ItemType Directory -Force -Path $VST3Path | Out-Null
    New-Item -ItemType Directory -Force -Path $AAXPath | Out-Null
    New-Item -ItemType Directory -Force -Path $AppDataPath | Out-Null
    
    # Install VST3 plugin
    $VST3Source = Join-Path $PluginDir "VST3\$PluginName.vst3"
    $VST3Dest = Join-Path $VST3Path "$PluginName.vst3"
    
    if (Test-Path $VST3Source) {
        Write-Status "Installing VST3 plugin..."
        if (Test-Path $VST3Dest) {
            Remove-Item $VST3Dest -Recurse -Force
        }
        Copy-Item $VST3Source $VST3Dest -Recurse -Force
        Write-Status "✅ VST3 plugin installed to: $VST3Dest"
    } else {
        Write-Status "⚠️  VST3 plugin not found at: $VST3Source" "Yellow"
    }
    
    # Install AAX plugin
    $AAXSource = Join-Path $PluginDir "AAX\$PluginName.aaxplugin"
    $AAXDest = Join-Path $AAXPath "$PluginName.aaxplugin"
    
    if (Test-Path $AAXSource) {
        Write-Status "Installing AAX plugin..."
        if (Test-Path $AAXDest) {
            Remove-Item $AAXDest -Recurse -Force
        }
        Copy-Item $AAXSource $AAXDest -Recurse -Force
        Write-Status "✅ AAX plugin installed to: $AAXDest"
    } else {
        Write-Status "⚠️  AAX plugin not found at: $AAXSource" "Yellow"
    }
    
    # Create registry entries
    Write-Status "Creating registry entries..."
    $RegPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$PluginName"
    New-Item -Path $RegPath -Force | Out-Null
    Set-ItemProperty -Path $RegPath -Name "DisplayName" -Value $PluginName
    Set-ItemProperty -Path $RegPath -Name "DisplayVersion" -Value $Version
    Set-ItemProperty -Path $RegPath -Name "Publisher" -Value $Company
    Set-ItemProperty -Path $RegPath -Name "URLInfoAbout" -Value "https://www.kingstudiospa.com"
    Set-ItemProperty -Path $RegPath -Name "UninstallString" -Value "powershell.exe -ExecutionPolicy Bypass -File `"$($MyInvocation.MyCommand.Path)`" -Uninstall"
    Set-ItemProperty -Path $RegPath -Name "NoModify" -Value 1 -Type DWord
    Set-ItemProperty -Path $RegPath -Name "NoRepair" -Value 1 -Type DWord
    
    Write-Status "✅ Installation completed successfully!" "Green"
    Write-Status ""
    Write-Status "Plugin installed for:" "Cyan"
    Write-Status "  • All VST3 compatible DAWs (Reaper, Cubase, Studio One, etc.)"
    Write-Status "  • Pro Tools (AAX format)"
    Write-Status ""
    Write-Status "🚀 Restart your DAW to load The King's Cab plugin!"
    Write-Status "🎸 Visit https://www.kingstudiospa.com/store for premium IR collections"
}

function Uninstall-Plugin {
    Write-Status "Uninstalling The King's Cab Plugin..." "Yellow"
    
    if (-not (Test-Administrator)) {
        Write-Status "ERROR: Administrator privileges required for uninstall!" "Red"
        exit 1
    }
    
    # Remove plugin files
    $VST3Dest = Join-Path $VST3Path "$PluginName.vst3"
    $AAXDest = Join-Path $AAXPath "$PluginName.aaxplugin"
    
    if (Test-Path $VST3Dest) {
        Remove-Item $VST3Dest -Recurse -Force
        Write-Status "✅ VST3 plugin removed"
    }
    
    if (Test-Path $AAXDest) {
        Remove-Item $AAXDest -Recurse -Force
        Write-Status "✅ AAX plugin removed"
    }
    
    # Remove application data
    if (Test-Path $AppDataPath) {
        Remove-Item $AppDataPath -Recurse -Force
        Write-Status "✅ Application data removed"
    }
    
    # Remove registry entries
    $RegPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$PluginName"
    if (Test-Path $RegPath) {
        Remove-Item $RegPath -Force
        Write-Status "✅ Registry entries removed"
    }
    
    Write-Status "✅ Uninstallation completed successfully!" "Green"
}

function Show-Header {
    if (-not $Silent) {
        Write-Host ""
        Write-Host "🎛️  ===============================================" -ForegroundColor Cyan
        Write-Host "🎛️   THE KING'S CAB - PROFESSIONAL IR LOADER" -ForegroundColor Cyan
        Write-Host "🎛️  ===============================================" -ForegroundColor Cyan
        Write-Host "🎛️   Version: $Version" -ForegroundColor White
        Write-Host "🎛️   Company: $Company" -ForegroundColor White
        Write-Host "🎛️   Website: https://www.kingstudiospa.com" -ForegroundColor White
        Write-Host "🎛️  ===============================================" -ForegroundColor Cyan
        Write-Host ""
    }
}

# Main execution
Show-Header

if ($Uninstall) {
    Uninstall-Plugin
} else {
    Install-Plugin
}

if (-not $Silent) {
    Write-Host ""
    Write-Host "Press any key to exit..." -ForegroundColor Gray
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}
