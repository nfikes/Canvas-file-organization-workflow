// By Nathan Fikes with the welcomed assistance of CHAT GPT (Thank you OPEN AI)
// Now with Claude Sonnet 4.5 in 2025

// Last Edited: 11/17/2025
// Program Use: To organize massive amounts of files that follow a specific naming convention. Allows CAD and 3D systems to
//              properly load dependencies without loss of original filenames and filename redundancy.

// Windows OS version with Unicode support

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

// For WinOS Compatability
#include <windows.h>
#include <direct.h>


#define MAX_FILENAME 1024

// A separate function that creates a directory (Windows Unicode version)
int create_directory_w(const wchar_t *dir_name) {
    if (_wmkdir(dir_name) == 0) {
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    // Set locale and console to UTF-8 for proper Unicode support
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Windows compatibility: Get the executable path
    wchar_t exec_path[MAX_FILENAME];
    DWORD size = sizeof(exec_path) / sizeof(wchar_t);
    if (GetModuleFileNameW(NULL, exec_path, size) == 0) {
        fwprintf(stderr, L"Failed to get executable path\n");
        return EXIT_FAILURE;
    }

    // Get the directory containing the executable (Windows-compatible way)
    wchar_t *last_separator = wcsrchr(exec_path, L'\\');
    wchar_t *last_forward = wcsrchr(exec_path, L'/');
    if (last_forward > last_separator) {
        last_separator = last_forward;
    }
    if (last_separator != NULL) {
        *last_separator = L'\0';
    }
    
    if (_wchdir(exec_path) != 0) {
        fwprintf(stderr, L"Failed to change working directory to executable's location\n");
        return EXIT_FAILURE;
    }
    wprintf(L"Working directory set to: %ls\n", exec_path);

    // Check if we're inside a directory named "submissions"
    wchar_t cwd[MAX_FILENAME];
    if (_wgetcwd(cwd, MAX_FILENAME) == NULL) {
        fwprintf(stderr, L"Failed to get current working directory\n");
        return EXIT_FAILURE;
    }
    
    // Extract the last directory name from the path
    wchar_t *last_dir = wcsrchr(cwd, L'\\');
    if (last_dir == NULL) {
        last_dir = wcsrchr(cwd, L'/');
    }
    if (last_dir == NULL) {
        last_dir = cwd;
    } else {
        last_dir++;
    }
    
    // Verify we're in a directory named "submissions"
    if (wcscmp(last_dir, L"submissions") != 0) {
        fwprintf(stderr, L"Error: This program must be run from inside a directory named 'submissions'.\n");
        fwprintf(stderr, L"Current directory: %ls\n", last_dir);
        return EXIT_FAILURE;
    }
    
    wprintf(L"Verified: Running in 'submissions' directory.\n\n");

    // Start the timer
    clock_t start_time = clock();

    // First pass: collect all filenames
    wchar_t **filenames = NULL;
    int file_count = 0;
    int file_capacity = 0;

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(L"*", &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"Failed to open directory\n");
        return EXIT_FAILURE;
    }

    // Collect all filenames first
    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || findFileData.cFileName[0] == L'.') {
            continue;
        }

        if (file_count >= file_capacity) {
            file_capacity = (file_capacity == 0) ? 100 : file_capacity * 2;
            wchar_t **new_filenames = (wchar_t **)realloc(filenames, file_capacity * sizeof(wchar_t *));
            if (new_filenames == NULL) {
                fwprintf(stderr, L"Memory allocation failed\n");
                FindClose(hFind);
                return EXIT_FAILURE;
            }
            filenames = new_filenames;
        }

        filenames[file_count] = (wchar_t *)malloc((wcslen(findFileData.cFileName) + 1) * sizeof(wchar_t));
        if (filenames[file_count] == NULL) {
            fwprintf(stderr, L"Memory allocation failed\n");
            FindClose(hFind);
            return EXIT_FAILURE;
        }
        wcscpy(filenames[file_count], findFileData.cFileName);
        file_count++;

    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);

    int dir_created = 0;
    int files_tracked = 0;
    int submissions_late = 0;
    int files_failed = 0;

    // Second pass: process all collected filenames
    for (int i = 0; i < file_count; i++) {
        wchar_t *filename = filenames[i];
        
        // Skip executables and files without underscore
        size_t filename_len = wcslen(filename);
        if ((filename_len > 4 && _wcsicmp(filename + filename_len - 4, L".exe") == 0) || 
            wcschr(filename, L'_') == NULL) {
            continue;
        }

        files_tracked += 1;
        wchar_t *underscore_pos = wcschr(filename, L'_');

        size_t name_length = underscore_pos - filename;
        wchar_t student_name[MAX_FILENAME];
        wcsncpy(student_name, filename, name_length);
        student_name[name_length] = L'\0';

        int is_late_file = (wcsncmp(underscore_pos + 1, L"LATE_", 5) == 0);

        wchar_t student_dir[MAX_FILENAME];
        swprintf(student_dir, L"%ls%ls", student_name, is_late_file ? L" [LATE]" : L"");

        if (create_directory_w(student_dir) == 0) {
            wprintf(L"Created directory for: %ls\n", student_dir);
            dir_created += 1;
        }

        wprintf(L"Processing files for student: %ls\n", student_name);

        wchar_t new_filename[MAX_FILENAME];
        if (is_late_file) {
            wprintf(L"Removed LATE identifier...\n");
            submissions_late += 1;
            swprintf(new_filename, L"%ls", underscore_pos + 6);
        } else {
            swprintf(new_filename, L"%ls", underscore_pos + 1);
        }

        if (wcslen(new_filename) > 16) {
            wprintf(L"Removing unique identifier number in: %ls \n", new_filename);
            wmemmove(new_filename, new_filename + 16, wcslen(new_filename) - 16 + 1);
        }

        const wchar_t *dot = wcsrchr(new_filename, L'.');
        const wchar_t *hyphen = wcsrchr(new_filename, L'-');
        if (hyphen && (dot - hyphen == 2) && iswdigit(*(hyphen + 1))) {
            wprintf(L"Found '-#' indicator before the extension in: %ls\nRemoving...\n", new_filename);
            size_t new_name_length = hyphen - new_filename;
            swprintf(new_filename, L"%.*ls%ls", (int)new_name_length, new_filename, dot);
        }

        wchar_t source_path[MAX_FILENAME];
        wchar_t destination_path[MAX_FILENAME];
        swprintf(source_path, L"%ls", filename);
        swprintf(destination_path, L"%ls\\%ls", student_dir, new_filename);

        if (_wrename(source_path, destination_path) != 0) {
            files_failed++;
        } else {
            wprintf(L"Identified Original File Name: %ls \n", new_filename);
            wprintf(L"Moving file...\n");
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
    wprintf(L"\n\nProcess Finished without error\n");
    wprintf(L"Total processing time: \t\t\t%.2f [seconds]\n", elapsed_time);
    wprintf(L"Average processing time per file: \t%.4f [milliseconds]\n", (elapsed_time/((double)files_tracked))*1000.0);
    wprintf(L"Files tracked: \t\t\t\t%d [files]\n", files_tracked);
    wprintf(L"Files failed to move: \t\t\t%d [files]\n", files_failed);
    wprintf(L"Directories created: \t\t\t%d [locations]\n", dir_created);
    wprintf(L"Submissions late: \t\t\t%d [submissions]\n", submissions_late);

    return EXIT_SUCCESS;
}
