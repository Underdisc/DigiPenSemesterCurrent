import matplotlib.image as mpimg
import numpy as np
from random import randint

def FindRandomMinimum(values):
    minimum = values[0]
    minimum_indicies = [0]
    for i in range(1, len(values)):
        v = values[i]
        if(v < minimum):
            minimum = v
            minimum_indicies.clear()
            minimum_indicies.append(i)
        elif(v == minimum):
            minimum_indicies.append(i)
    random_index = randint(0, len(minimum_indicies) - 1)
    minimum_index = minimum_indicies[random_index]
    return minimum_index

def FindMinimum(values):
    minimum = values[0]
    minimum_index = 0
    for i in range(1, len(values)):
        v = values[i]
        if(v < minimum):
            minimum = v
            minimum_index = i
    return minimum_index

# DataPoint will hold the actual point data and the cluster
# the point is in.
class DataPoint:
    # Members
    # point
    # cluster

    def __init__(self, point, centroids):
        self.point = point
        self.ClassifyPoint(centroids)

    def ClassifyPoint(self, centroids):
        p = self.point
        distances = []
        for c in centroids:
            d = c - p
            distances.append(d.dot(d))
        self.cluster = FindMinimum(distances)
        return self.cluster
    
    def PointDimension(self):
        return self.point.size

# Stores the centroids of a clustered set and classified the given
# points into their starting clusters.
class ClusterSet:
    # Members
    # centroids
    # data_points

    def __init__(self, points, centroids):
        self.centroids = centroids
        self.data_points = []
        for p in points:
            self.data_points.append(DataPoint(p, centroids))

    def UpdateCentroids(self):
        num_centroids = len(self.centroids)
        data_dimension = self.data_points[0].point.size
        # Find the sum of all points in each cluster and the number of points
        # in each cluster.
        cluster_sums = np.zeros((num_centroids, data_dimension))
        cluster_counts = np.zeros((num_centroids))
        for dp in self.data_points:
            cluster_sums[dp.cluster] += dp.point
            cluster_counts[dp.cluster] += 1
        # Find the new centroids
        for i in range(0, num_centroids):
            self.centroids[i] = cluster_sums[i] / cluster_counts[i]
        # Reclassify the points with the new centroids.
        num_data_points = len(self.data_points)
        previous_clusters = []
        new_clusters = []
        for i in range(0, num_data_points):
            previous_clusters.append(self.data_points[i].cluster)
            self.data_points[i].ClassifyPoint(self.centroids)
            new_clusters.append(self.data_points[i].cluster)
        # If the previous cluster classifications are equivalent to the
        # new classifications, return false since our centroids did not change.
        if(previous_clusters == new_clusters) :
            return False
        return True

def PickCentroidsAndOutputName(k):
    centroids = []
    centroids.append(np.array([1.0, 0.0, 0.0]))
    centroids.append(np.array([0.0, 1.0, 0.0]))
    centroids.append(np.array([0.0, 0.0, 1.0]))
    if(k >= 4):
        centroids.append(np.array([1.0, 1.0, 0.0]))
    if(k >= 5):
        centroids.append(np.array([1.0, 0.0, 1.0]))
    if(k >= 6):
        centroids.append(np.array([0.0, 1.0, 1.0]))
    if(k >= 7):
        centroids.append(np.array([1.0, 1.0, 1.0]))
    if(k >= 8):
        centroids.append(np.array([1.0, 0.5, 0.0]))
    if(k >= 9):
        centroids.append(np.array([1.0, 0.0, 0.5]))
    if(k >= 10):
        centroids.append(np.array([0.5, 1.0, 0.0]))
    output_file = "output_k_" + str(k) + ".png"
    centroids_filename = [centroids, output_file]
    return centroids_filename


def PerformKMeanClustering(points, k):
    centroids_filename = PickCentroidsAndOutputName(k)
    centroids = centroids_filename[0]
    output_file = centroids_filename[1]
    # Peform the k means algorithm until all centroids have been found.
    cluster_set = ClusterSet(points, centroids)
    iterations = 0
    while(cluster_set.UpdateCentroids()):
        iterations += 1
        print_list = ["Iteration", iterations]
        print(print_list)

    # Change all of the pixels image. The pixel color will be changed to the
    # pixel's associated centroid (the cluster the pixel is in).
    current_data_point = 0
    for row in range(0, image.shape[0]):
        for col in range(0, image.shape[1]):
            cluster_index = cluster_set.data_points[current_data_point].cluster
            new_color = cluster_set.centroids[cluster_index]
            image[row][col] = new_color
            current_data_point += 1

    # Write the image to an output file.
    mpimg.imsave(output_file, image)


# Read in the image file and put all of the pixels into a list.
image = mpimg.imread('urban_night.png')
points = []
for row in image:
    for pixel in row:
        new_point = np.array([pixel[0], pixel[1], pixel[2]])
        points.append(new_point)
# Perform K mean clustering for 3 <= k <= 10
for k in range(3, 11):
    print_list = ["K", k]
    print(print_list)
    PerformKMeanClustering(points, k)




