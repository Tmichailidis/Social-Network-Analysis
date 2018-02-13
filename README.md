Social Network Implementation(Software Development): Creating a Social Network with users,communities,groups and implementing graph analysis tools using C. Based on the SIGMOD 2014 Contest (http://www.cs.albany.edu/~sigmod14contest/task.html).


Metrics:
- degree Distribution
- diameter
- average Path Length
- number of connected component
- max connected component
- density
- closeness centrality
- betweenness centrality

Functionality:
- reachNode1: finds minimum path between 2 nodes.
- reachNodeN: finds minimum path from one node to all others.
- matchSuggestion: matches people using Jaccard index. (Query 1)
- getTopStalkers: finds top K stalkers in our SN. (Query 2)
- findTrends: find top K women's and men's trends. (Query 3)
- estimateTrust: estimates trust between 2 nodes using a variation of TidalTrust. (Query 4)
- cliquePercolationMethod: implementation of a Node Centric Community Detection algorithm. (https://en.wikipedia.org/wiki/Clique_percolation_method)
- GirvanNewman: implementation of a Divisive Hierarchical Clustering algorithm. (https://en.wikipedia.org/wiki/Girvan–Newman_algorithm))

A main file is included for testing.

Before running, untar Files.tar.gz

compile: make

run: ./sn_analysis
