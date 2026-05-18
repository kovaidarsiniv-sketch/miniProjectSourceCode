// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

// clientData structure definition
struct clientData {
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
}; // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[]) {
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // Try to open existing file, otherwise create new one
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL) {
            printf("%s: File could not be opened.\n", argv[0]);
            exit(-1);
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 5) {
        switch (choice) {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr); // fclose closes the file
    return 0;
}

// create formatted text file for printing
void textFile(FILE *readPtr) {
    FILE *writePtr; // accounts.txt file pointer
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("File could not be opened.");
    } else {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
            if (client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum, client.lastName,
                        client.firstName, client.balance);
            }
        }
        fclose(writePtr);
    }
}

// update balance in record
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("%s", "Enter account to update ( 1 - 100 ): ");
    if (scanf("%u", &account) != 1) {
        puts("Invalid input.");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account #%d has no information.\n", account);
    } else {
        printf("%-6d%-16s%-11s%10.2f\n\n",
               client.acctNum, client.lastName,
               client.firstName, client.balance);

        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1) {
            puts("Invalid input.");
            return;
        }
        client.balance += transaction;

        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum, client.lastName,
               client.firstName, client.balance);

        fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// delete an existing record
void deleteRecord(FILE *fPtr) {
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};
    unsigned int accountNum;

    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        puts("Invalid input.");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account %d does not exist.\n", accountNum);
    } else {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// create and insert record
void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("%s", "Enter new account number ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        puts("Invalid input.");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account #%d already contains information.\n", client.acctNum);
    } else {
        printf("%s", "Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3) {
            puts("Invalid input.");
            return;
        }
        client.acctNum = accountNum;
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// enable user to input menu choice
unsigned int enterChoice(void) {
    unsigned int menuChoice;
    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1) {
        puts("Invalid input.");
        return 5; // end program on invalid input
    }
    return menuChoice;
}
