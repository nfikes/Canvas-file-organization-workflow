// By Nathan Fikes with the welcomed assistance of CHAT GPT (Thank you OPEN AI)

// Last Edited: 11/19/2024
// Program Use: To organize massive amounts of files that follow a specific naming convention. Allows CAD and 3D systems to
//              properly load dependencies without loss of original filenames and filename redundancy.

// Windows OS version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// For WinOS Compatability
#include <windows.h>
#include <direct.h>


#define MAX_FILENAME 1024

// A separate function that creates a directory (Windows version)
int create_directory(const char *dir_name) {
    if (_mkdir(dir_name) == 0) {
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    // Windows compatibility: Get the executable path
    char exec_path[MAX_FILENAME];
    DWORD size = sizeof(exec_path);
    if (GetModuleFileName(NULL, exec_path, size) == 0) {
        fprintf(stderr, "Failed to get executable path\n");
        return EXIT_FAILURE;
    }

    // Get the directory containing the executable (Windows-compatible way)
    // Find the last backslash or forward slash
    char *last_separator = strrchr(exec_path, '\\');
    char *last_forward = strrchr(exec_path, '/');
    if (last_forward > last_separator) {
        last_separator = last_forward;
    }
    if (last_separator != NULL) {
        *last_separator = '\0'; // Null-terminate to get just the directory path
    }
    
    if (_chdir(exec_path) != 0) {
        perror("Failed to change working directory to executable's location");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Working directory set to: %s\n", exec_path);

    // Check if we're inside a directory named "submissions"
    char cwd[MAX_FILENAME];
    if (_getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Failed to get current working directory");
        return EXIT_FAILURE;
    }
    
    // Extract the last directory name from the path (handle both forward and backslashes for Windows)
    char *last_dir = strrchr(cwd, '\\');
    if (last_dir == NULL) {
        last_dir = strrchr(cwd, '/'); // Try forward slash if backslash not found
    }
    if (last_dir == NULL) {
        last_dir = cwd; // No slash found, use the entire path
    } else {
        last_dir++; // Move past the slash
    }
    
    // Verify we're in a directory named "submissions"
    if (strcmp(last_dir, "submissions") != 0) {
        fprintf(stderr, "Error: This program must be run from inside a directory named 'submissions'.\n");
        fprintf(stderr, "Current directory: %s\n", last_dir);
        return EXIT_FAILURE;
    }
    
    fprintf(stdout, "Verified: Running in 'submissions' directory.\n\n");

    // Start the timer
    clock_t start_time = clock();

    // First pass: collect all filenames to avoid issues with directory modification during iteration
    char **filenames = NULL;
    int file_count = 0;
    int file_capacity = 0;

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("Failed to open directory");
        return EXIT_FAILURE;
    }

    // Collect all filenames first
    do {
        // Skip directories and hidden files (starting with '.')
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || findFileData.cFileName[0] == '.') {
            continue;
        }

        // Expand array if needed
        if (file_count >= file_capacity) {
            file_capacity = (file_capacity == 0) ? 100 : file_capacity * 2;
            char **new_filenames = (char **)realloc(filenames, file_capacity * sizeof(char *));
            if (new_filenames == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                FindClose(hFind);
                return EXIT_FAILURE;
            }
            filenames = new_filenames;
        }

        // Allocate and copy filename
        filenames[file_count] = (char *)malloc(strlen(findFileData.cFileName) + 1);
        if (filenames[file_count] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            FindClose(hFind);
            return EXIT_FAILURE;
        }
        strcpy(filenames[file_count], findFileData.cFileName);
        file_count++;

    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    int dir_created = 0;
    int files_tracked = 0;
    int submissions_late = 0;
    int files_failed = 0;

    // Second pass: process all collected filenames
    for (int i = 0; i < file_count; i++) {
        char *filename = filenames[i];
        
        // Skip executables and files without underscore
        size_t filename_len = strlen(filename);
        if ((filename_len > 4 && _stricmp(filename + filename_len - 4, ".exe") == 0) || 
            strchr(filename, '_') == NULL) {
            continue;
        }

        files_tracked += 1;
        char *underscore_pos = strchr(filename, '_');

        size_t name_length = underscore_pos - filename;
        char student_name[MAX_FILENAME];
        strncpy(student_name, filename, name_length);
        student_name[name_length] = '\0';

        // Check if the file is late
        int is_late_file = (strncmp(underscore_pos + 1, "LATE_", 5) == 0);

        // Generate the student directory name
        char student_dir[MAX_FILENAME];
        snprintf(student_dir, sizeof(student_dir), "%s%s", student_name, is_late_file ? " [LATE]" : "");

        // Create the directory (only once per student)
        if (create_directory(student_dir) == 0) {
            fprintf(stdout, "Created directory for: %s\n", student_dir);
            dir_created += 1;
        }

        // Print progress message
        fprintf(stdout, "Processing files for student: %s\n", student_name);

        // Process the filename to remove the Canvas identifiers
        char new_filename[MAX_FILENAME];
        if (is_late_file) {
            printf("Removed LATE identifier...\n");
            submissions_late += 1;
            snprintf(new_filename, sizeof(new_filename), "%s", underscore_pos + 6);
        } else {
            snprintf(new_filename, sizeof(new_filename), "%s", underscore_pos + 1);
        }

        // Remove the unique identifier (first 16 characters)
        if (strlen(new_filename) > 16) {
            printf("Removing unique identifier number in: %s \n", new_filename);
            memmove(new_filename, new_filename + 16, strlen(new_filename) - 16 + 1);
        }

        // Check for the '-1' redundancy suffix
        const char *dot = strrchr(new_filename, '.');
        const char *hyphen = strrchr(new_filename, '-');
        if (hyphen && (dot - hyphen == 2) && isdigit(*(hyphen + 1))) {
            printf("Found '-#' indicator before the extension in: %s\nRemoving...\n", new_filename);
            size_t new_name_length = hyphen - new_filename;
            snprintf(new_filename, sizeof(new_filename), "%.*s%s", (int)new_name_length, new_filename, dot);
        }

        // Build the source and destination paths (use Windows backslashes)
        char source_path[MAX_FILENAME];
        char destination_path[MAX_FILENAME];
        snprintf(source_path, sizeof(source_path), "%s", filename);
        snprintf(destination_path, sizeof(destination_path), "%s\\%s", student_dir, new_filename);

        // Move the file to the new location
        if (rename(source_path, destination_path) != 0) {
            files_failed++;
        } else {
            fprintf(stdout, "Identified Original File Name: %s \n", new_filename);
            fprintf(stdout, "Moving file...\n");
        }
    }

    // Free allocated memory
    for (int i = 0; i < file_count; i++) {
        free(filenames[i]);
    }
    free(filenames);

    // End the timer and calculate elapsed time
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print summary
    fprintf(stdout, "\n\nProcess Finished without error\n");
    fprintf(stdout, "Total processing time: \t\t\t%.2f [seconds]\n", elapsed_time);
    fprintf(stdout, "Average processing time per file: \t%.4f [milliseconds]\n", (elapsed_time/((double)files_tracked))*1000.0);
    fprintf(stdout, "Files tracked: \t\t\t\t%d [files]\n", files_tracked);
    fprintf(stdout, "Files failed to move: \t\t\t%d [files]\n", files_failed);
    fprintf(stdout, "Directories created: \t\t\t%d [locations]\n", dir_created);
    fprintf(stdout, "Submissions late: \t\t\t%d [submissions]\n", submissions_late);

    return EXIT_SUCCESS;
}
