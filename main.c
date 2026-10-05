#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "appointments.dat"
#define NAME_LEN 30
#define TIME_LEN 10
#define MAX_PATIENTS 1000  // Prevents sparse file explosion (Max 1000 records)

typedef struct {
    int patient_id;
    char name[NAME_LEN];
    char appointment_time[TIME_LEN];
} Patient;

/* Helper & Validation Functions */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int is_valid_time(const char *time_str) {
    // Expected format: HH:MM (length must be exactly 5)
    if (strlen(time_str) != 5 || time_str[2] != ':') {
        return 0;
    }

    if (!isdigit(time_str[0]) || !isdigit(time_str[1]) ||
        !isdigit(time_str[3]) || !isdigit(time_str[4])) {
        return 0;
    }

    int hours = (time_str[0] - '0') * 10 + (time_str[1] - '0');
    int minutes = (time_str[3] - '0') * 10 + (time_str[4] - '0');

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return 0;
    }

    return 1;
}

/* Function Prototypes */
void add_patient();
void search_patient();
void update_appointment();
void list_all_patients();

int main() {
    int choice;

    do {
        printf("\n====================================\n");
        printf("   HOSPITAL RECORD SYSTEM (BINARY)\n");
        printf("====================================\n");
        printf("1. Add / Register Patient\n");
        printf("2. Search Patient by ID\n");
        printf("3. Update Appointment Time\n");
        printf("4. List All Records\n");
        printf("0. Exit\n");
        printf("Select an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[ERROR] Invalid input! Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                add_patient();
                break;
            case 2:
                search_patient();
                break;
            case 3:
                update_appointment();
                break;
            case 4:
                list_all_patients();
                break;
            case 0:
                printf("\nExiting system. Goodbye!\n");
                break;
            default:
                printf("\n[ERROR] Invalid option. Please try again.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}

void add_patient() {
    FILE *file = fopen(FILE_NAME, "rb+");
    if (file == NULL) {
        file = fopen(FILE_NAME, "wb+");
        if (file == NULL) {
            printf("\n[ERROR] Could not open database file!\n");
            return;
        }
    }

    Patient new_patient = {0}; // Fix 3: Zero initialization

    printf("\nEnter Patient ID (1 - %d): ", MAX_PATIENTS);
    if (scanf("%d", &new_patient.patient_id) != 1 ||
        new_patient.patient_id < 1 ||
        new_patient.patient_id > MAX_PATIENTS) { // Fix 1: Upper limit bound
        printf("\n[ERROR] Patient ID must be between 1 and %d!\n", MAX_PATIENTS);
        clear_input_buffer();
        fclose(file);
        return;
    }
    clear_input_buffer();

    // Check if ID already exists
    Patient temp = {0};
    fseek(file, (long)(new_patient.patient_id - 1) * sizeof(Patient), SEEK_SET);
    if (fread(&temp, sizeof(Patient), 1, file) == 1 && temp.patient_id != 0) {
        printf("\n[ERROR] Patient ID %d is already registered!\n", new_patient.patient_id);
        fclose(file);
        return;
    }

    printf("Enter Patient Full Name: ");
    if (fgets(new_patient.name, NAME_LEN, stdin) != NULL) {
        char *newline = strchr(new_patient.name, '\n');
        if (newline) {
            *newline = '\0';
        } else {
            clear_input_buffer(); // Fix 2: Clear buffer if input exceeded NAME_LEN
        }
    }

    if (strlen(new_patient.name) == 0) {
        printf("\n[ERROR] Patient name cannot be empty!\n");
        fclose(file);
        return;
    }

    printf("Enter Appointment Time (e.g., 14:30): ");
    if (scanf("%9s", new_patient.appointment_time) != 1 || !is_valid_time(new_patient.appointment_time)) { // Fix 4: Time validation
        printf("\n[ERROR] Invalid time format! Must be HH:MM (e.g., 09:15 or 14:30).\n");
        clear_input_buffer();
        fclose(file);
        return;
    }
    clear_input_buffer();

    // Direct offset seek and binary write
    fseek(file, (long)(new_patient.patient_id - 1) * sizeof(Patient), SEEK_SET);
    fwrite(&new_patient, sizeof(Patient), 1, file);

    fclose(file);
    printf("\n[SUCCESS] Patient '%s' registered with ID %d.\n", new_patient.name, new_patient.patient_id);
}

void search_patient() {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL) {
        printf("\n[WARNING] Database is empty or file not found.\n");
        return;
    }

    int search_id;
    printf("\nEnter Patient ID to search (1 - %d): ", MAX_PATIENTS);
    if (scanf("%d", &search_id) != 1 || search_id < 1 || search_id > MAX_PATIENTS) {
        printf("\n[ERROR] Invalid Patient ID!\n");
        clear_input_buffer();
        fclose(file);
        return;
    }
    clear_input_buffer();

    Patient found_patient = {0};
    fseek(file, (long)(search_id - 1) * sizeof(Patient), SEEK_SET);

    if (fread(&found_patient, sizeof(Patient), 1, file) == 1 && found_patient.patient_id != 0) {
        printf("\n[FOUND] Patient ID: %d | Name: %s | Appointment: %s\n",
               found_patient.patient_id, found_patient.name, found_patient.appointment_time);
    } else {
        printf("\n[NOT FOUND] No active record for Patient ID %d.\n", search_id);
    }

    fclose(file);
}

void update_appointment() {
    FILE *file = fopen(FILE_NAME, "rb+");
    if (file == NULL) {
        printf("\n[WARNING] Database is empty or file not found.\n");
        return;
    }

    int update_id;
    printf("\nEnter Patient ID to update appointment: ");
    if (scanf("%d", &update_id) != 1 || update_id < 1 || update_id > MAX_PATIENTS) {
        printf("\n[ERROR] Invalid Patient ID!\n");
        clear_input_buffer();
        fclose(file);
        return;
    }
    clear_input_buffer();

    Patient patient = {0};
    fseek(file, (long)(update_id - 1) * sizeof(Patient), SEEK_SET);

    if (fread(&patient, sizeof(Patient), 1, file) == 1 && patient.patient_id != 0) {
        printf("\nCurrent Appointment Time: %s\n", patient.appointment_time);

        char new_time[TIME_LEN];
        printf("Enter New Appointment Time (e.g., 10:15): ");
        if (scanf("%9s", new_time) != 1 || !is_valid_time(new_time)) {
            printf("\n[ERROR] Invalid time format! Update cancelled.\n");
            clear_input_buffer();
            fclose(file);
            return;
        }
        clear_input_buffer();

        strcpy(patient.appointment_time, new_time);

        // Move cursor back and update
        fseek(file, (long)(update_id - 1) * sizeof(Patient), SEEK_SET);
        fwrite(&patient, sizeof(Patient), 1, file);

        printf("\n[SUCCESS] Appointment updated successfully for ID %d.\n", update_id);
    } else {
        printf("\n[ERROR] Patient ID %d not found.\n", update_id);
    }

    fclose(file);
}

void list_all_patients() {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL) {
        printf("\n[WARNING] Database is empty or file not found.\n");
        return;
    }

    Patient p = {0};
    int count = 0;

    printf("\n-----------------------------------------------------\n");
    printf(" ID\t\tPATIENT NAME\t\tAPPOINTMENT\n");
    printf("-----------------------------------------------------\n");

    while (fread(&p, sizeof(Patient), 1, file) == 1) {
        if (p.patient_id != 0) {
            printf(" %-8d\t%-20s\t%s\n", p.patient_id, p.name, p.appointment_time);
            count++;
        }
    }

    if (count == 0) {
        printf(" No active patient records found.\n");
    }
    printf("-----------------------------------------------------\n");

    fclose(file);
}
