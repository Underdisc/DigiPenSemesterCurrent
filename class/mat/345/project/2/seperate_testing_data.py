import os

# Create all of the full directory names that emails are and will be located in.
training_dir = "training"
testing_dir = "testing"
email_dirs = ["easy_ham", "hard_ham", "spam"]
full_training_dirs = []
full_testing_dirs = []
for email_dir in email_dirs:
    full_training_dir = training_dir + "/" + email_dir
    full_training_dirs.append(full_training_dir)
    full_testing_dir = testing_dir + "/" + email_dir
    full_testing_dirs.append(full_testing_dir)

# Move every fourth email into the testing data set.
email_counter = 0
for i in range(0, 3):
    full_training_dir = full_training_dirs[i]
    full_testing_dir = full_testing_dirs[i]
    for file in os.listdir(full_training_dir):
        email_counter += 1
        if(email_counter == 4):
            og_file_path = full_training_dir + "/" + file
            new_file_path = full_testing_dir + "/" + file
            os.rename(og_file_path, new_file_path)
            email_counter = 0


