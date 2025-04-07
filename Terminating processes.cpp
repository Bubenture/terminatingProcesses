#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <locale>
#include <codecvt>

// Структура для хранения информации о процессе
struct Process {
    std::string name;        // Название процесса
    DWORD pid;               // Идентификатор процесса
    int duration_seconds;    // Время до завершения процесса в секундах
    bool is_closed;          // Флаг, указывающий на закрытие процесса

    // Конструктор, принимающий std::string
    Process(const std::string& processName)
        : name(processName), pid(0), duration_seconds(0), is_closed(false) {}
};

// Мьютекс для обеспечения потокобезопасности
std::mutex processMutex;

// Функция для получения списка всех процессов
std::vector<PROCESSENTRY32> getProcesses() {
    std::vector<PROCESSENTRY32> processes;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось создать снимок процессов." << std::endl;
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            processes.push_back(pe32);
        } while (Process32Next(hSnapshot, &pe32));
    }
    else {
        std::cerr << "Не удалось получить информацию о первом процессе." << std::endl;
    }

    CloseHandle(hSnapshot);
    return processes;
}

// Функция для преобразования std::wstring в std::string
std::string wstringToString(const std::wstring& wstr) {
    // Используем std::wstring_convert для преобразования
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// Функция для завершения процесса
void closeProcess(Process& process) {
    {
        std::lock_guard<std::mutex> lock(processMutex);
        std::cout << "Запуск таймера для процесса \"" << process.name
            << "\" (PID: " << process.pid << ") на "
            << process.duration_seconds << " секунд." << std::endl;
    }

    // Ждём заданное количество времени
    std::this_thread::sleep_for(std::chrono::seconds(process.duration_seconds));

    {
        std::lock_guard<std::mutex> lock(processMutex);

        // Завершение процесса по его PID
        std::string command = "taskkill /PID " + std::to_string(process.pid) + " /F";
        int result = std::system(command.c_str());

        if (result == 0) {
            process.is_closed = true;
            std::cout << "Процесс \"" << process.name << "\" (PID: " << process.pid << ") завершен." << std::endl;
        }
        else {
            std::cerr << "Не удалось завершить процесс \"" << process.name << "\" (PID: " << process.pid << ")." << std::endl;
        }
    }
}

int main() {
    std::vector<Process> processes;
    std::string input;
    int duration;
    setlocale(LC_ALL, "Russian");

    // Получаем список всех процессов
    auto allProcesses = getProcesses();

    // Выводим список процессов
    std::cout << "Список всех активных процессов:" << std::endl;
    for (const auto& pe : allProcesses) {
        std::wstring ws(pe.szExeFile);
        std::string name = wstringToString(ws);
        std::cout << "PID: " << pe.th32ProcessID << " | Name: " << name << std::endl;
    }

    std::cout << "Введите PID процесса, который хотите завершить (пустая строка для завершения выбора):" << std::endl;

    // Выбор процессов пользователем
    while (true) {
        std::cout << "Введите PID процесса: ";
        std::getline(std::cin, input);

        if (input.empty()) {
            break;
        }

        DWORD pid = std::stoi(input);
        auto it = std::find_if(allProcesses.begin(), allProcesses.end(), [pid](const PROCESSENTRY32& pe) {
            return pe.th32ProcessID == pid;
            });

        if (it == allProcesses.end()) {
            std::cerr << "Процесс с PID " << pid << " не найден." << std::endl;
            continue;
        }

        // Создание строки из массива WCHAR
        std::wstring ws(it->szExeFile);
        std::string name = wstringToString(ws);

        Process process(name);
        process.pid = pid;

        std::cout << "Введите продолжительность до завершения (в секундах) для процесса \""
            << process.name << "\" (PID: " << process.pid << "): ";
        std::cin >> duration;
        std::cin.ignore(); // Чтобы избежать проблем с getline после ввода int

        process.duration_seconds = duration;
        processes.push_back(process);
    }

    // Запуск таймеров для выбранных процессов
    std::vector<std::thread> threads;
    for (Process& process : processes) {
        threads.emplace_back(closeProcess, std::ref(process));
    }

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Все выбранные процессы завершены." << std::endl;
    return 0;
}
