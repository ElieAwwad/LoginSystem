// login_Sys.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mariadb/conncpp.hpp>
#include "bcrypt.h"
using namespace std;
int main()
{
    void signUp(sql::Connection * conn);
    void logIn(sql::Connection * conn);

    try
    {
        sql::Driver* driver = sql::mariadb::get_driver_instance();

        sql::SQLString url("jdbc:mariadb://localhost:3306/login_system");

        sql::Properties properties({
            {"user", "root"},
            {"password",std::getenv("DB_PASSWORD") }
            });

        std::unique_ptr<sql::Connection> conn(
            driver->connect(url, properties)
        );

        std::cout << "Connected successfully!" << std::endl;
            
        int choice = 0;
        while (true) {
            cout << "Please enter the corresponding number of your choice:\n";
            cout << "\n\"1\"-> Sign Up\n\"2\"-> Log In\n\"3\"-> Exit\nChoice:";
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "ERROR : Please input INTEGERS |1| |2| or |3|\n";
                continue;
            }
            else {
                cin.ignore(10000, '\n');
            }
            if(choice==1) {
                signUp(conn.get());
            }
            else if (choice == 2)
            {
                logIn(conn.get());
            }
            else if (choice == 3)
            {
                break;
            }
            else {
                cout << "Error: Please choose |1| |2| or |3|\n";
            }
        }
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
void signUp(sql::Connection* conn) {
    string username, password;
    cout << "Choose a username: ";
    cin >> username;
    cout << "Choose a password: ";
    cin >> password;

    std::string hashed = bcrypt::generateHash(password, 12);

    unique_ptr<sql::PreparedStatement> pstmt(
        conn->prepareStatement("INSERT INTO users (username, password_hash) VALUES (?, ?)"));
    pstmt->setString(1, username);
    pstmt->setString(2, hashed); // we'll hash this properly later
    pstmt->executeUpdate();

    std::cout << "Account created!\n";
}
void logIn(sql::Connection* conn)
{
    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::unique_ptr<sql::PreparedStatement> pstmt(
        conn->prepareStatement("SELECT password_hash FROM users WHERE username = ? ")
    );
    pstmt->setString(1, username);
    

    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    if (res->next())
    {
        std::string storedHash = res->getString("password_hash").c_str();
        bool valid = bcrypt::validatePassword(password, storedHash);
        if(valid){
        std::cout << "Login successful! Welcome, " << username << ".\n";
    }
    else
    {
        std::cout << "Invalid username or password.\n";
    }
}
    else
    {
        std::cout << "Invalid username or password.\n"; 
    }
}