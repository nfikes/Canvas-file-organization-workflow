   Canvas is an academic architecture for manipulating grades, posting files, hosting discussions, facilitating tests, etc. When it comes time to grade, it is useful to have all the files related to a specific student organized into subdirectories. When downloading all submissions, Canvas puts them all into one zip file. Additionally, Canvas attaches a filename prefix and suffix, which obscures the original filename causing issues:

Issue 1: (CAD systems)

CAD systems that rely on a hierarchy of file names loose that capability as the prefixes and suffixes change the overall file name. This system aims to clean up the filenames as it is sorting to allow these systems to function properly.

Issue 2: (Volume)

Canvas Download provides a large folder with all the student's uploaded files. Searching for these files can be tedious and impede the grading workflow, especially if multiple files need to be observed. The subdirectories created by this system allow a grader to open up a student's folder, and all related files will be stored in one place.

Issue 3: (Tracking)

Some students submit items late. If there is a grade penalty, it would be good to easily identify which students this rule applies to. Each subdirectory will inherit information from the added suffix to the filename to ensure file submission times are tracked properly.

Issue 4: (Speed)

If files do need to be renamed for any reason, an edit to the original filename is much faster than deleting the suffix and prefix attachments.

***

How do you use this system?

1. To effectively use this system, an EXE or EXEC can be downloaded from the macOS or WindowsOS (WIP) folders in this repository.
   
2. Next download your submissions from Canvas, they should arrive in a zip file, extract these to a folder you want to operate in.
   
3. Drop either the EXE (Windows) or EXEC (MAC) into the directory with all the files. Make sure this file is in the same directory as all those files. Make sure this directory is named 'submissions' to ensure the EXE or EXEC will work.
   
4. Double-click the EXE/EXEC and a Terminal window will automatically pop up. Once finished, close the terminal window, and return to your file location.

4.5. IMPORTANT NOTE: MacOS EXEC downloaded from the internet are quarantined, to remove the quarantine status from the EXEC,  cd to your exec location, run ```xattr -d com.apple.quarantine ./submission-organizer-macOS``` in Terminal before running the program.
   
5. All the files (Ones that fit the naming scheme) are now in subdirectories, and their original file names are preserved.

Terminal approach:

1. Install GCC onto your machine. MAC and Windows have different methods. Find the most suitable one.

2. Download the submissions as a zip and extract to a folder.

3. Depending on the system, download the "submission-organizer-macOS.c" or "submission-organizer-WinOS.c" C files.

4. Move these files from the downloads to the location where all the submission files are. 

5. Move the terminal head to the file locations using cd, and use ls to check if the location is correct. Build the executable using the GCC command:
    
    Windows:    ```gcc -o submission-organizer-WinOS.exe submission-organizer-WinOS.c```
    
    Mac:        ```gcc -o submission-organizer-macOS submission-organizer-macOS.c```

6. Invoke the generated executables using this terminal command:
    
    Windows:    ```./submission-organizer-WinOS.exe```
    
    Mac:        ```./submission-organizer-macOS```

7. The same effect as using the executables will occur.

***
Example use:

![](https://github.com/nfikes/Canvas-file-organization-workflow/blob/main/macOS-Demo.gif)

***

Safety features and information:

- These programs are built to ignore Directories "/example/" and Hidden files ".example.txt" and if accidentally run in an unwanted place will leave these entities alone.
  
- Files that are tampered with by these programs are ONLY renamed and moved one directory level down. No files are deleted or added by this process!
  
- Files that are not Directories or Hidden files that also do not fit the Canvas naming scheme will not be moved.
  
- Everything here is open source so feel free to double-check before running anything.

- All file renaming follows this format: 

"\[name]\_\[LATE]\_123456\_12345678\_\[OriginalFilename]\[-1 redundancy suffix].\[extension]" -> "\[OriginalFilename].\[extension]"

- All directory creations follow this format: 

"./\[name] \[LATE]/"

***

- Last note: Although this program is fast, if run on more than 3000 files, the UI for the folder system may take some extra time to render so be patient. When the program is done the directory has already been reorganized even though it may not look like it.

If you have any questions feel free to contact me at:   nathan@fikesfarm.com
