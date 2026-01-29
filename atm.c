#include<stdio.h>
#include<string.h>
#include<time.h>
#include "atm.h"

int account_check(int account) {
    struct Account a;
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
        return 0;

    while (fread(&a, sizeof(struct Account), 1, fp)) {
        if (account == a.acc_no) {
            printf("\nAccount already exists!\n\n");
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void create_account() {
    struct Account a;
    int account;
    float balance;

    FILE *fp = fopen(ACC_FILE, "ab");
    if (!fp) {
        printf("\nError opening file.\n\n");
        return;
    }

    printf("\nEnter account number: ");
    scanf("%d", &account);

    while (account_check(account) != 0) {
        printf("Enter account number: ");
        scanf("%d", &account);
    }

    a.acc_no = account;

    printf("Enter account holder's name: ");
    scanf(" %[^\n]", a.name);

    printf("Set 4-digit PIN: ");
    scanf(" %d", &a.pin);

    a.balance = 0.0;
    a.blocked = 0;

    fwrite(&a, sizeof(struct Account), 1, fp);

    printf("\nAccount created successfully!\n\n");

    fclose(fp);
}

int authenticate(struct Account *acc) {
    FILE *fp = fopen(ACC_FILE, "rb+");
    int acc_no, pin, attempts = 0;

    if (!fp) {
        printf("File error!");
        fclose(fp);
        return 0;
    }

    printf("\nEnter account number: ");
    scanf("%d", &acc_no);

    while (fread(acc, sizeof(struct Account), 1, fp)) {
        if (acc_no == acc->acc_no) {

            if (acc->blocked) {
                printf("\nAccount is BLOCKED!\n\n");
                fclose(fp);
                return 0;
            }

            while (attempts < 3) {

                printf("Enter 4-digit pin: ");
                scanf("%d", &pin);

                if (pin == acc->pin) {
                    fclose(fp);
                    return 1;
                }

                attempts++;
                printf("\nIncorrect pin! Attempts left (%d/3).\n\n", attempts);
            }

            acc->blocked = 1;
            fseek(fp, -sizeof(struct Account), SEEK_CUR);
            fwrite(&acc, sizeof(struct Account), 1, fp);

            printf("\nAccount blocked due to multiple failures!\n\n");
            fclose(fp);
            return 0;
        }
    }

    printf("\nAccount not found!\n\n");
    fclose(fp);
    return 0;
}

void ATM_menu(struct Account acc) {
    int choice;

    do {
        printf("1. Balance");
        printf("\n2. Deposit");
        printf("\n3. Withdraw");
        printf("\n4. Transactions");
        printf("\n5. Exit\n\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: check_balance(acc); break;
            case 2: deposit(&acc); break;
            case 3: withdraw(&acc); break;
            case 4: show_transactions(acc.acc_no); break;
        }

    } while (choice != 5);
    printf("\n");
}

void check_balance(struct Account acc) {
    printf("\nCurrent Balance: %.2f\n\n", acc.balance);
}

void deposit(struct Account *acc) {
    FILE *fp = fopen(ACC_FILE, "rb+");
    FILE *fpt = fopen(TRANS_FILE, "ab");
    struct Account temp;
    struct Transaction t;
    float amount;

    if (fp == NULL) {
        printf("\nFile not found.\n\n");
        return;
    }

    printf("\nEnter deposit amount: ");
    scanf("%f", &amount);

    while (fread(&temp, sizeof(struct Account), 1, fp)) {
        if (temp.acc_no == acc->acc_no) {
            if (amount > 0) {
                temp.balance += amount;
                acc->balance = temp.balance;

                fseek(fp, -sizeof(struct Account), SEEK_CUR);
                fwrite(&temp, sizeof(struct Account), 1, fp);

                time_t now = time(NULL);
                struct tm *tm = localtime(&now);

                sprintf(t.date_time, "%02d-%02d-%04d %02d:%02d",
                    tm->tm_mday, tm->tm_mon+1,
                    tm->tm_year+1900,
                    tm->tm_hour, tm->tm_min);

                t.acc_no = acc->acc_no;
                strcpy(t.type, "DEPOSIT");
                t.amount = amount;

                fwrite(&t, sizeof(struct Transaction), 1, fpt);

                break;
            } else {
                printf("\nInvalid amount!\n\n");
                fclose(fp);
                fclose(fpt);
                return;
            }
        }
    }

    fclose(fp);
    fclose(fpt);

    printf("\nDeposit successful. New balance: %.2f\n\n", acc->balance);
}

void withdraw(struct Account *acc) {
    FILE *fp = fopen(ACC_FILE, "rb+");
    FILE *fpt = fopen(TRANS_FILE, "ab");
    struct Account temp;
    struct Transaction t;
    float amount;

    if (fp == NULL) {
        printf("\nFile not found.\n\n");
        return;
    }

    printf("\nEnter withdraw amount: ");
    scanf("%f", &amount);

    while (fread(&temp, sizeof(struct Account), 1, fp)) {
        if (temp.acc_no == acc->acc_no) {
            if (temp.balance > amount) {

                if (amount < 0) {
                    printf("\nInvalid amount!\n\n");
                    fclose(fp);
                    fclose(fpt);
                    return;
                }
                temp.balance -= amount;
                acc->balance = temp.balance;

                fseek(fp, -sizeof(struct Account), SEEK_CUR);
                fwrite(&temp, sizeof(struct Account), 1, fp);

                time_t now = time(NULL);
                struct tm *tm = localtime(&now);

                sprintf(t.date_time, "%02d-%02d-%04d %02d:%02d",
                        tm->tm_mday, tm->tm_mon+1,
                        tm->tm_year+1900,
                        tm->tm_hour, tm->tm_min);

                t.acc_no = acc->acc_no;
                strcpy(t.type, "WITHDRAW");
                t.amount = amount;

                fwrite(&t, sizeof(struct Transaction), 1, fpt);

                break;
            } else {
                printf("\nInsufficient balance!\n\n");
                fclose(fp);
                fclose(fpt);
                return;
            }
        }
    }

    fclose(fp);
    fclose(fpt);

    printf("\nWithdrawal successful. New balance: %.2f\n\n", acc->balance);
}

void show_transactions(int acc_no) {
    FILE *fp = fopen(TRANS_FILE, "rb");
    struct Transaction t;

    printf("\n%-10s %-10s %-10s %-20s\n",
           "Acc No", "Type", "Amount", "Date & Time");

    while (fread(&t, sizeof(struct Transaction), 1, fp)) {
        if (t.acc_no == acc_no) {
            printf("%-10d %-10s %-10.2f %-20s\n",
                   t.acc_no, t.type, t.amount, t.date_time);
        }
    }
    printf("\n");

    fclose(fp);
}