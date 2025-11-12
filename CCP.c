#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "text.txt"   // File to store account data
#define MAX_ACCOUNTS 10        // Maximum number of accounts allowed

// Structure to hold single account info
typedef struct {
    int pin;
    char name[50];
    float balance;
} Account;

// Structure to hold all accounts
typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int count;
} AccountList;

// Load all accounts from file
int load_all_account_data(AccountList *list) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("Account file not found (%s).\n", FILE_NAME);
        return 0;
    }

    list->count = 0;

    // Read each account line by line
    while (list->count < MAX_ACCOUNTS) {
        int result = fscanf(
            file, 
            "%d,%49[^,],%f\n",
            &list->accounts[list->count].pin, 
            list->accounts[list->count].name, 
            &list->accounts[list->count].balance
        );
        
        if (result == 3) {
            list->count++;
        } else if (result == EOF) {
            break;
        } else {
            printf("Error reading data near line %d.\n", list->count + 1);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    printf("Loaded %d account(s).\n", list->count);
    return 1;
}

// Save all accounts back to file
void save_all_account_data(const AccountList *list) {
    FILE *file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        printf("Cannot save account data.\n");
        return;
    }

    // Write each account to file
    for (int i = 0; i < list->count; i++) {
        fprintf(file, "%d,%s,%.2f\n", 
            list->accounts[i].pin, 
            list->accounts[i].name, 
            list->accounts[i].balance);
    }

    fclose(file);
}

// Main ATM function
void atm_machine() {
    AccountList account_list;
    int entered_pin;
    int logged_in_index = -1;

    // Load accounts from file
    if (!load_all_account_data(&account_list)) {
        return;
    }

    // Login step
    printf("\n--- sharcodec ATM Login ---\n");
    printf("Enter your PIN: ");
    while (getchar() != '\n'); // Clear input buffer

    if (scanf("%d", &entered_pin) != 1) {
        printf("Invalid PIN.\n");
        while (getchar() != '\n'); 
        return;
    }
    
    // Match entered PIN
    for (int i = 0; i < account_list.count; i++) {
        if (entered_pin == account_list.accounts[i].pin) {
            logged_in_index = i;
            break;
        }
    }

    if (logged_in_index == -1) {
        printf("Incorrect PIN.\n");
        return;
    }

    Account *active_account = &account_list.accounts[logged_in_index];
    printf("\nWelcome %s!\n", active_account->name);

    int choice;
    float amount;
    
    // ATM menu loop
    while (1) {
        printf("\n--- Sharcodec ATM Menu ---\n");
        printf("1. Withdraw\n");
        printf("2. Deposit\n");
        printf("3. Check Balance\n");
        printf("4. Exit\n");
        printf("Choose option (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {  // Withdraw
            printf("Enter amount to withdraw: ");
            if (scanf("%f", &amount) != 1 || amount <= 0) {
                printf("Invalid amount.\n");
                while (getchar() != '\n'); 
                continue;
            }

            if (amount > active_account->balance) {
                printf("Insufficient balance.\n");
            } else {
                active_account->balance -= amount;
                printf("Withdrawn $%.2f. New balance: $%.2f\n", amount, active_account->balance);
                save_all_account_data(&account_list);
            }

        } else if (choice == 2) {  // Deposit
            printf("Enter amount to deposit: ");
            if (scanf("%f", &amount) != 1 || amount <= 0) {
                printf("Invalid amount.\n");
                while (getchar() != '\n'); 
                continue;
            }

            active_account->balance += amount;
            printf("Deposited $%.2f. New balance: $%.2f\n", amount, active_account->balance);
            save_all_account_data(&account_list);

        } else if (choice == 3) {  // Check Balance
            printf("Current balance: $%.2f\n", active_account->balance);

        } else if (choice == 4) {  // Exit
            printf("Thank you, %s. Goodbye!\n", active_account->name);
            break;

        } else {
            printf("Invalid option.\n");
        }
    }
}

int main() {
    atm_machine();
    return 0;
}
