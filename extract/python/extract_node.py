import xml.etree.ElementTree as ET
import sys


ALLOWED_WAY_TYPES = ["motoway", "trunk", "primary", "secondary", "tertiaty", "unclassified", 
                    "residental", "motoway_link", "trunk_link", "primary_link", "tertiary_link", "road"]
ALLOWED_ALL_TYPES = True
file_name = sys.argv[1]


root = ET.parse(file_name).getroot()


ways = map(lambda way: map(lambda child: child.attrib["ref"], filter(lambda child: child.tag == "nd", way._children)), 
           filter(lambda way: way.tag == "way" and reduce(lambda x,a: x or a, map(lambda child: child.tag == "tag" and child.attrib["k"] == "highway" and (child.attrib["v"] in ALLOWED_WAY_TYPES or ALLOWED_ALL_TYPES), way._children)),
               root._children)
        )

nds_map = reduce(lambda a, b: a.union(b), map(set, ways))


nds = map(lambda nd: [nd.attrib["id"], nd.attrib["lat"], nd.attrib["lon"]], filter(lambda nd: nd.tag == "node" and nd.attrib["id"] in nds_map, root._children))

edges = reduce(lambda a, way: a + zip(way[:-1], way[1:]), [[]] + ways)

print len(nds)
for nd in nds:
    print nd[0], nd[1], nd[2]

print len(edges)
for edge in edges:
    print edge[0], edge[1]
