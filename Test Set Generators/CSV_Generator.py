import random
import string
import csv

# List of names provided
names_list = [
    "Haaris Haas", "Shaun Mercado", "Arjan Tate", "Maria Carey", "Bryn Serrano",
    "Ariana Pineda", "Miranda Strong", "Cleo Martin", "Anas Sampson", "Hiba Salas",
    "Alexandra Ray", "Cameron Barnes", "Brett Cain", "Luke Terry", "Maisy Cannon",
    "Ethan Bowers", "Hamish Holden", "Dora Schmidt", "Adil Morales", "Sameer Santos",
    "Muhammed Merritt", "Shayla Patton", "Catherine Wyatt", "Alasdair Mitchell", "Hector Woodward",
    "Mohsin Lucas", "Caspar Thornton", "Roy Terrell", "Rebecca Doyle", "Mathilda Garrett",
    "Aayan Spencer", "Audrey Moody", "Rahul Logan", "Zain Hernandez", "Emil Carrillo",
    "Aliyah Gillespie", "Marilyn Douglas", "Seren Hunt", "Haydn Grant", "Gwen Small",
    "Saba Bond", "Maisie Woodard", "Oliwier Tucker", "Ezekiel Ellis", "Aryan Quinn",
    "Oakley Winters", "Jed Benjamin", "Dewey Huff", "Erica Higgins", "Safa Marshall",
    "Gabriel Lewis", "Nataniel James", "Dylan Kirby", "Edwin Barber", "Khadijah Banks",
    "Heath Barrera", "Abdirahman Chandler", "Rachel Weiss", "Xander Watson", "Arthur Bradshaw",
    "Eryn Stanton", "Lloyd Schultz", "Elle Bridges", "Enya Ashley", "Rafe Guerrero",
    "Nelson Fitzpatrick", "Mohammed Franco", "Fiona Lamb", "Antonio Rogers", "Fannie Fuller",
    "Sarah Velasquez", "Bruce Webb", "Leonard Glover", "Ethel Irwin", "Anika Tyler",
    "Leonie Jacobs", "Nikhil Lindsay", "Agnes Petty", "Renee Patrick", "Traci Hilton",
    "Eddie England", "Victoria Alexander", "Amelie Rios", "Sabrina Day", "Dexter Barton",
    "Herbie Obrien", "Evie Sweeney", "Tobias Bray", "Cassius Waters", "Junaid Dalton",
    "Sebastian Arroyo", "Shanice Berg", "Leila Johns", "Inayah O'Gallagher", "Tillie Stuart",
    "Naima Lambert", "Clementine Chavez", "Keeley Miller", "Tess York", "Edna Harvey",
    "Harun Cervantes", "Madiha Warren", "Zack Edwards", "Matilda Howe", "Mario York",
    "Abdur Tate", "Solomon Burke", "Billie Dickerson", "Antony Weeks", "Wanda Curry",
    "Camilla Sutton", "Yousef Harding", "Shauna Reilly", "Maximus O'Moore", "Danny Morales",
    "Patricia Harmon", "Eliot Bray", "Nabil Gutierrez", "Owen Guerrero", "Lance Morse",
    "Kaine Walker", "Hugh Buck", "Aleeza Graves", "Nikodem Cantu", "Shirley Horne",
    "Rihanna Bond", "Andy Hancock", "Callie Cole", "Rhianna Gilbert", "Stevie Hoffman",
    "Athena Deleon", "Ella-Louise Figueroa", "Alisa Higgins", "Habiba Mccormick", "Mackenzie Hood",
    "Marshall Keller", "Adelaide Woods", "Thea Hudson", "Elijah Jenkins", "Emily Mcdowell",
    "Armaan Ramos", "Nataniel Lawrence", "Diana Sharpe", "Ida Wade", "Carolyn Hubbard",
    "Alexia Berg", "Dulcie Giles", "Frederic Reid", "Nikhil Peters", "Malachi Austin",
    "Oliwia Lynn", "Fletcher Barton", "Kaitlyn Olson", "Nicole Bolton", "Nell Juarez",
    "Mae Burgess", "Minnie O'Reilly", "Cassandra Williamson", "Conrad Swanson", "Christopher Santiago",
    "Hashim Avila", "Aishah Wilkinson", "Keenan Acosta", "Alyssia Jacobs", "Bronte Rios",
    "Nancy Oconnell", "Ebony Delacruz", "Joanne Johns", "Giovanni Lowe", "Calvin Singh",
    "Katerina Whitney", "Fynn Lowery", "Abu Santana", "Dewi Harrell", "Alec Wiley",
    "Leonie Knowles", "Cassie Barlow", "Rosemary Frank", "Anton Sanders", "Carter Merritt",
    "Aron Dale", "Elinor Barnes", "Lawrence Copeland", "Verity Donovan", "Carlo Moyer",
    "Isabel Solis", "Melody Duncan", "Lennox Wolf", "Amaan Murphy", "Hamish Oconnor",
    "Amina Short", "Jeffrey Sanchez", "Ilyas Landry", "Cory Sandoval", "Edna Barr",
    "Lilly-Rose Hayes", "Isha Hopkins", "Katie Elliott", "Safiya Ford", "Mark Poole",
    "Tristan Summers", "Louise O'Doherty", "Joao Holland", "Mathew Orr", "Remi Holden",
    "Harold Daniel", "Xavier Horne", "Mae Bird", "Celine Blackburn", "Noel Romero",
    "Billy Carey", "Muhammad Meyer", "Faye Morgan", "Ethel Gallagher", "Hanna Wu",
    "Louis Crawford", "Cynthia Leon", "Damon Miranda", "Barry Wise", "Rhys Lindsey",
    "Sebastien Black", "Greta Schaefer", "Poppy Wheeler", "Laila Pineda", "Danyal Bradley",
    "Oliwier Montes", "Hiba Weaver", "Owain Fuller", "Armaan Barlow", "Lacie Sullivan",
    "Amira Bennett", "Sian Cisneros", "Ellie-May Poole", "Aurora Holder", "Saira Duffy",
    "Doris Clark", "Shelby Alvarado", "Jayson Wilcox", "Alfie Roberson", "Maariyah Baird",
    "Aimee Christian", "Tasneem Moody", "Margie Floyd", "Leila Pittman", "Abdul Bray",
    "Coral Odom", "Niamh Carlson", "Marjorie Petty", "Demi Benson", "Keyaan Booker",
    "Ross Jackson", "Harry Andrade", "Mikey Vance", "Mario Wagner", "Flynn Carver",
    "Ibraheem Lopez", "Nate Knox", "Cody Briggs", "Maxim Mooney", "Issac Leblanc",
    "Noah Owens", "Khalid Bass", "Aliya Lee", "Samara Anthony", "Caspar Duran",
    "Omer Odonnell", "Neve Arnold", "Will Golden", "Nelson Henry", "Harris Calderon",
    "Alesha Hatfield", "Fred Chapman", "Myles Garrison", "Liliana Finley", "Santiago Sharp",
    "Zahra Francis", "Rhianna Bowman", "Harvey Villarreal", "Virgil Melendez", "Keziah Valenzuela",
    "Jazmin Price", "Jazmine Banks", "Dewey Walter", "Kira Crane", "Abdullahi Frank",
    "Nadia Shepard", "Terry Frederick", "Saba Rios", "Freyja Archer", "Victoria Lam",
    "Cassie Peters", "Peggy Hawkins", "Eva Pena", "Carolyn Kaufman", "Ashley Watson",
    "Mohamad Williams", "Cassandra Mcclure", "Wojciech Farmer", "Hamzah Kent", "Jeremiah Adams"
]

# Function to generate filenames using the names from the provided list
def generate_filename_with_names():
    name = random.choice(names_list)  # Randomly choose a name from the list
    late = random.choice([True, False])  # Randomly choose whether to include 'LATE_'
    num1 = random.randint(100000, 999999)  # 6 digit number
    num2 = random.randint(10000000, 99999999)  # 8 digit number
    filename = ''.join(random.choices(string.ascii_letters, k=random.randint(1, 40)))  # Random filename length between 3 to 10 characters
    include_number = random.choice([True, False])  # Randomly choose whether to include '-1'
    
    # Construct the filename
    filename_str = f"{name.replace(' ', '')}_"
    if late:
        filename_str += "LATE_"
    filename_str += f"{num1}_{num2}_{filename}"
    if include_number:
        filename_str += "-1"
    filename_str += ".txt"
    
    return filename_str

# Generate a number of filenames with the new names list
filenames_with_names = [generate_filename_with_names() for _ in range(10000)]

# Save to CSV
csv_file_with_names = 'Desktop/Test_Set.csv'
with open(csv_file_with_names, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Filename"])  # Header
    for filename in filenames_with_names:
        writer.writerow([filename])

csv_file_with_names  # Return the
