var mapdb = require("./build/Release/mapdb.node");
mapdb.loadMapFromXML("map.osm");
console.log(mapdb.initMapServer({
    "highway": ["motoway", "trunk", "primary", "secondary", "tertiaty", "unclassified",
    "residental", "motoway_link", "trunk_link", "primary_link", "tertiary_link", "road"]
}))
console.log(mapdb.getN());
console.log(mapdb.getM());
console.log(mapdb.getW());
//console.log(mapdb.getCluster(100));
