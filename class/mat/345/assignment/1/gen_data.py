import random

class Point:
    x = 0
    y = 0

    def randomize(self, minimum, maximum):
        self.x = random.randint(minimum, maximum)
        self.y = random.randint(minimum, maximum)

points_l = []
fh = open("gen_data.txt", "w")

i = 0
num_points = 20
while i < num_points:
    np = Point()
    np.randomize(-10, 10)
    
    result = 2 * np.x + 3 * np.y
    output = 0;
    if(result > 1):
        output = 1
    elif(result < 1):
        output = -1

    if(output != 0):
        fh.write(str(np.x) + " " +  str(np.y) + " " + str(output) + "\n")
        i += 1

