#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <limits>
#include <locale> // ��� setlocale


using namespace std;

// ����� ��� �������� ���������
class Message {
public:
    string from;  // ��� �����������
    string to;    // ��� ���������� (��� "all" ��� ��������)
    string text;  // ����� ���������

    // ����������� ���������
    Message(const string& f, const string& t, const string& txt)
        : from(f), to(t), text(txt) {
    }
};

// ����� ������������
class User {
private:
    string login;     // ����� ������������
    string password;  // ������
    string nick;      // �������
    vector<Message> inbox;  // �������� ���������

public:
    // ����������� ������������
    User(const string& l, const string& p, const string& n)
        : login(l), password(p), nick(n) {
    }

    // �������� �����
    const string& getLogin() const { return login; }
    // �������� ���
    const string& getNick() const { return nick; }
    // �������� ������ (��� ��������� ������)
    const string& getPassword() const { return password; }

    // �������� ������
    bool checkPassword(const string& p) const {
        return password == p;
    }

    // �������� ��������� (�������� � "��������")
    void receiveMessage(const Message& msg) {
        inbox.push_back(msg);
    }

    // ��������, ���� �� ������������� ���������
    bool hasUnread() const {
        return !inbox.empty();
    }

    // �������� ��� �������� ��������� � �������� ��
    void showMessages() {
        if (inbox.empty()) {
            cout << "� ��� ��� ����� ���������.\n";
            return;
        }
        cout << "� ��� " << inbox.size() << " ����� ���������:\n";
        for (size_t i = 0; i < inbox.size(); ++i) {
            cout << i + 1 << ") ��: " << inbox[i].from << "\n";
            cout << "   ���������: " << inbox[i].text << "\n";
        }
        // �������� �������� ����� ������
        inbox.clear();
    }
};

// ����� ���� � �������� ������
class Chat {
private:
    // ������� �������������: ���� � �����, �������� � ������ User
    map<string, User> users;
    User* currentUser = nullptr;  // ������� �������� ������������

public:
    // ����������� � ������ 3 ������������ �� �������
    Chat() {
        registerUser("user1@mail.ru", "100501", "user1");
        registerUser("user2@mail.ru", "100502", "user2");
        registerUser("user3@mail.ru", "100503", "user3");
    }

    // ����������� ������������ (���������� � �������)
    void registerUser(const string& login, const string& password, const string& nick) {
        if (users.count(login) == 0) {
            users.insert(make_pair(login, User(login, password, nick)));
        }
        else {
            throw runtime_error("������������ � ����� ������� ��� ����������");
        }
    }

    // ���� ������������
    void loginUser() {
        cout << "������� �����: ";
        string login;
        cin >> login;

        // ���������, ���� �� ����� �����
        if (users.count(login) == 0) {
            throw runtime_error("������������ � ����� ������� �� ������");
        }

        cout << "������� ������: ";
        string password;
        cin >> password;

        // �������� ������ �� ������������ �� map
        User& user = users.at(login);

        if (!user.checkPassword(password)) {
            throw runtime_error("�������� ������");
        }

        currentUser = &user;
        cout << "����� ����������, " << currentUser->getNick() << "!\n";

        // ���� ���� ����� ���������, ���������� �� ���������
        if (currentUser->hasUnread()) {
            cout << "� ��� ���� ����� ���������. ������ �� ���������? (1 - ��, 0 - ���): ";
            int choice = getIntInput(0, 1);
            if (choice == 1) {
                currentUser->showMessages();
            }
        }
    }

    // ����� �� ��������
    void logout() {
        currentUser = nullptr;
    }

    // ��������� ��������� ����������� ������������ �� ����
    void sendMessageToUser() {
        if (!currentUser) {
            cout << "������� ������� � �������.\n";
            return;
        }
        cout << "������� ��� ����������: ";
        string nickTo;
        cin >> nickTo;

        // ���� ������������ �� ���� (���������� map)
        User* recipient = nullptr;
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            User& user = it->second;
            if (user.getNick() == nickTo) {
                recipient = &user;
                break;
            }
        }
        if (!recipient) {
            cout << "������������ � ����� ����� �� ������.\n";
            return;
        }

        cout << "������� ���������: ";
        // ������� ����� ����� ����� getline
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string text;
        getline(cin, text);

        // ������ ��������� � ���������� ����������
        Message msg(currentUser->getNick(), nickTo, text);
        recipient->receiveMessage(msg);

        cout << "��������� ���������� ������������ " << nickTo << ".\n";
    }

    // ��������� ��������� ���� �������������
    void sendMessageToAll() {
        if (!currentUser) {
            cout << "������� ������� � �������.\n";
            return;
        }
        cout << "������� ��������� ��� ����: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string text;
        getline(cin, text);

        // ���������� ��������� ����, ����� ����
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            User& user = it->second;
            if (user.getNick() != currentUser->getNick()) {
                user.receiveMessage(Message(currentUser->getNick(), user.getNick(), text));
            }
        }
        cout << "��������� ���������� ���� �������������.\n";
    }

    // �������� ����� � ����� ���� �������, ������� � �����
    void testMode() {
        cout << "\n=== �������� �����: ������ ���� ������������� ===\n";
        for (map<string, User>::iterator it = users.begin(); it != users.end(); ++it) {
            const User& user = it->second;
            cout << "�����: " << user.getLogin()
                << ", ������: " << user.getPassword()
                << ", ���: " << user.getNick() << "\n";
        }
        cout << "===============================================\n";
    }

    // �������� ������ ������������
    void addNewUser() {
        cout << "������� ����� ������ ������������: ";
        string login;
        cin >> login;

        if (users.count(login) > 0) {
            cout << "������������ � ����� ������� ��� ����������.\n";
            return;
        }

        cout << "������� ������: ";
        string password;
        cin >> password;

        cout << "������� ���: ";
        string nick;
        cin >> nick;

        try {
            registerUser(login, password, nick);
            cout << "������������ ������� ��������.\n";
        }
        catch (const exception& e) {
            cout << "������ ��� ���������� ������������: " << e.what() << "\n";
        }
    }

    // ��������, ����� �� ������������
    bool isUserLoggedIn() const {
        return currentUser != nullptr;
    }

    // �������� ���� ��������
    void showMenu() {
        cout << "\n����:\n";
        cout << "1. �����\n";
        cout << "2. ��������� ��������� ����������� ������������\n";
        cout << "3. ��������� ��������� ����\n";
        cout << "4. �����\n";
        cout << "5. �������� ����� ��������� (�������� ���� �������������)\n";
        cout << "6. �������� ������ ������������\n";
        cout << "0. ����� �� ���������\n";
        cout << "�������� ��������: ";
    }

    // ������� ��� ����������� ����� ������ ����� � ��������� ���������
    int getIntInput(int minVal, int maxVal) {
        int val;
        while (true) {
            if (!(cin >> val)) {
                cin.clear(); // ����� ������ �����
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ������� ������
                cout << "������ �����, ������� �����: ";
                continue;
            }
            if (val < minVal || val > maxVal) {
                cout << "������� ����� �� " << minVal << " �� " << maxVal << ": ";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ������� ������� ������
            return val;
        }
    }
};

int main() {
    setlocale(LC_ALL, "rus");  //  ��� ����������� ������/����� �������� �����

    Chat chat;

    while (true) {
        chat.showMenu();
        int choice;
        try {
            choice = chat.getIntInput(0, 6);
        }
        catch (const exception& e) {
            cout << "������ �����: " << e.what() << "\n";
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
                cout << "�� ����� �� ��������.\n";
                break;
            case 5:
                chat.testMode();
                break;
            case 6:
                chat.addNewUser();
                break;
            case 0:
                cout << "����� �� ���������.\n";
                return 0;
            default:
                cout << "�������� �����.\n";
            }
        }
        catch (const exception& ex) {
            cout << "������: " << ex.what() << "\n";
        }
    }
    return 0;
}
