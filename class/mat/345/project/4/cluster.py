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
    
    def PointDimension():
        return self.point.size

# Stores the centroids of a clustered set and classified the given
# points into their starting clusters.
class ClusterSet:
    def __init__(self, points, centroids):
        self.centroids = centroids
        self.data_points = []
        for p in points:
            self.data_points.append(DataPoint(p, centroids))

    def UpdateCentroids(self):
        cluster_sums = np.zeros((self.centroids))
        for dp in self.data_points:


