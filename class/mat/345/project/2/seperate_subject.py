import os
import re

def extract_subject(file_path):
    file_stream = open(file_path, "r") 
    content = file_stream.readlines()
    counter = 0
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
            count += 1;
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


# Create all of the full directory names that emails are and will be located in.
training_dir = "training"
email_dirs = ["easy_ham", "hard_ham", "spam"]
full_training_dirs = []
for email_dir in email_dirs:
    full_training_dir = training_dir + "/" + email_dir
    full_training_dirs.append(full_training_dir)

# Get all of the subject lines from the email files.
ham_subjects = []
spam_subjects = []
for i in range(0, 3):
    full_dir = full_training_dirs[i]
    for file in os.listdir(full_dir):
        file_path = full_dir + "/" + file
        subject_line = extract_subject(file_path)
        subject_words = extract_words_lower(subject_line)
        if(i == 2):
            spam_subjects.append(subject_words)
        else:
            ham_subjects.append(subject_words)

# Create a set of all of the existing words.
word_set = set()
for l_words in ham_subjects:
    build_word_set(word_set, l_words)
for l_words in spam_subjects:
    build_word_set(word_set, l_words)

# Put the word set into a list to maintain order.
word_list = []
for word in word_set:
    word_list.append(word)

# Calculate probablity of words appearing given the email is spam or ham.
p_words_g_ham = p_words_given_type(word_list, ham_subjects)
p_words_g_spam = p_words_given_type(word_list, spam_subjects)

# Calculate the probability of spam and ham emails given the sample size.
ham_count = len(ham_subjects)
spam_count = len(spam_subjects)
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

# Print trainging results.
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

