// Include necessary standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
// Run
// gcc fork_children.c -o fork_children
// ./fork_children file1.txt file2.txt

Define constants for maximum age and maximum line length.
#define MAX_AGE 150
#define MAX_LINE 1024

// Function for the first child process to get input and calculate average age.
void childOne() {
    int ages[6];  // Declare an array to store the ages.
    int oldest = 0;  // Initialize the oldest age to 0.
    double average = 0.0;  // Initialize the average to 0.

    // Loop to get input from the user.
    for (int i = 0; i < 6; i++) {
        printf("Enter age %d: ", i + 1);
        // Input validation. If it's not a number, prompt the user to reenter.
        if(scanf("%d", &ages[i]) == 1){
            average += ages[i];  // Calculate the sum of ages.
            // Find the oldest age.
            if (ages[i] > oldest) oldest = ages[i];
        } else {
            printf("Invalid input! Please enter a number.\n");
            i--;  // Decrement i to repeat the current iteration.
        }
    }
    average /= 6.0;  // Calculate the average age.

    // Print the average and the oldest age.
    printf("Child One (PID: %d): Average Age: %.2f, Oldest: %d\n", getpid(), average, oldest);
    
    // End the child process.
    exit(0);
}

// Function for the second child process to sort the input file in reverse.
void childTwo(char *file) {
    // Execute the sort command. '-r' option for reverse sorting.
    execlp("sort", "sort", "-r", file, NULL);
    
    // If execlp returns, an error occurred. Print the error and exit.
    perror("Child Two: exec error");
    exit(1);
}

// Function for the third child process to modify a given file.
void childThree(char *file) {
    // Open the file in read and update mode.
    FILE *fp = fopen(file, "r+");
    
    // If fopen returns NULL, an error occurred. Print the error and exit.
    if (!fp) {
        perror("Child Three: File open error");
        exit(1);
    }

    // Declare a buffer to store the file content.
    char buffer[MAX_LINE] = "";
    char temp[MAX_LINE];
    
    // Move the file position to the beginning.
    fseek(fp, 0, SEEK_SET);
    
    // Add a new line to the file.
    strncat(buffer, "This is the updated version.\n", MAX_LINE - strlen(buffer) - 1);
    
    // Read the file line by line.
    while (fgets(temp, sizeof(temp), fp)) {
        char *pos = NULL;
        // Replace the word "table" with "desk".
        while ((pos = strstr(temp, "table"))) {
            strncpy(pos, "desk", 4);
        }
        // Concatenate the modified line to the buffer.
        strncat(buffer, temp, MAX_LINE - strlen(buffer) - 1);
    }
    
    // Move the file position to the beginning.
    rewind(fp);
    
    // Write the buffer content to the file.
    fputs(buffer, fp);
    
    // Close the file.
    fclose(fp);    // Print a success message.
    printf("Child Three (PID: %d): File updated successfully\n", getpid());

    // End the child process.
    exit(0);
}

// The main function of the program.
int main(int argc, char *argv[]) {
    // Ensure there are exactly 2 command-line arguments.
    if (argc != 3) {
        printf("Error: Invalid number of arguments\n");
        printf("Usage: %s <file_to_sort> <file_to_modify>\n", argv[0]);
        // Terminate the program due to invalid input.
        exit(EXIT_FAILURE);
    }

    // Declare variables for process IDs.
    pid_t pid1, pid2, pid3;
    int status;

    // Create the first child process.
    pid1 = fork();
    // If fork returns -1, an error occurred.
    if (pid1 < 0) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {  // In the child process, fork returns 0.
        // Call the first child's function.
        childOne();
    }

    // Create the second child process.
    pid2 = fork();
    // If fork returns -1, an error occurred.
    if (pid2 < 0) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {  // In the child process, fork returns 0.
        // Call the second child's function.
        childTwo(argv[1]);
    }

    // Create the third child process.
    pid3 = fork();
    // If fork returns -1, an error occurred.
    if (pid3 < 0) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    } else if (pid3 == 0) {  // In the child process, fork returns 0.
        // Call the third child's function.
        childThree(argv[2]);
    }

    // Wait for the first child to finish and print its exit status.
    waitpid(pid1, &status, 0);
    printf("Child One (PID: %d) finished with exit status %d\n", pid1, WEXITSTATUS(status));

    // Wait for the second child to finish and print its exit status.
    waitpid(pid2, &status, 0);
    printf("Child Two (PID: %d) finished with exit status %d\n", pid2, WEXITSTATUS(status));

    // Wait for the third child to finish and print its exit status.
    waitpid(pid3, &status, 0);
    printf("Child Three (PID: %d) finished with exit status %d\n", pid3, WEXITSTATUS(status));

    // Print a message to indicate that the parent is ending.
    printf("Parent (PID: %d) ending.\n", getpid());

    // End the program.
    return 0;
}
