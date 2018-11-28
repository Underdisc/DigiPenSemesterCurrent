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
x_l_v = [Vector([1, 3.2, 3]),
         Vector([1, 4.5, 2]),
         Vector([1, 6.0, 2]),
         Vector([1, 5.3, 1]),
         Vector([1, 2.5, 4]),
         Vector([1, 6.8, 1]),
         Vector([1, 8.2, 2]),
         Vector([1, 3.8, 5]),
         Vector([1, 6.7, 2]),
         Vector([1, 9.2, 2]),
         Vector([1, 7.2, 3]),
         Vector([1, 2.1, 5]),
         Vector([1, 2.6, 3]),
         Vector([1, 4.0, 4]),
         Vector([1, 3.3, 3]),
         Vector([1, 4.5, 1]),
         Vector([1, 6.1, 2]),
         Vector([1, 1.6, 3]),
         Vector([1, 1.8, 4]),
         Vector([1, 2.2, 6])]
w_v = Vector([0, 0, 0])
g_v = Vector([0, 0, 0])

n = 0.1 
for iteration in range(1, 50000):
    g_v = g(y_l, x_l_v, w_v)
    w_v = w_v - g_v * n
print(w_v.values)






