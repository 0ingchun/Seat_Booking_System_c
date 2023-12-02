#ifndef LOGIN_USER__H
#define LOGIN_USER__H

typedef struct {
    char username[100];
    char passwd[100];
    short auth;    // »®œﬁ
    unsigned int balance;   // ’Àªß”‡∂Ó
} Login_User_t;

// extern Login_User_t Login_User;

// void Login_Test();

void viewUser(const char* jsonStr);

int findUser(const char* jsonStr, const char* username, char* outPasswd, short* outAuth, unsigned int* outBalance);

char* modifyUser(const char* jsonStr, const char* newUsername, const char* newPasswd, short newAuth, unsigned int newBalance);

char* addUser(const char* jsonStr, const char* username, const char* passwd, short auth);

char* deleteUser(const char* jsonStr, const char* username);

#endif