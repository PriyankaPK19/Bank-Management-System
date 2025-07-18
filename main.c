#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct Account {
    int accNo;
    char name[100];
    char dob[11];
    char mob[20];
    float balance;
    char password[10];
    struct Account* next;

} Account;

Account* head = NULL;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int isValidName(const char *name) {
    if (name == NULL || strlen(name) == 0) return 0;
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && !isspace(name[i]) &&
            name[i] != '-' && name[i] != '\'') {
            return 0;
        }
    }
    for (int i = 0; name[i]; i++) {
        if (!isspace(name[i])) return 1;
    }
    return 0;
}

int isValidPassword(const char *password)
{
    if(password==NULL||strlen(password)!=4)
      return 0;
    for(int i=0;password[i];i++)
        if(!isdigit(password[i])) return 0;
    return 1;
}

int verifyPassword(Account *acc) {
    char input[10];
    int attempts = 3;

    while (attempts--) {
        printf("Enter password for account %d (Attempts left: %d): ", acc->accNo, attempts + 1);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        //clearInputBuffer();

        if (strcmp(acc->password, input) == 0) {
            return 1;
        } else {
            printf("Incorrect password.\n");
        }
    }
    printf("Too many failed attempts. Access denied.\n");
    return 0;
}


int isDuplicate(int accNo) {
    Account* curr = head;
    while (curr) {
        if (curr->accNo == accNo) return 1;
        curr = curr->next;
    }
    return 0;
}

void loadFromFile() {
    FILE *file = fopen("bank.dat", "rb");
    if (!file) {
        return;
    }
    Account temp;
    while (fread(&temp, sizeof(Account) - sizeof(Account*), 1, file)) {
        Account* newAcc = (Account*)malloc(sizeof(Account));
        *newAcc = temp;
        newAcc->next = head;
        head = newAcc;
    }
    fclose(file);
}

void saveToFile() {
    FILE *file = fopen("bank.dat", "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }
    Account* curr = head;
    while (curr) {
        fwrite(curr, sizeof(Account) - sizeof(Account*), 1, file);
        curr = curr->next;
    }
    fclose(file);
}

int generateAccountNumber(const char *mob, const char *dob) {
    char combined[25];
    int seed = 0;

    // Combine phone and DOB without slashes
    int j = 0;
    for (int i = 0; dob[i]; i++) {
        if (dob[i] != '/') {
            combined[j++] = dob[i];
        }
    }
    combined[j] = '\0';
    strcat(combined, mob);

    // Simple ASCII sum for seeding
    for (int i = 0; combined[i]; i++) {
        seed += combined[i];
    }

    srand(seed);

    // Generate 10-digit number
    int accNo = 1000000000 + rand() % 9000000000;  // Ensure it's 10-digit
    return accNo;
}

void createAccount() {
    int accNo;
    char name[100], dob[11], mob[20];
    float balance;
    char password[10];

    while (1) {
        printf("Enter Full Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        clearInputBuffer();
        if (!isValidName(name)) {
            printf("Invalid name. Use only alphabets, spaces, hyphens, and apostrophes.\n");
        } else {
            break;
        }
    }

    while (1) {
        printf("Enter Date of Birth [DD/MM/YYYY]: ");
        fgets(dob, sizeof(dob), stdin);
        dob[strcspn(dob, "\n")] = '\0';
        //clearInputBuffer();

        int day, month, year;
        if (strlen(dob) != 10 || dob[2] != '/' || dob[5] != '/' ||
            sscanf(dob, "%d/%d/%d", &day, &month, &year) != 3 ||
            day < 1 || day > 31 || month < 1 || month > 12 || year < 1900) {
            printf("Invalid date! Use DD/MM/YYYY format with valid values.\n");
        } else {
            break;
        }
    }
clearInputBuffer();
    while (1) {
        printf("Enter Mobile number: ");
        fgets(mob, sizeof(mob), stdin);
        mob[strcspn(mob, "\n")] = '\0';

        int isValid = 1;
        if (strlen(mob) != 10) isValid = 0;
        else {
            for (int i = 0; i < 10; i++) {
                if (!isdigit(mob[i])) {
                    isValid = 0;
                    break;}
                }
             }
        if (!isValid) {
            printf("Invalid number. Enter a 10-digit number.\n");
        } else {
            break;
        }
    }
    accNo = generateAccountNumber(mob, dob);
    while (isDuplicate(accNo)) {
        accNo++;  // Ensure uniqueness if duplicate exists
    }
    printf("Generated Account Number: %d\n", accNo);
    while(1)
    {
       printf("Set a password:");
       fgets(password,sizeof(password),stdin);
       password[strcspn(password,"\n")]='\0';
       //clearInputBuffer();
       if(!isValidPassword(password))
        printf("Invalid password! enter only 4 digits");
       else
        break;
    }
    while (1) {
        printf("Enter Initial Balance: ");
        if (scanf("%f", &balance) != 1 || balance < 0 || balance > 10000000) {
            printf("Invalid balance. Enter a non-negative number less than 10,000,000.\n");
            //clearInputBuffer();
        } else {
            break;
        }
    }
    clearInputBuffer();

    Account* newAcc = (Account*)malloc(sizeof(Account));
    newAcc->accNo = accNo;
    strcpy(newAcc->name, name);
    strcpy(newAcc->dob, dob);
    strcpy(newAcc->mob, mob);
    strcpy(newAcc->password,password);
    newAcc->balance = balance;
    newAcc->next = head;
    head = newAcc;

    saveToFile();
    printf("Account created successfully!\n");
}

void displayAccounts() {
    Account* curr = head;
    if (!curr) {
        printf("No accounts found.\n");
        return;
    }
    while (curr) {
        printf("\n--- Account Details ---\n");
        printf("Account No: %d\nName: %s\nDOB: %s\nMobile: %s\nBalance: %.2f\n",
               curr->accNo, curr->name, curr->dob, curr->mob, curr->balance);
        printf("-----------------------\n");
        curr = curr->next;
    }
}

Account* findAccount(int accNo) {
    Account* curr = head;
    while (curr) {
        if (curr->accNo == accNo) return curr;
        curr = curr->next;
    }
    return NULL;
}

void checkBalance() {
    int accNo;
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    Account* acc = findAccount(accNo);
    if (!acc) {
        printf("Account not found.\n");
        return;
    }
    if (verifyPassword(acc)) {
        printf("\n--- Account Balance ---\n");
        printf("Account No: %d\nName: %s\nBalance: %.2f\n",
               acc->accNo, acc->name, acc->balance);
        printf("-----------------------\n");
    }
}

void deposit() {
    int accNo;
    float amount;
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    Account* acc = findAccount(accNo);
    if (!acc) {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPassword(acc)) return;

    printf("Current Balance: %.2f\n", acc->balance);
    printf("Enter amount to deposit: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("Invalid amount.\n");
        clearInputBuffer();
        return;
    }
    acc->balance += amount;
    saveToFile();
    printf("Amount Rs %.2f deposited to account %d successfully\n",amount,acc->accNo);
    printf("New Balance: %.2f\n", acc->balance);
}


void withdraw() {
    int accNo;
    float amount;
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    Account* acc = findAccount(accNo);
    if (!acc) {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPassword(acc)) return;

    printf("Current Balance: %.2f\n", acc->balance);
    printf("Enter amount to withdraw: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("Invalid amount.\n");
        clearInputBuffer();
        return;
    }
    if (acc->balance >= amount) {
        acc->balance -= amount;
        saveToFile();
        printf("Rs %.2f debited from account %d successfully!\n",amount,acc->accNo);
        printf("New Balance: %.2f\n", acc->balance);
    } else {
        printf("Insufficient balance.\n");
    }
}

void transfer() {
    int fromAccNo, toAccNo;
    float amount;
    printf("Enter Sender Account Number: ");
    if (scanf("%d", &fromAccNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    printf("Enter Receiver Account Number: ");
    if (scanf("%d", &toAccNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (fromAccNo == toAccNo) {
        printf("Cannot transfer to the same account.\n");
        return;
    }

    Account *from = findAccount(fromAccNo);
    Account *to = findAccount(toAccNo);
    if (!from || !to) {
        printf("Invalid account number(s).\n");
        return;
    }

    if (!verifyPassword(from)) return;

    printf("Sender Current Balance: %.2f\n", from->balance);
    printf("Enter amount to transfer: ");
    if (scanf("%f", &amount) != 1 || amount <= 0) {
        printf("Invalid amount.\n");
        clearInputBuffer();
        return;
    }

    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
        saveToFile();
        printf("Rs %.2f transferred from account %d to account %d successfully \n",amount,from->accNo,to->accNo);
        printf("Sender New Balance: %.2f\n", from->balance);
        printf("Receiver New Balance: %.2f\n", to->balance);
    } else {
        printf("Insufficient funds.\n");
    }
}

void closeAccount() {
    int accNo;
    printf("Enter Account Number to delete: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    Account *curr = head, *prev = NULL;
    while (curr) {
        if (curr->accNo == accNo) {
            if (!verifyPassword(curr)) return;

            if (curr->balance != 0.0f) {
                printf("Account has balance Rs %.2f. Please withdraw the amount before closing.\n", curr->balance);
                return;
            }

            if (prev) prev->next = curr->next;
            else head = curr->next;

            free(curr);
            saveToFile();
            printf("Account is closed successfully.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Account not found.\n");
}



void cleanup() {
    Account* curr = head;
    while (curr) {
        Account* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

int main() {
    int choice;
    loadFromFile();
    do {
        printf("\n--- Bank Management System ---\n");
        printf("1. Create Account\n");
        printf("2. View All Accounts\n");
        printf("3. Check Balance\n");
        printf("4. Deposit\n");
        printf("5. Withdraw\n");
        printf("6. Transfer\n");
        printf("7. Close Account\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: createAccount(); break;
            case 2: displayAccounts(); break;
            case 3: checkBalance(); break;
            case 4: deposit(); break;
            case 5: withdraw(); break;
            case 6: transfer(); break;
            case 7: closeAccount(); break;
            case 8:
                saveToFile();
                cleanup();
                printf("Exiting...\n");
                printf("THANK YOU\n");
                break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 8);
    return 0;
}
