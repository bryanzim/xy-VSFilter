; xy-VSFilter 4.0.0.0 setup.
; Compile with Inno Setup 7:
;   ISCC.exe installer\xy-vsfilter.iss

#if Ver < EncodeVer(7, 0, 0)
  #error This script requires Inno Setup 7 or later.
#endif

#define MyAppName "xy-VSFilter"
#define MyAppVersion "4.0.0.0"
#define MyAppPublisher "xy-VSFilter"
#define MyAppURL "https://github.com/pinterf/xy-VSFilter/"
#define BuildWin32 "..\build\clang-win32\bin\Release"
#define BuildX64 "..\build\clang-x64\bin\Release"

[Setup]
AppId={{7E4B1C2A-5D83-4F19-A6C0-8B2E91D44F70}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=..\License.txt
InfoAfterFile=AfterInstall.txt
OutputDir=..\dist
OutputBaseFilename=xy-VSFilter-{#MyAppVersion}-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
ArchitecturesAllowed=x86compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=6.1sp1
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
VersionInfoVersion={#MyAppVersion}
VersionInfoProductVersion={#MyAppVersion}
VersionInfoDescription={#MyAppName} DirectShow subtitle filters
VersionInfoProductName={#MyAppName}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "XySubFilter, DirectVobSub, and the subtitle consumer"
Name: "xysubfilter"; Description: "XySubFilter only"
Name: "custom"; Description: "Choose which filters to register"; Flags: iscustom

[Components]
Name: "xysubfilter"; Description: "XySubFilter and XySubFilter AutoLoader"; Types: full xysubfilter custom; Flags: fixed
Name: "vsfilter"; Description: "DirectVobSub (VSFilter), including its auto-loading version"; Types: full custom
Name: "consumer"; Description: "XySubFilter consumer, for a player that renders the subtitle frames itself"; Types: full custom

[Files]
Source: "{#BuildX64}\XySubFilter.dll"; DestDir: "{app}\x64"; Components: xysubfilter; Flags: ignoreversion restartreplace uninsrestartdelete; Check: Is64BitInstallMode
Source: "{#BuildX64}\VSFilter.dll"; DestDir: "{app}\x64"; Components: vsfilter; Flags: ignoreversion restartreplace uninsrestartdelete; Check: Is64BitInstallMode
Source: "{#BuildX64}\xy_sub_filter_consumer.dll"; DestDir: "{app}\x64"; Components: consumer; Flags: ignoreversion restartreplace uninsrestartdelete; Check: Is64BitInstallMode
Source: "{#BuildWin32}\XySubFilter.dll"; DestDir: "{app}\x86"; Components: xysubfilter; Flags: ignoreversion restartreplace uninsrestartdelete
Source: "{#BuildWin32}\VSFilter.dll"; DestDir: "{app}\x86"; Components: vsfilter; Flags: ignoreversion restartreplace uninsrestartdelete
Source: "{#BuildWin32}\xy_sub_filter_consumer.dll"; DestDir: "{app}\x86"; Components: consumer; Flags: ignoreversion restartreplace uninsrestartdelete
Source: "..\License.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\XySubFilter settings"; Filename: "{sys}\rundll32.exe"; Parameters: """{app}\x64\XySubFilter.dll"",XySubFilterConfiguration"; Components: xysubfilter; Check: Is64BitInstallMode; Comment: "Open the XySubFilter property pages"
Name: "{group}\XySubFilter settings"; Filename: "{sys}\rundll32.exe"; Parameters: """{app}\x86\XySubFilter.dll"",XySubFilterConfiguration"; Components: xysubfilter; Check: not Is64BitInstallMode; Comment: "Open the XySubFilter property pages"
Name: "{group}\DirectVobSub settings"; Filename: "{sys}\rundll32.exe"; Parameters: """{app}\x64\VSFilter.dll"",DirectVobSub"; Components: vsfilter; Check: Is64BitInstallMode; Comment: "Open the DirectVobSub property pages"
Name: "{group}\DirectVobSub settings"; Filename: "{sys}\rundll32.exe"; Parameters: """{app}\x86\VSFilter.dll"",DirectVobSub"; Components: vsfilter; Check: not Is64BitInstallMode; Comment: "Open the DirectVobSub property pages"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"

[Code]
function RegSvr32(const DllPath, RegSvr: String; const Unregister: Boolean): Boolean;
var
  Parameters: String;
  ResultCode: Integer;
begin
  Result := True;
  if not FileExists(DllPath) then
    Exit;
  if Unregister then
    Parameters := '/s /u "' + DllPath + '"'
  else
    Parameters := '/s "' + DllPath + '"';
  if not Exec(RegSvr, Parameters, '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
  begin
    MsgBox('Could not start regsvr32 for:' + #13#10 + DllPath, mbError, MB_OK);
    Result := False;
    Exit;
  end;
  if ResultCode <> 0 then
  begin
    if Unregister then
      MsgBox('Could not unregister:' + #13#10 + DllPath + #13#10 +
        'Close any player using the filter, then run the uninstaller again.', mbError, MB_OK)
    else
      MsgBox('Could not register:' + #13#10 + DllPath + #13#10 +
        'regsvr32 returned ' + IntToStr(ResultCode) + '.', mbError, MB_OK);
    Result := False;
  end;
end;

procedure RegisterFiltersIn(const AppDir: String; const Unregister: Boolean);
var
  NativeReg: String;
  WowReg: String;
begin
  { Only files for the selected components are present, so registering
    every DLL that exists matches the install, and unregistering every
    DLL that exists matches the uninstall. }
  NativeReg := ExpandConstant('{sys}\regsvr32.exe');
  WowReg := ExpandConstant('{syswow64}\regsvr32.exe');
  if Is64BitInstallMode then
  begin
    RegSvr32(AppDir + '\x64\XySubFilter.dll', NativeReg, Unregister);
    RegSvr32(AppDir + '\x64\VSFilter.dll', NativeReg, Unregister);
    RegSvr32(AppDir + '\x64\xy_sub_filter_consumer.dll', NativeReg, Unregister);
    RegSvr32(AppDir + '\x86\XySubFilter.dll', WowReg, Unregister);
    RegSvr32(AppDir + '\x86\VSFilter.dll', WowReg, Unregister);
    RegSvr32(AppDir + '\x86\xy_sub_filter_consumer.dll', WowReg, Unregister);
  end
  else
  begin
    RegSvr32(AppDir + '\x86\XySubFilter.dll', NativeReg, Unregister);
    RegSvr32(AppDir + '\x86\VSFilter.dll', NativeReg, Unregister);
    RegSvr32(AppDir + '\x86\xy_sub_filter_consumer.dll', NativeReg, Unregister);
  end;
end;

procedure RegisterInstalledFilters(const Unregister: Boolean);
begin
  RegisterFiltersIn(ExpandConstant('{app}'), Unregister);
end;

function PreviousInstallPath(var InstallPath: String): Boolean;
var
  Key: String;
begin
  Key := 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppId")}_is1';
  Result := RegQueryStringValue(HKLM, Key, 'Inno Setup: App Path', InstallPath);
  if not Result then
    Result := RegQueryStringValue(HKCU, Key, 'Inno Setup: App Path', InstallPath);
  if not Result and IsWin64 then
    Result := RegQueryStringValue(HKLM32, Key, 'Inno Setup: App Path', InstallPath);
end;

procedure RemoveFilterFiles(const AppDir: String);
begin
  DeleteFile(AppDir + '\x64\XySubFilter.dll');
  DeleteFile(AppDir + '\x64\VSFilter.dll');
  DeleteFile(AppDir + '\x64\xy_sub_filter_consumer.dll');
  DeleteFile(AppDir + '\x86\XySubFilter.dll');
  DeleteFile(AppDir + '\x86\VSFilter.dll');
  DeleteFile(AppDir + '\x86\xy_sub_filter_consumer.dll');
  RemoveDir(AppDir + '\x64');
  RemoveDir(AppDir + '\x86');
end;

procedure RemovePreviousInstall;
var
  OldDir: String;
  NewDir: String;
begin
  if not PreviousInstallPath(OldDir) then
    Exit;
  OldDir := RemoveBackslash(OldDir);
  if not DirExists(OldDir) then
    Exit;
  NewDir := RemoveBackslash(ExpandConstant('{app}'));
  RegisterFiltersIn(OldDir, True);
  RemoveFilterFiles(OldDir);
  { A different previous folder has been unregistered. Remove the files this
    setup owns there, then the folder itself when nothing else remains. }
  if CompareText(OldDir, NewDir) <> 0 then
  begin
    DeleteFile(OldDir + '\License.txt');
    DeleteFile(OldDir + '\README.md');
    DeleteFile(OldDir + '\unins000.exe');
    DeleteFile(OldDir + '\unins000.dat');
    DeleteFile(OldDir + '\unins000.msg');
    RemoveDir(OldDir);
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
    RemovePreviousInstall
  else if CurStep = ssPostInstall then
    RegisterInstalledFilters(False);
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  { usUninstall runs while the DLLs are still on disk. }
  if CurUninstallStep = usUninstall then
    RegisterInstalledFilters(True);
end;
