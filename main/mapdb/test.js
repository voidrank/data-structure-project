var fs = require("fs");

var mapdb = require("./build/Release/mapdb.node");
mapdb.loadMapFromXML("map2.osm");

console.log("Init mapdb...")
console.log(mapdb.initMapServer({
    "highway": ["motorway", "trunk", "primary", "secondary", "tertiary", "unclassified",
    "residential", "motorway_link", "trunk_link", "primary_link", "tertiary_link", "road"]
}));
console.log("Server up!!!")


function testShrotestPath() {
    console.log(mapdb.getN());
    console.log(mapdb.getM());
    console.log(mapdb.getW());
    var x = mapdb.getRandomRoadPoint(),
        y = mapdb.getRandomRoadPoint();
    mapdb.queryPath(x, y);
}

testShrotestPath();
var id = mapdb.getNearestPoint({
    lon: 122,
    lat: 31
});
console.log("The nearest points " + id + ".");

//console.log(mapdb.getCluster(100));
