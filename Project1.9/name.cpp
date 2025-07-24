//#ifdef _WIN32
//#pragma execution_character_set("utf-8")
//#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <limits>
#include <locale> // для setlocale


using namespace std;

// Класс для хранения сообщения
class Message {
public:
    string from;  // Ник отправителя
    string to;    // Ник получателя (или "all" для рассылки)
    string text;  // Текст сообщения

    // Конструктор сообщения
    Message(const string& f, const string& t, const string& txt)
        : from(f), to(t), text(txt) {
    }
};

// Класс пользователя
class User {
private:
    string login;     // Логин пользователя
    string password;  // Пароль
    string nick;      // Никнейм
    vector<Message> inbox;  // Входящие сообщения

public:
    // Конструктор пользователя
    User(const string& l, const string& p, const string& n)
        : login(l), password(p), nick(n) {
    }

    // Получить логин
    const string& getLogin() const { return login; }
    // Получить ник
    const string& getNick() const { return nick; }
    // Получить пароль (для тестового режима)
    const string& getPassword() const { return password; }

    // Проверка пароля
    bool checkPassword(const string& p) const {
        return password == p;
    }

    // Получить сообщение (положить в "входящие")
    void receiveMessage(const Message& msg) {
        inbox.push_back(msg);
    }

    // Проверка, есть ли непрочитанные сообщения
    bool hasUnread() const {
        return !inbox.empty();
    }

    // Показать все входящие сообщения и очистить их
    void showMessages() {
        if (inbox.empty()) {
            cout << "У вас нет новых сообщений.\n";
            return;
        }
        cout << "У вас " << inbox.size() << " новых сообщений:\n";
        for (size_t i = 0; i < inbox.size(); ++i) {
            cout << i + 1 << ") От: " << inbox[i].from << "\n";
            cout << "   Сообщение: " << inbox[i].text << "\n";
        }
        // Очистить входящие после показа
        inbox.clear();
    }
};

// Класс чата — основная логика
class Chat {
private:
    // Словарь пользователей: ключ — логин, значение — объект User
    map<string, User> users;
    User* currentUser = nullptr;  // Текущий вошедший пользователь

public:
    // Конструктор — создаём 3 пользователя по условию
    Chat() {
        registerUser("user1@mail.ru", "100501", "user1");
        registerUser("user2@mail.ru", "100502", "user2");
        registerUser("user3@mail.ru", "100503", "user3");
    }

    // Регистрация пользователя (добавление в словарь)
    void registerUser(const string& login, const string& password, const string& nick) {
        if (users.count(login) == 0) {
            users.insert(make_pair(login, User(login, password, nick)));
        }
        else {
            throw runtime_error("Пользователь с таким логином уже существует");
        }
    }

    // Вход пользователя
    void loginUser() {
        cout << "Введите логин: ";
        string login;
        cin >> login;

        // Проверяем, есть ли такой логин
        if (users.count(login) == 0) {
            throw runtime_error("Пользователь с таким логином не найден");
        }

        cout << "Введите пароль: ";
        string password;
        cin >> password;

        // Получаем ссылку на пользователя из map
        User& user = users.at(login);

        if (!user.checkPassword(password)) {
            throw runtime_error("Неверный пароль");
        }

        currentUser = &user;
        cout << "Добро пожаловать, " << currentUser->getNick() << "!\n";

        // Если есть новые сообщения, предлагаем их прочитать
        if (currentUser->hasUnread()) {
            cout << "У вас есть новые сообщения. Хотите их прочитать? (1 - да, 0 - нет): ";
            int choice = getIntInput(0, 1);
            if (choice == 1) {
                currentUser->showMessages();
            }
        }
    }

    // Выход из аккаунта
    void logout() {
        currentUser = nullptr;
    }

    // Отправить сообщение конкретному пользователю по нику
    void sendMessageToUser() {
        if (!currentUser) {
            cout << "Сначала войдите в систему.\n";
            return;
        }
        cout << "Введите ник получателя: ";
        string nickTo;
        cin >> nickTo;

        // Ищем пользователя по нику (перебираем map)
        User* recipient = nullptr;
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            User& user = it->second;
            if (user.getNick() == nickTo) {
                recipient = &user;
                break;
            }
        }
        if (!recipient) {
            cout << "Пользователь с таким ником не найден.\n";
            return;
        }

        cout << "Введите сообщение: ";
        // Очищаем буфер ввода перед getline
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string text;
        getline(cin, text);

        // Создаём сообщение и отправляем получателю
        Message msg(currentUser->getNick(), nickTo, text);
        recipient->receiveMessage(msg);

        cout << "Сообщение отправлено пользователю " << nickTo << ".\n";
    }

    // Отправить сообщение всем пользователям
    void sendMessageToAll() {
        if (!currentUser) {
            cout << "Сначала войдите в систему.\n";
            return;
        }
        cout << "Введите сообщение для всех: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string text;
        getline(cin, text);

        // Отправляем сообщение всем, кроме себя
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            User& user = it->second;
            if (user.getNick() != currentUser->getNick()) {
                user.receiveMessage(Message(currentUser->getNick(), user.getNick(), text));
            }
        }
        cout << "Сообщение отправлено всем пользователям.\n";
    }

    // Тестовый режим — вывод всех логинов, паролей и ников
    void testMode() {
        cout << "\n=== Тестовый режим: список всех пользователей ===\n";
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            const User& user = it->second;
            cout << "Логин: " << user.getLogin()
                << ", Пароль: " << user.getPassword()
                << ", Ник: " << user.getNick() << "\n";
        }
        cout << "===============================================\n";
    }

    // Добавить нового пользователя
    void addNewUser() {
        cout << "Введите логин нового пользователя: ";
        string login;
        cin >> login;

        if (users.count(login) > 0) {
            cout << "Пользователь с таким логином уже существует.\n";
            return;
        }

        cout << "Введите пароль: ";
        string password;
        cin >> password;

        cout << "Введите ник: ";
        string nick;
        cin >> nick;

        try {
            registerUser(login, password, nick);
            cout << "Пользователь успешно добавлен.\n";
        }
        catch (const exception& e) {
            cout << "Ошибка при добавлении пользователя: " << e.what() << "\n";
        }
    }

    // Проверка, вошёл ли пользователь
    bool isUserLoggedIn() const {
        return currentUser != nullptr;
    }

    // Показать меню действий
    void showMenu() {
        cout << "\nМеню:\n";
        cout << "1. Войти\n";
        cout << "2. Отправить сообщение конкретному пользователю\n";
        cout << "3. Отправить сообщение всем\n";
        cout << "4. Выйти\n";
        cout << "5. Тестовый режим подсказка (показать всех пользователей)\n";
        cout << "6. Добавить нового пользователя\n";
        cout << "0. Выход из программы\n";
        cout << "Выберите действие: ";
    }

    // Функция для безопасного ввода целого числа с проверкой диапазона
    int getIntInput(int minVal, int maxVal) {
        int val;
        while (true) {
            if (!(cin >> val)) {
                cin.clear(); // сброс ошибки ввода
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // очистка буфера
                cout << "Ошибка ввода, введите число: ";
                continue;
            }
            if (val < minVal || val > maxVal) {
                cout << "Введите число от " << minVal << " до " << maxVal << ": ";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // очистка остатка строки
            return val;
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian_Russia.65001"); // для Windows с UTF-8
    setlocale(LC_ALL, "rus");  //  для корректного вывода/ввода русского языка на гитхабе краказабли

    Chat chat;

    while (true) {
        chat.showMenu();
        int choice;
        try {
            choice = chat.getIntInput(0, 6);
        }
        catch (const exception& e) {
            cout << "Ошибка ввода: " << e.what() << "\n";
            continue;
        }

        try {
            switch (choice) {
            case 1:
                chat.loginUser();
                break;
            case 2:
                chat.sendMessageToUser();
                break;
            case 3:
                chat.sendMessageToAll();
                break;
            case 4:
                chat.logout();
                cout << "Вы вышли из аккаунта.\n";
                break;
            case 5:
                chat.testMode();
                break;
            case 6:
                chat.addNewUser();
                break;
            case 0:
                cout << "Выход из программы.\n";
                return 0;
            default:
                cout << "Неверный выбор.\n";
            }
        }
        catch (const exception& ex) {
            cout << "Ошибка: " << ex.what() << "\n";
        }
    }
    return 0;
}
