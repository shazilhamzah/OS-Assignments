#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int execute_chassis();
int execute_engine();
int execute_painting();
int execute_final();

int main() {
    srand(time(NULL));
    printf("Starting Car Assembly...\n");

    int chassis_attempts = 1;
    int engine_attempts = 1;
    int painting_attempts = 0;
    int final_attempts = 0;

    // Chassis Manufacturing
    if (!execute_chassis()) {
        printf("ASSEMBLY LINE SHUT DOWN\n");
        return 1;
    }

    // Engine Installation
    if (!execute_engine()) {
        printf("ASSEMBLY LINE SHUT DOWN\n");
        return 1;
    }

    // Painting
    if (!execute_painting()) {
        printf("ASSEMBLY LINE SHUT DOWN\n");
        return 1;
    } else {
        // Read painting_attempts from file
        FILE *f = fopen("painting_attempts.txt", "r");
        if (f) {
            fscanf(f, "%d", &painting_attempts);
            fclose(f);
        }
    }

    // Final Inspection
    if (!execute_final()) {
        printf("ASSEMBLY LINE SHUT DOWN\n");
        return 1;
    } else {
        // Read final_attempts from file
        FILE *f = fopen("final_attempts.txt", "r");
        if (f) {
            fscanf(f, "%d", &final_attempts);
            fclose(f);
        }
    }

    printf("Car successfully manufactured!\n");
    printf("Assembly Summary:\n");
    printf("- Chassis: %d attempts\n", chassis_attempts);
    printf("- Engine: %d attempts\n", engine_attempts);
    printf("- Painting: %d attempts\n", painting_attempts);
    printf("- Final Inspection: %d attempts\n", final_attempts);

    return 0;
}

int execute_chassis() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: Chassis Manufacturing
        pid_t sub1 = fork();
        if (sub1 == 0) {
            printf("✅ Welding attempt 1: Success\n");
            exit(0);
        }
        pid_t sub2 = fork();
        if (sub2 == 0) {
            printf("✅ Assembling frame attempt 1: Success\n");
            exit(0);
        }

        int status;
        int success = 1;
        waitpid(sub1, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            success = 0;
        waitpid(sub2, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            success = 0;

        exit(success ? 0 : 1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

int execute_engine() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: Engine Installation
        pid_t sub1 = fork();
        if (sub1 == 0) {
            printf("✅ Engine mounting attempt 1: Success\n");
            exit(0);
        }
        pid_t sub2 = fork();
        if (sub2 == 0) {
            printf("✅ Electrical systems attempt 1: Success\n");
            exit(0);
        }
        pid_t sub3 = fork();
        if (sub3 == 0) {
            printf("✅ Engine testing attempt 1: Success\n");
            exit(0);
        }

        int status;
        int success = 1;
        waitpid(sub1, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            success = 0;
        waitpid(sub2, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            success = 0;
        waitpid(sub3, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            success = 0;

        exit(success ? 0 : 1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

int execute_painting() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: Painting
        int attempts = 0;
        int success = 0;
        for (int i = 0; i < 2; i++) {
            attempts++;
            pid_t attempt_pid = fork();
            if (attempt_pid == 0) {
                srand(getpid());
                int r = rand() % 2;
                printf(r ? "✅ Painting attempt %d: Success\n" : "❌ Painting attempt %d: FAILED\n", attempts);
                exit(r ? 0 : 1);
            } else {
                int status;
                waitpid(attempt_pid, &status, 0);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        FILE *f = fopen("painting_attempts.txt", "w");
        if (f) {
            fprintf(f, "%d", attempts);
            fclose(f);
        }
        exit(success ? 0 : 1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

int execute_final() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: Final Inspection
        int attempts = 0;
        int success = 0;
        for (int i = 0; i < 2; i++) {
            attempts++;
            pid_t attempt_pid = fork();
            if (attempt_pid == 0) {
                srand(getpid());
                int r = rand() % 2;
                printf(r ? "✅ Final Inspection attempt %d: Success\n" : "❌ Final Inspection attempt %d: FAILED\n", attempts);
                exit(r ? 0 : 1);
            } else {
                int status;
                waitpid(attempt_pid, &status, 0);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        FILE *f = fopen("final_attempts.txt", "w");
        if (f) {
            fprintf(f, "%d", attempts);
            fclose(f);
        }
        exit(success ? 0 : 1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}