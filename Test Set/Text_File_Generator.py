import os
import csv

# Get the current directory where the script is stored
output_folder = os.getcwd()

# Specify the correct CSV file name or path
csv_file_path = output_folder + '/Desktop/Nathan TA Organizer Program/Test Set/Test_Set.csv'  # Ensure this matches the actual CSV filename


correct_folder = output_folder + '/Desktop/Nathan TA Organizer Program/Test Set/'
print(csv_file_path)


# Check if the file exists to avoid runtime errors
if not os.path.exists(csv_file_path):
    raise FileNotFoundError(f"CSV file '{csv_file_path}' not found in the current directory.")


with open(csv_file_path, mode='r') as file:
    reader = csv.reader(file)
    next(reader)  # Skip header
    for row in reader:
        filename = row[0]
        # Create the blank text file in the current directory
        with open(os.path.join(correct_folder + 'Set/', filename), 'w') as f:
            pass  # Create an empty file
