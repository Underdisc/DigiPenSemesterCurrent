# This file is for calculating the probability of students passing the class.
import numpy as np
from openpyxl import load_workbook

# Using the w vector, find the probability that the ouput is 1 (the student
# passes the class with their known grades).
e = 2.718281828459045 
def probability(y, x, w) :
  expo = y * w.dot(x)
  numer = pow(e, expo)
  return numer / (1.0 + numer)

# Get a reference to the predict worksheet.
wb = load_workbook('data.xlsx')
predict_ws = wb["Predict"]

input_range = predict_ws['A2':'C55']
# This w vector was calculated in gradient_descent_0.py
w = np.array([-14.60362618, 7.61206173, 5.2962236, 9.66512732])
for midterm, homework, quiz in input_range:
  x1 = midterm.value / 100.0
  x2 = homework.value / 100.0
  x3 = quiz.value / 100.0
  x = np.array([1.0, x1, x2, x3])
  print(probability(1, x, w))
