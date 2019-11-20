#!/usr/bin/python3
import sys
if len(sys.argv) < 2:
    sys.exit("enter filename as argv[1]")

pts = []
avg = 0
with open(sys.argv[1], "r") as f:
    for s in f:
        pts.append(float(s))
        avg += int(float(s) * 10000)

avg /= len(pts) * 10000

pts.sort()
fastest = pts[0]
slowest = pts[len(pts) - 1]
med     = pts[len(pts) // 2]
one     = pts[len(pts) * 99 // 100]

print("fastest: " + str(fastest)  + "ms")
print("slowest: " + str(slowest)  + "ms")
print("average: " + str(avg)[0:7] + "ms")
print(" median: " + str(med)      + "ms")
print("    99%: " + str(one)      + "ms")
