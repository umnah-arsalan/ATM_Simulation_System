#ifndef ATM_H
#define ATM_H

#define ACC_FILE "accounts.dat"
#define TRANS_FILE "transactions.dat"

struct Account {
    int acc_no;
    char name[30];
    int pin;
    float balance;
    int blocked;
};

struct Transaction {
    int acc_no;
    char type[10];
    float amount;
    char date_time[60];
};

int account_check(int account);
void create_account(void);
int authenticate(struct Account *acc);
void ATM_menu(struct Account acc);
void check_balance(struct Account acc);
void deposit(struct Account *acc);
void withdraw(struct Account *acc);
void show_transactions(int acc_no);

#endif