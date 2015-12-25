import urllib
import time
import random

start_time = time.time()
for i in range(1000):
    x = int(random.random() * 400000)
    y = int(random.random() * 400000)
    urllib.urlopen("http://localhost:3000/testShortest?a=%d&b=%d" % (x, y))

end_time = time.time()

print end_time - start_time

