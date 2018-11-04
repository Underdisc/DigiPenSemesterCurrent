
def diff(x, y):
    r = []
    r.append(x[0] - y[0])
    r.append(x[1] - y[1])
    return r

def mul(n, i):
    r = []
    r.append(n * i[0])
    r.append(n * i[1])
    return r

def grad_f(i):
    r = []
    r.append(2 * i[0] - 2 * i[1] * i[1])
    r.append(4 * i[0] * i[1] + 4 * i[1] * i[1] * i[1])
    return r

n = 0.05
i = (0, 3)
for iteration in range(1, 100):
    i = diff(i, mul(n, grad_f(i)))
    print_list = []
    print_list.append(iteration)
    print_list.append(i)
print(i)
