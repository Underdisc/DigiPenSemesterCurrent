import numpy as np
from numpy.linalg import inv
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
  X[data_point] = np.array([1.0, midterm.value, homework.value, quiz.value])
  Y[data_point] = course_grade.value
  data_point += 1

# Compute the A matrix.
A = X.T @ X
A = inv(A) @ X.T

# Compute the weight vector using A and Y.
w = A @ Y
print(w)

# Get a reference to the predict worksheet.
predict_ws = wb["Predict"]

# Using the w vector, predict the outputs with the input data in the
# predict sheet.
input_range = predict_ws['A2':'C55']
for midterm, homework, quiz in input_range:
  x = np.array([1.0, midterm.value, homework.value, quiz.value])
  y = w.dot(x)
  print(y)



