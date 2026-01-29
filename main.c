#include<stdio.h>
#include<stdlib.h>
#include "atm.h"

int main() {
    int choice;
    struct Account acc;

    while (1) {
        printf("1. Create Account");
        printf("\n2. Login");
        printf("\n3. Exit\n\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: create_account(); break;
            case 2:
                if (authenticate(&acc))
                    ATM_menu(acc);
                break;
            case 3: exit(0);
        }
    }
}
