import osmnx as ox

G = ox.graph_from_place('Мадрид, Испания', network_type='drive')
ox.save_graphml(G, 'madrid_road_network.graphml')
