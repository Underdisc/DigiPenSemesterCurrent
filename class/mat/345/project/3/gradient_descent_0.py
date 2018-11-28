# This file is for calculating the weight vector using the gradient descent
# algorithm.

import math
import numpy as np
from openpyxl import load_workbook

# Open the Excel workbook and get a reference to the training worksheet.
wb = load_workbook('data.xlsx')
training_ws = wb["Training"]

# The number of data points and the dimenson of each data point.
data_points = 331
data_dimension = 4
# X will be our input data matrix.
# Y will be our result data.
X = np.zeros((data_points, data_dimension))
Y = np.zeros((data_points))

# Read all of the input data into X and all output values into Y.
input_range = training_ws['A2':'D332']
data_point = 0
for midterm, homework, quiz, course_grade in input_range:
  x1 = midterm.value / 100.0
  x2 = homework.value / 100.0
  x3 = quiz.value / 100.0
  X[data_point] = np.array([1.0, x1, x2, x3])
  # The output value is set to 1 or -1 depending on the initial value.
  if(course_grade.value >= 70.0) :
    Y[data_point] = 1.0
  else :
    Y[data_point] = -1.0
  data_point += 1

# Calculate the gradient given the input data, the output, and the current
# weight vector.
e = 2.718281828459045 
def gradient(X, Y, w) :
  data_points = Y.size
  data_size = X[0].size
  g = np.zeros((data_size))
  for k in range(0, data_points) :
    numer = Y[k] * X[k]
    expo = Y[k] * w.dot(X[k])
    denom = 1.0 + pow(e, expo)
    g += numer / denom
  g = -g / data_points
  return g

# Calculate the magnitude of a N dimensional vector.
def magnitude(v) :
  result = 0.0
  for i in range(0, v.size) :
    result += v[i] * v[i]
  return math.sqrt(result)

# Perform the gradient descent algorithm.
data_size = X[0].size
w = np.zeros((data_size))
g = np.ones((data_size))
n = 0.05
epsilon = 0.001
print_counter = 0
iteration = 0
while(magnitude(g) > epsilon) :
  g = gradient(X, Y, w)
  w -= n * g
  if(print_counter >= 1000) :
    print(iteration)
    print(w)
    print(g)
    print_counter = 0
  print_counter += 1
  iteration += 1
print("done")
print(w)
print(g)