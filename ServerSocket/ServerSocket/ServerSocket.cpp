#define WIN32_LEAN_AND_MEAN // Определение для исключения редко используемых компонентов Windows из заголовков

#include <Windows.h>  // Основной заголовочный файл для использования Windows API
#include <iostream>   // Заголовочный файл для стандартного ввода-вывода
#include <WinSock2.h> // Заголовочный файл для использования WinSock 2 API
#include <WS2tcpip.h> // Дополнительные функции для работы с TCP/IP, такие как getaddrinfo

using namespace std; // Пространство имен для стандартной библиотеки C++

int main() {
    WSADATA wsaData; // Структура для хранения информации о версии WinSock
    ADDRINFO hints;  // Структура для хранения информации о сокете
    ADDRINFO* addrResult;  // Указатель на результат функции getaddrinfo
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания входящих соединений
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к клиенту
    char recvBuffer[512]; // Буфер для хранения данных, полученных от клиента

    const char* sendBuffer = "Hello from server \n"; // Сообщение для отправки клиенту

    // Инициализация WinSock, использование версии 2.2
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) { // Проверка на ошибки
        cout << "WSAStartup failed with result: " << result << endl; // Вывод сообщения об ошибке
        return 1; // Завершение программы с кодом ошибки
    }

    // Заполнение структуры hints для использования с getaddrinfo
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Указание на использование IPv4
    hints.ai_socktype = SOCK_STREAM; // Указание на использование потокового сокета
    hints.ai_protocol = IPPROTO_TCP; // Указание на использование протокола TCP
    hints.ai_flags = AI_PASSIVE; // Указание на использование пассивного сокета (для прослушивания)

    // Получение информации о сокете
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) { // Проверка на ошибки
        cout << "getaddrinfo failed with error: " << result << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Создание сокета для прослушивания входящих соединений
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) { // Проверка на ошибки
        cout << "Socket creation failed" << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Привязка сокета к адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Bind failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Настройка сокета на прослушивание входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Listen failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    // Принятие входящего соединения
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибки
        cout << "Accept failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    closesocket(ListenSocket); // Закрытие сокета прослушивания, так как соединение установлено

    // Обработка сообщений от клиента
    do {
        ZeroMemory(recvBuffer, 512); // Очистка буфера
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных от клиента
        if (result > 0) { // Проверка на успешное получение данных
            cout << "Received " << result << " bytes" << endl; // Вывод количества полученных байтов
            cout << "Received data: " << recvBuffer << endl; // Вывод полученных данных

            // Отправка данных клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) { // Проверка на ошибки
                cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
                closesocket(ConnectSocket); // Закрытие сокета
                freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
                WSACleanup(); // Завершение использования WinSock
                return 1; // Завершение программы с кодом ошибки
            }
        }
        else if (result == 0) { // Проверка на закрытие соединения
            cout << "Connection closing" << endl; // Вывод сообщения о закрытии соединения
        }
        else { // Проверка на ошибки
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
            closesocket(ConnectSocket); // Закрытие сокета
            freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
            WSACleanup(); // Завершение использования WinSock
            return 1; // Завершение программы с кодом ошибки
        }
    } while (result > 0); // Цикл продолжается, пока принимаются данные

    // Завершение соединения
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) { // Проверка на ошибки
        cout << "Shutdown failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
        WSACleanup(); // Завершение использования WinSock
        return 1; // Завершение программы с кодом ошибки
    }

    closesocket(ConnectSocket); // Закрытие соединения
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
    WSACleanup(); // Освобождение ресурсов, выделенных WinSock
    return 0; // Завершение программы с кодом 0 (успешно)
}
