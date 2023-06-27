## Loader

```bash
@set ProcessName="*.exe"
@set DllPath32="%~dp0Test32.dll"
@set DllPath64="%~dp0Test64.dll"

@sc stop Bridge
@sc delete Bridge
@reg add HKLM\SYSTEM\CurrentControlSet\Services\Bridge /f /t REG_SZ /v ProcessName /d %ProcessName%
@reg add HKLM\SYSTEM\CurrentControlSet\Services\Bridge /f /t REG_SZ /v DllPath32 /d %DllPath32%
@reg add HKLM\SYSTEM\CurrentControlSet\Services\Bridge /f /t REG_SZ /v DllPath64 /d %DllPath64%
@sc create Bridge type= kernel binpath= "%~dp0Bridge.sys"
@sc start Bridge
@pause
```

## Unloader

```bash
@sc stop Bridge
@sc delete Bridge
@pause
```

