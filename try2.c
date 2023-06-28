// gcc assignment.c -o assignment
// ./assignment file_to_sort.txt file_to_modify.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_AGE 150
#define MAX_LINE 1024

void childOne() {
  printf("Child %d here. Will calculate average age.\n", getpid());

  int ages[6];
  int oldest = 0;
  double average = 0.0;
  printf("Please enter 6 ages:\n");
  for (int i = 0; i < 6; i++) {
    printf("(%d): ", i + 1);
    fflush(stdout); // Ensure immediate printing of prompts
    if (scanf("%d", &ages[i]) != 1) {
      printf("Invalid input.\n");
      exit(EXIT_FAILURE);
    }
    oldest = ages[i] > oldest ? ages[i] : oldest;
    average += ages[i];
  }
  average /= 6.0;
  printf("Average age: %.2f, the highest age: %.2f\n", average, (double)oldest);

  exit(0); // Use exit code 0 for successful execution
}

void childTwo(char *file) {
  printf("Child %d here. Will execute command sort.\n", getpid());
  execlp("sort", "sort", "-r", file, (char *)NULL);
  perror("execlp failed");
  exit(1); // Use exit code 1 for execution failure
}

void childThree(char *file) {
  printf("Child %d here. Will update file.\n", getpid());
  FILE *fp = fopen(file, "r+");
  if (!fp) {
    perror("File opening failed");
    exit(1); // Use exit code 1 for file opening failure
  }

  char buffer[MAX_LINE];
  char temp[MAX_LINE];
  fseek(fp, 0, SEEK_SET);
  sprintf(buffer, "This is the updated version.\n");
  while (fgets(temp, sizeof(temp), fp)) {
    char *pos = NULL;
    while ((pos = strstr(temp, "table"))) {
      strncpy(pos, "desk", 4);
    }
    strcat(buffer, temp);
  }
  rewind(fp);
  fputs(buffer, fp);
  fclose(fp);
  exit(0); // Use exit code 0 for successful execution
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Insufficient arguments!\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid1, pid2, pid3;
  int status1, status2, status3;

  pid1 = fork();
  if (pid1 == 0) {
    childOne();
  }

  pid2 = fork();
  if (pid2 == 0) {
    childTwo(argv[1]);
  }

  pid3 = fork();
  if (pid3 == 0) {
    childThree(argv[2]);
  }

  waitpid(pid1, &status1, 0);
  printf("child %d done, the exit status is %d\n", pid1, WEXITSTATUS(status1));

  waitpid(pid2, &status2, 0);
  printf("child %d done, the exit status is %d\n", pid2, WEXITSTATUS(status2));

  waitpid(pid3, &status3, 0);
  printf("child %d done, the exit status is %d\n", pid3, WEXITSTATUS(status3));

  printf("Parent done\n");

  return 0;
}
