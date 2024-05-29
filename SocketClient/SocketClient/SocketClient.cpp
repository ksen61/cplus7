#define WIN32_LEAN_AND_MEAN // Определение для исключения редко используемых компонентов Windows

#include <Windows.h> // Заголовочный файл для Windows API
#include <iostream>  // Заголовочный файл для стандартного ввода-вывода
#include <WinSock2.h> // Заголовочный файл для WinSock API
#include <WS2tcpip.h> // Заголовочный файл для дополнительных функций WinSock, таких как getaddrinfo

using namespace std; // Пространство имен для стандартной библиотеки C++

int main() {
    WSADATA wsaData; // Структура для хранения информации о версии WinSock
    ADDRINFO hints;  // Структура для хранения информации о сокете
    ADDRINFO* addrResult; // Указатель на результат функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к серверу
    char recvBuffer[512]; // Буфер для хранения данных, полученных от сервера

    const char* sendBuffer1 = "Hello from client 1"; // Первое сообщение для отправки серверу
    const char* sendBuffer2 = "Hello from client 2"; // Второе сообщение для отправки серверу

    // Инициализация WinSock, использование версии 2.2
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) { // Проверка на ошибки
        cout << "WSAStartup failed with result: " << result << endl; // Вывод сообщения об ошибке
        return 1; // Завершение программы с кодом ошибки
    }

    // Обнуление структуры hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Указание на использование IPv4
    hints.ai_socktype = SOCK_STREAM; // Указание на использование потокового сокета
    hints.ai_protocol = IPPROTO_TCP; // Указание на использование протокола TCP

    // Получение информации о сокете на основе введенных данных
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) { // Проверка на ошибки
        cout << "getaddrinfo failed with error: " << result << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Создание сокета на основе информации, полученной из getaddrinfo
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибки
        cout << "Socket creation failed" << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Подключение к серверу с использованием полученного адреса
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Unable to connect to server" << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        ConnectSocket = INVALID_SOCKET; // Указание на недействительный сокет
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }
    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байтов

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }
    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байтов

    // Завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Shutdown failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Получение данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // Очистка буфера
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных от сервера
        if (result > 0) { // Проверка на успешное получение данных
            cout << "Received " << result << " bytes" << endl; // Вывод количества полученных байтов
            cout << "Received data: " << recvBuffer << endl; // Вывод полученных данных
        }
        else if (result == 0) { // Проверка на закрытие соединения
            cout << "Connection closed" << endl; // Вывод сообщения о закрытии соединения
        }
        else { // Проверка на ошибки
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
        }
    } while (result > 0); // Цикл продолжается, пока принимаются данные

    closesocket(ConnectSocket); // Закрытие сокета
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
    WSACleanup(); // Завершение использования WinSock
    return 0; // Завершение программы с кодом 0 (успешно)
}
