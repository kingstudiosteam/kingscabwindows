; The King's Cab - Professional Windows Installer Script
; This creates a traditional Windows .exe installer
; Compile with Inno Setup Compiler

#define AppName "The King's Cab"
#define AppVersion "1.0.0"
#define AppPublisher "King Studios"
#define AppURL "https://www.kingstudiospa.com"
#define AppExeName "The King's Cab.vst3"

[Setup]
AppId={{B8C7E8A2-9F4D-4A5B-8C3E-1F2D3E4F5A6B}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
DefaultDirName={autopf}\King Studios\{#AppName}
DefaultGroupName={#AppName}
LicenseFile=license.txt
InfoBeforeFile=readme.txt
OutputBaseFilename=TheKingsCab-{#AppVersion}-Windows-Setup
SetupIconFile=icon.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
UninstallDisplayName={#AppName}
UninstallDisplayIcon={app}\uninstall.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; VST3 Plugin
Source: "windows\plugins\VST3\The King's Cab.vst3\*"; DestDir: "{commoncf}\VST3\The King's Cab.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

; AAX Plugin (Pro Tools)
Source: "windows\plugins\AAX\The King's Cab.aaxplugin\*"; DestDir: "{commoncf}\Avid\Audio\Plug-Ins\The King's Cab.aaxplugin"; Flags: ignoreversion recursesubdirs createallsubdirs

; IR Collections
Source: "windows\ir_collections\*"; DestDir: "{commonappdata}\King Studios\The King's Cab\IR Collections"; Flags: ignoreversion recursesubdirs createallsubdirs

; Documentation
Source: "windows\README-WINDOWS.txt"; DestDir: "{app}"; DestName: "README.txt"

[Icons]
Name: "{group}\{#AppName} Documentation"; Filename: "{app}\README.txt"
Name: "{group}\Visit King Studios Store"; Filename: "{#AppURL}/store"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "SOFTWARE\King Studios\{#AppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"
Root: HKLM; Subkey: "SOFTWARE\King Studios\{#AppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#AppVersion}"
Root: HKLM; Subkey: "SOFTWARE\King Studios\{#AppName}"; ValueType: string; ValueName: "IRCollectionsPath"; ValueData: "{commonappdata}\King Studios\The King's Cab\IR Collections"

[Run]
Filename: "{app}\README.txt"; Description: "{cm:LaunchProgram,Documentation}"; Flags: postinstall nowait skipifsilent shellexec

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf}\VST3\The King's Cab.vst3"
Type: filesandordirs; Name: "{commoncf}\Avid\Audio\Plug-Ins\The King's Cab.aaxplugin"
Type: filesandordirs; Name: "{commonappdata}\King Studios\The King's Cab"

[Messages]
WelcomeLabel2=This will install [name/ver] on your computer.%n%nThe King's Cab is a professional impulse response loader plugin with premium cabinet simulation.%n%nIncludes:%n• VST3 plugin for all major DAWs%n• AAX plugin for Pro Tools%n• Complete IR collection with 500+ impulse responses%n• Vintage Neve/API style interface%n%nIt is recommended that you close all other applications before continuing.

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Kill any running DAWs to ensure plugin registration
    Exec('taskkill', '/f /im "Pro Tools.exe"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec('taskkill', '/f /im "reaper.exe"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec('taskkill', '/f /im "Cubase*.exe"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec('taskkill', '/f /im "Studio One.exe"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Exec('taskkill', '/f /im "Live.exe"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
end;

function InitializeSetup(): Boolean;
begin
  Result := True;
  
  // Check Windows version
  if not IsWin64 then
  begin
    MsgBox('This plugin requires a 64-bit version of Windows 10 or later.', mbError, MB_OK);
    Result := False;
  end;
end;
