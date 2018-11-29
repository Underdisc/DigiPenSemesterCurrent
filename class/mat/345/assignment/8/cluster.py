import numpy as np
from openpyxl import load_workbook

wb = load_workbook('data.xlsx')
data_ws = wb["Data"]

# Find the minimum value in an array.
def min_index(V):
  min = V[0]
  min_index = 0
  current_index = 0
  for v in V :
    if(v < min):
      min = v
      min_index = current_index
    current_index += 1
  return min_index

# C contains our current centroids.
def new_centroids(C, P) :
  # Find the distances to each centroid.
  D = np.zeros((data_points, total_centroids))
  cluster_sum = np.zeros((total_centroids, data_dimension))
  cluster_size = np.zeros((total_centroids))
  point = 0
  for p in P :
    centroid = 0
    for c in C :
      delta = p - c
      D[point][centroid] = delta.dot(delta)
      centroid += 1
    # Find the closest centroid.
    centroid_index = min_index(D[point])
    cluster_sum[centroid_index] += p
    cluster_size[centroid_index] += 1
    point += 1

  print("Squared Distances")
  print(D)
  print("Cluster Sizes")
  print(cluster_size)

  # Find new centroids
  current_cluster = 0
  for cs in cluster_sum  :
    C[current_cluster] = cs / cluster_size[current_cluster]
    current_cluster += 1
  return C

# The number of data points and dimension we are working with.
data_points = 10
data_dimension = 3
# Read in all of the points.
P = np.zeros((data_points, data_dimension))
data_range = data_ws['A1' : 'C10']
k = 0
for x1, x2, x3 in data_range:
  P[k] = np.array([x1.value, x2.value, x3.value])
  k += 1

# Set the starting centroids.
total_centroids = 3
C = np.zeros((total_centroids, data_dimension))
C[0] = np.array([0, 0, 0])
C[1] = np.array([3, 3, 2])
C[2] = np.array([2, 0, 0])

# Continue to compute new centroids.
print("Centroids")
print(C)
for r in range(0, 2) :
  print_list = ["Run", r]
  print(print_list)
  C = new_centroids(C, P)
  print("Centroids")
  print(C)

