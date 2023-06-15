
from common import print_tour, read_input
#!/usr/bin/env python3
import sys
import math
import numpy as np
import copy
from common import print_tour, read_input
MAX = 10000
def distance(cities):
    n = len(cities)
    city_distance = np.zeros((n,n))
    city_distance = city_distance.tolist()
    for i in range(0,n):
        for j in range(0,n):
            if i == j:
                city_distance[i][j] = MAX
            else:
                city_distance[i][j] = math.sqrt((cities[i][0] - cities[j][0]) ** 2 + (cities[i][1] - cities[j][1]) ** 2)          
    return city_distance
def solve(cities,city_distance_):
    city_distance = copy.deepcopy(city_distance_)

    n = len(cities)
    now = 0
    tour = [now]
    for i in range(0,n):
        city_distance[i][now] = MAX
    unvisited = list(map(int,range(1,n)))
    while(len(unvisited) != 0):
        next = city_distance[now].index(min(city_distance[now]))
        next_next = city_distance[next].index(min(city_distance[next]))
        if city_distance[next_next].index(min(city_distance[next_next])) == now:
            tour.append(next_next)
            tour.append(next)
            unvisited.remove(next_next)
            unvisited.remove(next)
            for i in range(0,n):
                city_distance[i][next] = city_distance[i][next_next] = MAX
        else:
            unvisited.remove(next)
            tour.append(next)
            for i in range(0,n):
                city_distance[i][next] = MAX
        now = next
       
    return tour

def count_distance(tour,city_distance):
    sum = 0
    for i in range(0,len(tour)-1):
        sum += city_distance[tour[i]][tour[i+1]]
    return sum


if __name__ == '__main__':
    cities = read_input('input_6.csv')
    city_distance = distance(cities)
    tour = solve(cities,city_distance)
    sum_distance = count_distance(tour,city_distance)
    print_tour(tour)
    print(sum_distance)
    
