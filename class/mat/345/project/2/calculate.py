# Author: Connor Deakin
# Class: MAT 345
# Assignment: Project 2
# Email: connor.deakin@digipen.edu 

import os
import re

def extract_subject(file_path):
    file_stream = open(file_path, "r", errors='ignore') 
    content = file_stream.readlines()
    for line in content:
        find_result = line.find("Subject:", 0, 8)
        if(find_result >= 0):
            return line[9:]
    return ""

def extract_words_lower(string):
    regex = r'[a-zA-Z]+'
    words = re.findall(regex, string)
    for i in range(0, len(words)):
        words[i] = words[i].lower()
    return words

def build_word_set(word_set, new_words):
    for word in new_words:
        word_set.add(word)

def contains(search_word, v_words):
    for word in v_words:
        if(search_word == word):
            return True
    return False

def subjects_containing_word(search_word, l_subjects):
    count = 0
    for subject in l_subjects:
        if(contains(search_word, subject)):
            count += 1
    return count

def p_words_given_type(words, l_subjects):
    p_words_given_subjects = []
    for word in words:
        word_occurrences = subjects_containing_word(word, l_subjects)
        prob = (1.0 + word_occurrences) / (2.0 + len(l_subjects))
        p_words_given_subjects.append(prob)
    return p_words_given_subjects

def p_type_g_word(p_type, p_word_g_type, p_word_g_not_type):
    numerator = p_word_g_type * p_type
    denominator = numerator + (p_word_g_not_type * (1.0 - p_type))
    return numerator / denominator

def find_largest(count, values):
    largest_indicies = []
    og_values = []

    # Find the largest values.
    for c in range(0, count):
        largest_index = 0
        for i in range(1, len(values)):
            value = values[i]
            largest_value = values[largest_index]
            if(value > largest_value):
                largest_index = i
        largest_indicies.append(largest_index)
        og_values.append(values[largest_index])
        values[largest_index] = 0

    # reset the values that have been set to zero.
    for i in range(0, count):
        index = largest_indicies[i]
        value = og_values[i]
        values[index] = value

    return largest_indicies

def full_dirs(parent_dir):
    email_dirs = ["easy_ham", "hard_ham", "spam"]
    full_dirs = []
    for email_dir in email_dirs:
        full_dir = parent_dir + "/" + email_dir
        full_dirs.append(full_dir)
    return full_dirs

def get_subjects(dir):
    subjects = []
    for file in os.listdir(dir):
        file_path = dir + "/" + file
        subject_line = extract_subject(file_path)
        subject_words = extract_words_lower(subject_line)
        subjects.append(subject_words)
    return subjects

def get_ham_subjects(full_dirs):
    subjects = []
    for i in range(0, 2):
        full_dir = full_dirs[i]
        subjects.extend(get_subjects(full_dir))
    return subjects

def get_spam_subjects(full_dirs):
    subjects = get_subjects(full_dirs[2])
    return subjects

# Create all of the full directory names that emails are be located in.
full_training_dirs = full_dirs("training")

# Get all of the subject lines from the email files.
training_ham_subjects = get_ham_subjects(full_training_dirs)
training_spam_subjects = get_spam_subjects(full_training_dirs)

# Create a set of all of the existing words.
word_set = set()
for l_words in training_ham_subjects:
    build_word_set(word_set, l_words)
for l_words in training_spam_subjects:
    build_word_set(word_set, l_words)

# Put the word set into a list to maintain order.
word_list = []
for word in word_set:
    word_list.append(word)

# Calculate probablity of words appearing given the email is spam or ham.
p_words_g_ham = p_words_given_type(word_list, training_ham_subjects)
p_words_g_spam = p_words_given_type(word_list, training_spam_subjects)

# Calculate the probability of spam and ham emails given the sample size.
ham_count = len(training_ham_subjects)
spam_count = len(training_spam_subjects)
total_count = ham_count + spam_count
p_ham = ham_count / float(total_count)
p_spam = spam_count / float(total_count)

# Calculate the probability of an email being spam or ham depending on the word
# that appeared in the email.
p_ham_g_words = []
p_spam_g_words = []
for i in range(0, len(word_list)):
    result = p_type_g_word(p_ham, p_words_g_ham[i], p_words_g_spam[i])
    p_ham_g_words.append(result)
for i in range(0, len(word_list)):
    result = p_type_g_word(p_spam, p_words_g_spam[i], p_words_g_ham[i])
    p_spam_g_words.append(result)

# Find the hammies and spammiest words.
hammiest_words = find_largest(5, p_ham_g_words)
spammiest_words = find_largest(5, p_spam_g_words)

# Print training results.
print("Hammiest Words")
for index in hammiest_words:
    print_list = []
    print_list.append(word_list[index])
    print_list.append(p_words_g_ham[index])
    print_list.append(p_words_g_spam[index])
    print_list.append(p_ham_g_words[index])
    print_list.append(p_spam_g_words[index])
    print(print_list)
print("\nSpammiest Words")
for index in spammiest_words:
    print_list = []
    print_list.append(word_list[index])
    print_list.append(p_words_g_ham[index])
    print_list.append(p_words_g_spam[index])
    print_list.append(p_ham_g_words[index])
    print_list.append(p_spam_g_words[index])
    print(print_list)

def existing_words_in_subject(word_list, subject):
    existing_words = []
    for word in word_list:
        word_exists = 0
        for element in subject:
            if(word == element):
                word_exists = 1
        existing_words.append(word_exists)
    return existing_words

def probability_of_being_spam(existing_words, p_words_g_ham, p_words_g_spam, p_spam):
    p_existing_words_g_spam = 1
    p_existing_words_g_ham = 1
    for i in range(0, len(existing_words)):
        word_exists = existing_words[i]
        pwgs = p_words_g_spam[i]
        pwgh = p_words_g_ham[i]
        if(word_exists == 1):
            p_existing_words_g_spam *= pwgs
            p_existing_words_g_ham *= pwgh
        else:
            p_existing_words_g_spam *= (1.0 - pwgs)
            p_existing_words_g_ham *= (1.0 - pwgh)
    numerator = p_existing_words_g_spam * p_spam
    denominator = numerator + p_existing_words_g_ham * (1.0 - p_spam)
    result = numerator / denominator
    return result

def is_spam(word_list, subject, p_words_g_ham, p_words_g_spam, p_spam):
    existing_words = existing_words_in_subject(word_list, subject)
    p = probability_of_being_spam(existing_words, p_words_g_ham, p_words_g_spam, p_spam)
    if(p > 0.5):
        return 1
    return 0

def is_spam_from_subjects(subjects, word_list, p_words_g_ham, p_words_g_spam, p_spam):
    spam_emails = []
    for subject in subjects:
        spam_emails.append(is_spam(word_list, subject, p_words_g_ham, p_words_g_spam, p_spam))
    return spam_emails

# Get all of the testing subjects.
full_testing_dirs = full_dirs("testing")
testing_ham_subjects = get_ham_subjects(full_testing_dirs)
testing_spam_subjects = get_spam_subjects(full_testing_dirs)

# Predict whether the subjects are spam or not.
is_s_from_h = is_spam_from_subjects(testing_ham_subjects, word_list, p_words_g_ham, p_words_g_spam, p_spam)
is_s_from_s = is_spam_from_subjects(testing_spam_subjects, word_list, p_words_g_ham, p_words_g_spam, p_spam)

# Compute accuracy, precision, and recall from results.
emails_predicted_as_spam = 0
correctly_predicted_ham = 0
correctly_predicted_spam = 0
for value in is_s_from_h:
    emails_predicted_as_spam += value
    if(value == 0):
        correctly_predicted_ham += 1
for value in is_s_from_s:
    emails_predicted_as_spam += value
    if(value == 1):
        correctly_predicted_spam += 1
correctly_predicted_emails = correctly_predicted_ham + correctly_predicted_spam
total_emails = len(is_s_from_h) + len(is_s_from_s)
accuracy = correctly_predicted_emails / total_emails
precision = correctly_predicted_spam / emails_predicted_as_spam
recall = correctly_predicted_spam / len(is_s_from_s)

# Print accuracy, precision, and recall.
print("\nAccuracy: ", accuracy)
print("Precision: ", precision)
print("Recall: ", recall)






