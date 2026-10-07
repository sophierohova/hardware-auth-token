#include <iostream>
#include <ctime>
#include <windows.h>
    
const DWORD PORT_BAUD_RATE = 115200;
const BYTE  TARGET_SUM = 0xFF;

HANDLE configureSerialPort(const char* portName) {
    HANDLE hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(hSerial, &dcb)) {
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcb.BaudRate = PORT_BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcb)) {
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = { 50, 10, 50, 10, 50 };
    SetCommTimeouts(hSerial, &timeouts);
    return hSerial;
}

int main() {
    setlocale(LC_ALL, "Russian");
    const char* portName = "\\\\.\\COM5";
    std::cout << "[SYSTEM] Подключение к аппаратному ключу" << std::endl;
    HANDLE hSerial = configureSerialPort(portName);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] Ошибка: Токен на COM5 не найден" << std::endl;
        return 1;
    }

    Sleep(2000);
    srand(time(NULL));
    BYTE challengeByte = rand() % 256;
    BYTE responseByte = 0;
    DWORD bytesWritten, bytesRead;
    std::cout << "[INFO] Challenge: 0x" << std::hex << (int)challengeByte << std::endl;

    if (!WriteFile(hSerial, &challengeByte, 1, &bytesWritten, NULL)) {
        CloseHandle(hSerial);
        return 1;
    }
    if (!ReadFile(hSerial, &responseByte, 1, &bytesRead, NULL) || bytesRead == 0) {
        std::cerr << "[ERROR] Тайм-аут ответа устройства\n";
        CloseHandle(hSerial);
        return 1;
    }

    std::cout << "[INFO] Response: 0x" << std::hex << (int)responseByte << std::endl;
    BYTE result = challengeByte + responseByte;
    std::cout << "[INFO] (X + ~X): 0x" << (int)result << std::endl;
    CloseHandle(hSerial);

    if (result == TARGET_SUM) {
        std::cout << "ДОСТУП РАЗРЕШЕН" << std::endl;
        std::cout << std::endl;
        std::cout << " Студент: Ерохова Софи Владимировна" << std::endl;
        std::cout << " Курс: 3  Группа: 601" << std::endl;
        std::cout << " Специальность: Кибербезопасность\n Специализация: Безопасность компьютерных технологий и систем" << std::endl;
    }
    else {
        std::cerr << "[ALERT] Ключ не прошел проверку подлинности" << std::endl;
        return 1;
    }
    return 0;
}
