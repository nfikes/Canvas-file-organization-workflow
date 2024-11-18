// By Nathan Fikes with the welcomed assistance of CHAT GPT (Thank you OPEN AI)

// Last Edited: 11/18/2024
// Program Use: To organize massive amounts of files that follow a specific naming convention. Allows CAD and 3D systems to
//              properly load dependencies without loss of original filenames and filename redundancy.

//Some libries here are able to provide terminal commands to edit a directory the file is in, move files, change filenames.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

// For IOS Compatability
#include <mach-o/dyld.h>
#include <libgen.h>

//====================================================================================================================//
// Useful information on how to use this small program to quickly organize thousands of Downloaded Canvas submissions //

// 1. Download all the submissions from the canvas page into a zip file.

// 2. Extract all submission files into any folder in any directory, make sure they are not inside a subfolder.
//    Check to make sure the submissions have the following format (the program will convert from the format below):

//    "[name]_[LATE]_123456_12345678_[OriginalFilename][-1 redundancy suffix].[extension]"
//     converts to:
//    "[OriginalFilename].[extension]"

//  Example dataset (Mixture of NX files and PDF):

//  david_LATE_226674_36384427_.25_.75Screw.prt                 ->  david [LATE]/.25_.75Screw.prt
//  john_203772_36359031_ThebikeAssembly-1.prt                  ->  john/ThebikeAssembly.prt
//  jack_LATE_199184_36332751_MyModel.pdf                       ->  jack [LATE]/MyModel.pdf
//  christina_LATE_201106_36357737_M3-0.5 Screw.STEP-1_stp.prt  ->  christina [LATE]/M3-0.5 Screw.STEP-1_stp.prt
//  julia_226171_36362062_Wedge_Pin-1.prt                       ->  julia/Wedge_Pin.prt
//  mary_198429_36345787_Finished Report.pdf                    ->  mary/Finished Report.pdf

// 3. Move the EXEC (for MAC) or EXE (for Windows) into the directory where you want to organize files. Double click it.

// 4. Observe the Terminal Output and then check the folder where the submissions are,
//    if there are no errors, all the submissions will have their Canvas download identifiers removed.
//    All organized files will have their original submitted names, their subdirectories will inherit
//    the name and LATE attribute designated by each file, and subdirectiories will prevent file
//    name redundancy in the system.
//====================================================================================================================//


//File names should be of a predictable size, if there are any really long ones, 1024 should catch most if not all.
#define MAX_FILENAME 1024

// A separate function that takes a dereferenced pointer
int create_directory(const char *dir_name) {
    struct stat st = {0};

    // We take the pointer and ask for its information to be put into st.
    // If there happens to be no directory, we can make a new one using the name given to the function.
    // 0700 is a permission set for read, write, and execute
    if (mkdir(dir_name, 0700) == 0) {
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {

    // MacOS EXEC
    //========================================================================//
    // Determine the path of the executable
    char exec_path[MAX_FILENAME];
    uint32_t size = sizeof(exec_path);
    if (_NSGetExecutablePath(exec_path, &size) != 0) {
        fprintf(stderr, "Executable path is too long\n");
        return EXIT_FAILURE;
    }

    // Get the directory containing the executable
    char *exec_dir = dirname(exec_path);
    if (chdir(exec_dir) != 0) {
        perror("Failed to change working directory to executable's location");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Working directory set to: %s\n", exec_dir);
    //========================================================================//

    // Start the timer (This is used to time the entire process)
    clock_t start_time = clock();

    // First open the director the file is in, the directory stem is "." to indicate the current dir where this file is in.
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Failed to open directory");
        return EXIT_FAILURE;
    }

    // The while loop below will iterate through the entries of the directory stream. It takes a pointer of the dir
    // we had just opened. Each "entry" is a pointer to a directory entry struct called a dirent.

    // dirents hold information about a directory item such as:

    //ino_t d_ino;             // Inode number
    //off_t d_off;             // Offset to the next dirent
    //unsigned short d_reclen; // Length of this record
    //unsigned char d_type;    // Type of file (e.g., regular file, directory, etc.)
    //char d_name[256];        // Name of the file or directory

    int dir_created = 0;
    int files_tracked = 0;
    int submissions_late = 0;

    // We can initialize a pointer to a dirent called entry.
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip directories and hidden files. Continue will skip everything and resume the while, this is a guard statement.
        if (entry->d_type == DT_DIR || entry->d_name[0] == '.') {
            continue;
        } else {
            files_tracked += 1;
        }

        
        // d_name of the dirent is important because it contains the information needed to make our new subdirectories.
        char *filename = entry->d_name;

        // Find the first underscore in the filename. If you remember the format of the files that get downloaded by canvas
        // the _ symbol separates specific attributes of the file like the name, lateness, identifiers. We can skip anything
        // that doesn't have a _ in it, but if it does, create a pointer of where that FIRST underscore is. This tells us where
        // the end of the student's name is in the file.
        char *underscore_pos = strchr(filename, '_');
        if (underscore_pos == NULL) {
            fprintf(stderr, "Skipping file with no underscore: %s\n", filename);
            continue;
        }

        // Extract the student's name in this section:

        // The name length is just the difference in pointer positions. Filename is at the front while underscore_pos is at the
        // Underscore. Then declare that student_name is just a character array with that max size determined at the start.
        // Once it has been copied, we can put a null terminator on it to make it a proper string.
        size_t name_length = underscore_pos - filename;
        char student_name[MAX_FILENAME];
        strncpy(student_name, filename, name_length);
        student_name[name_length] = '\0';

        // Determine if this file has "LATE_" in this section:

        // The underscore pointer is already at the underscore, but about 5 positions away after 1 of the underscore
        // there could be a "LATE_" array of characters, if this is true meaning "LATE_" is there, then the student is
        // marked late.
        int is_late_file = (strncmp(underscore_pos + 1, "LATE_", 5) == 0);


        // Generate the student directory name in this section:

        // We have the student's name and wheter they are late. The directory name is initialized and then set up.
        // The student's name is first.
        // A ternary operator is used to put " [LATE]" on the directory name.
        // snprintf makes a new string with some string formatting using the student's name and lateness.
        char student_dir[MAX_FILENAME];
        snprintf(student_dir, sizeof(student_dir), "%s%s", student_name, is_late_file ? " [LATE]" : "");


        // Create the directory (only once per student). With everything set up for the dir name for a student, make a dir now.
        int result = create_directory(student_dir);
        if (result == 0) {
            fprintf(stdout, "Created directory for: %s\n", student_dir);
            dir_created += 1;
        }


        // Print progress message for the current student. These will appear in the terminal window to show which student
        // the program is working on currently. It may take several passes to get all the student's files.
        fprintf(stdout, "Processing files for student: %s\n", student_name);


        // We pretty much want to strip the name and lateness (if it is true)
        // from the current working file. This turns the name into:

        //"[name]_[LATE]_123456_12345678_[OriginalFilename][-1 redundancy suffix].[extension]" -> "123456_12345678_[OriginalFilename][-1 redundancy suffix].[extension]"

        char new_filename[MAX_FILENAME];
        if (is_late_file) {
            printf("Removed LATE identifier...\n");
            submissions_late += 1;
            snprintf(new_filename, sizeof(new_filename), "%s", underscore_pos + 6); // Skip "LATE_" and inherently the name because of the underscore pos.
        } else {
            snprintf(new_filename, sizeof(new_filename), "%s", underscore_pos + 1); // Move past the "_"
        }

        // Further trim the `123456_12345678_` portion (first 16 characters). This is always true but the numbers are
        // different for each file. Canvas uses these numbers to avoid redundant filenames. They are always 16 characters long.

        // "123456_12345678_[OriginalFilename][-1 redundancy suffix].[extension]" -> "[OriginalFilename][-1 redundancy suffix].[extension]"
        if (strlen(new_filename) > 16) {
            printf("Removing unique identifier number in: %s \n", new_filename);
            memmove(new_filename, new_filename + 16, strlen(new_filename) - 16 + 1);
        }


        // To handle if a file has been duplicated once or twice, removal of "-1" is required. [the original file name can include "."" and "-"] strrchr will ignore them.

        // puts a pointer where the file extension is:                         v
        //                                               [original file name]-1.[extension]
        const char *dot = strrchr(new_filename, '.');
        // puts a pointer to where the last hyphen is:                       v
        //                                               [original file name]-1.[extension]
        const char *hyphen = strrchr(new_filename, '-');

        // Check if "-1" is right before the extension. // (&& *(hyphen + 1) == '1') <-- Usually it is just a 1, I've checked the set and haven't found a 2,
        // if there is a 2 in the set, then this will not catch it, the fix is to remove the line this is pointing to. I don't want to accidentally remove part of the original filename.
        
        // "[OriginalFilename][-1 redundancy suffix].[extension]" -> "[OriginalFilename].[extension]"
        // If the original filename happens to have a hyphen in it, ignore this command.
        if (hyphen && (dot - hyphen == 2) && *(hyphen + 1) == '1') {
            printf("Found '-#' indicator before the extension in: %s\nRemoving...\n", new_filename);

            // Create a new filename excluding the "-1".
            size_t new_name_length = hyphen - new_filename; // Length up to the hyphen.
            snprintf(new_filename, sizeof(new_filename), "%.*s%s", (int)new_name_length, new_filename, dot);
        } else {
            // Keep the original filename if no "-1" is found.
            snprintf(new_filename, sizeof(new_filename), "%s", new_filename);
        }

        // Now new_filename has the char array of [OriginalFilename].[extension]
        // In the format we wanted.

        // Construct the source and destination paths. Basically to be able to move files within a directory, we need a source
        // and a destination. The source path is that original filename, which is a pointer to the untampered file.
        // Within code here we have done some char array maniputation to get two important parts. We got the new Dir name,
        // so for the destination we can put that as the first part in the path, and second we successfully trimmed up the
        // filename so that is where the file should land.
        char source_path[MAX_FILENAME];
        char destination_path[MAX_FILENAME];

        snprintf(source_path, sizeof(source_path), "./%s", filename);
        snprintf(destination_path, sizeof(destination_path), "./%s/%s", student_dir, new_filename);

        // Move the file to the student's directory and change up the name. If this is successful meaning the old file
        // exists in our current directory, the destination path is open, and the source path exists, we can move the file
        // to its new home.
        if (rename(source_path, destination_path) != 0) {
            perror("Failed to move file");
        } else {
            fprintf(stdout, "Identified Original File Name: %s \n", new_filename);
            fprintf(stdout, "Moving file...\n");
        }
    }
    
    // Once all done and dusted in the directory stream, we can close out the directory we were workining in by passing
    // the ponter to our current directory into closedir.
    closedir(dir);

    // End the timer and calculate elapsed time. This is useful for showing how much time elapsed while the program was running.
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print some goodies at the end:
    fprintf(stdout, "\n\nProcess Finished without error\n");
    fprintf(stdout, "Total processing time: \t\t\t%.2f [seconds]\n", elapsed_time);
    fprintf(stdout, "Average processing time per file: \t%.4f [milliseconds]\n", (elapsed_time/((double)files_tracked))*1000.0);
    fprintf(stdout, "Files tracked: \t\t\t\t%d [files]\n", files_tracked);
    fprintf(stdout, "Directories created: \t\t\t%d [locations]\n", dir_created);
    fprintf(stdout, "Submissions late: \t\t\t%d [submissions]\n", submissions_late);
    fprintf(stdout, "Thank you for using this workflow and hope I was able to help! - From, Nathan Fikes\n\n");
    
    // mains usually return 0 when done, but EXIT_SUCCESS is good here.
    return EXIT_SUCCESS;
}