
def parse_line(line):
    value_list = []
    value_string = ""
    for char in line:
        if(char == " "):
            value_list.append(int(value_string))
            value_string = ""
        else:
            value_string += char
    value_list.append(int(value_string))
    return value_list


def read_value_lists(filename):
    value_lists = []
    file_stream = open(filename, "r")
    for line in file_stream:
        value_lists.append(parse_line(line))
    return value_lists

def sign(value):
    if value == 0:
        return 0
    if value > 0:
        return 1
    return -1

def dot(a_v, b_v):
    result = 0
    for i in range(len(a_v)):
        result += a_v[i] * b_v[i]
    return result

def mul(a, b_v):
    result = []
    for i in b_v:
        result.append(a * i)
    return result

def add(a_v, b_v):
    result = []
    for i in range(len(a_v)):
        result.append(a_v[i] + b_v[i])
    return result

def test(weights_v, i_v, o):
    result = dot(weights_v, i_v)
    result = sign(result)
    if(result == o):
        return True
    return False

def update(weights_v, i_v, o):
    weights_v = add(weights, mul(o, i_v)) 
    return weights_v

value_lists = read_value_lists("gen_data.txt")

weights = []
for i in range(len(value_lists[0])):
    weights.append(0)

input_lists = []
output_list = []
for value_list in value_lists:
    input_list = [1]
    num_inputs = len(value_list) - 1
    for i in range(num_inputs):
        input_list.append(value_list[i])
    input_lists.append(input_list)
    output_list.append(value_list[num_inputs])

current_test = 0
weight_changes = 0
print(weights)
while current_test < len(input_lists):
    current_input = input_lists[current_test]
    current_output = output_list[current_test]
    if(test(weights, current_input, current_output) == True):
        current_test += 1
        continue
    weights = update(weights, current_input, current_output)
    weight_changes += 1;
    print(weights)
    current_test = 0

print(weight_changes)
