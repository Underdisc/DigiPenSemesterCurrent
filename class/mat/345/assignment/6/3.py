e = 2.718281828459045 

class Vector:
    values = []

    def __init__(self, values):
        self.values = values

    def __add__(self, other):
        new_values = []
        for i in range(0, len(self.values)):
            new_values.append(self.values[i] + other.values[i])
        return Vector(new_values)

    def __sub__(self, other):
        new_values = []
        for i in range(0, len(self.values)):
            new_values.append(self.values[i] - other.values[i])
        return Vector(new_values)

    def __mul__(self, scaler):
        new_values = []
        for i in range(0, len(self.values)):
            new_values.append(self.values[i] * scaler)
        return Vector(new_values)
    
    def __div__(self, scaler):
        new_values = []
        for i in range(0, len(self.values)):
            new_values.append(self.values[i] / scaler)
        return Vector(new_values)

    def dot(self, other):
        value = 0;
        for i in range(0, len(self.values)):
            value += self.values[i] * other.values[i]
        return value

    def Print(self):
        print(self.values)


def g(y_l, x_l_v, w_v):
    g_vals = []
    for i in range(0, len(x_l_v[0].values)):
        g_vals.append(0)
    g_v = Vector(g_vals)
    for k in range(0, len(y_l)):
        numer = x_l_v[k] * y_l[k]
        expo = y_l[k] * w_v.dot(x_l_v[k])
        denom = 1 + pow(e, expo)
        result = numer / denom 
        g_v = g_v + result
    g_v = g_v / -len(y_l)
    return g_v

y_l = [-1, -1 ,1, -1, -1, 1, 1, 1, -1, 1, 1, -1, -1, 1, -1, -1, 1, -1, 1, -1]
x_l_v = [Vector([1, 32, 3]),
         Vector([1, 45, 2]),
         Vector([1, 60, 2]),
         Vector([1, 53, 1]),
         Vector([1, 25, 4]),
         Vector([1, 68, 1]),
         Vector([1, 82, 2]),
         Vector([1, 38, 5]),
         Vector([1, 67, 2]),
         Vector([1, 92, 2]),
         Vector([1, 72, 3]),
         Vector([1, 21, 5]),
         Vector([1, 26, 3]),
         Vector([1, 40, 4]),
         Vector([1, 33, 3]),
         Vector([1, 45, 1]),
         Vector([1, 61, 2]),
         Vector([1, 16, 3]),
         Vector([1, 18, 4]),
         Vector([1, 22, 6])]
w_v = Vector([0, 0, 0])
g_v = Vector([0, 0, 0])

n = 0.004 
for iteration in range(1, 50000):
    g_v = g(y_l, x_l_v, w_v)
    print(g_v.values)
    w_v = w_v - g_v * n







