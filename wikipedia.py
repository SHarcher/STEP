import sys
import copy
import collections


class Wikipedia:

    # Initialize the graph of pages.
    def __init__(self, pages_file, links_file):

        # A mapping from a page ID (integer) to the page title.
        # For example, self.titles[1234] returns the title of the page whose
        # ID is 1234.
        self.titles = {} 
        self.title_id = {}
        self.ranks = {}
        self.sum_rank = 0
        # A set of page links.
        # For example, self.links[1234] returns an array of page IDs linked
        # from the page whose ID is 1234.
        self.links = {}

        # Read the pages file into self.titles.

        with open(pages_file) as file:
            for line in file:
                (id, title) = line.rstrip().split(" ")
                id = int(id)
                assert not id in self.titles, id
                self.titles[id] = title
                self.links[id] = []
                self.title_id[title] = id
                self.ranks[id] = 1
                self.sum_rank += 1
        print("Finished reading %s" % pages_file)

        # Read the links file into self.links.
        with open(links_file) as file:
            for line in file:
                (src, dst) = line.rstrip().split(" ")
                (src, dst) = (int(src), int(dst))
                assert src in self.titles, src
                assert dst in self.titles, dst
                self.links[src].append(dst)
        print("Finished reading %s" % links_file)
        print()


    # Find the longest titles. This is not related to a graph algorithm at all
    # though :)
    def find_longest_titles(self):
        titles = sorted(self.titles.values(), key=len, reverse=True)
        print("The longest titles are:")
        count = 0
        index = 0
        while count < 15 and index < len(titles):
            if titles[index].find("_") == -1:
                print(titles[index])
                count += 1
            index += 1
        print()


    # Find the most linked pages.
    def find_most_linked_pages(self):
        link_count = {}
        for id in self.titles.keys():
            link_count[id] = 0

        for id in self.titles.keys():
            for dst in self.links[id]:
                link_count[dst] += 1

        print("The most linked pages are:")
        link_count_max = max(link_count.values())
        for dst in link_count.keys():
            if link_count[dst] == link_count_max:
                print(self.titles[dst], link_count_max)
        print()


    # Find the shortest path.
    # |start|: The title of the start page.
    # |goal|: The title of the goal page.
    def find_shortest_path(self, start, goal):
        #------------------------#
        # Write your code here!  #
        #スタートノードをキューに追加し、キューが空になるまでのwhileループ
        start_id = self.title_id.get(start)
        goal_id = self.title_id.get(goal)

        if start_id is None or goal_id is None:
            print("Start or goal page is not found.")
            return None
        queue = collections.deque([start_id])
        route = {start_id : [start]}
        visited = [start_id]
        while queue:
            node = queue.popleft()
            child = self.links[node]
            #最短ルート保存
            for item in child:
                if item not in visited:
                    queue.append(item)                    
                    route[item] = route[node] + [self.titles[item]]
            if node not in visited:
                visited.append(node)
            if node == goal_id:
                path = route[node]
                for path_node in path:
                   print(path_node,end=' ') 
                break
            if len(queue) == 0:    
                print("No path found from {} to {}.".format(start, goal))
        return None


        #------------------------#



    # Calculate the page ranks and print the most popular pages.
    def find_most_popular_pages(self):
        #------------------------#
        # Write your code here!  #

        while(1):
            pre_rank = {}
            is_rank_end = 0
            threshold = 0.01
            for  item in self.titles:
                child = self.links[item]
                deliver = self.ranks[item]*0.85/len(child)
                for child_item in child:
                    if child_item not in pre_rank:
                        pre_rank[child_item] = deliver + 0.15*self.ranks[child_item]
                    else:
                        pre_rank[child_item] += deliver
                
            

            for item in self.titles:
                is_rank_end += abs(pre_rank[item]-self.ranks[item])
            if abs(sum(pre_rank.values()) - self.sum_rank ) >= 0.1**5 :
                print('error')
            
            self.ranks = pre_rank

            if is_rank_end > threshold:
                self.ranks = pre_rank
            else:
                rank = []
                sorted(self.ranks)
                for id in self.ranks:
                    rank.append(self.ranks[id])
                subrank = copy.deepcopy(rank)
                for top in range(10):
                    index_number = rank.index(max(rank))
                    rank[index_number] = 0
                    print(self.titles[index_number+1])
                break
        #------------------------#
        pass


    # Do something more interesting!!
    def find_something_more_interesting(self):
        #------------------------#
        # Write your code here!  #
        #------------------------#
        pass


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: %s pages_file links_file" % sys.argv[0])
        exit(1)

    wikipedia = Wikipedia('pages_medium.txt','links_medium.txt')
    #wikipedia.find_longest_titles()
   # wikipedia.find_most_linked_pages()
    wikipedia.find_shortest_path("渋谷", "パレートの法則")
    wikipedia.find_most_popular_pages()