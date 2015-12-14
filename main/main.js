var Canvas = require('canvas'),
    Image = Canvas.Image,
    canvas = new Canvas(1000, 1000),
    ctx = canvas.getContext('2d'),
    mapdb = require("./mapdb/build/Release/mapdb");
  

function drawMap() {
    var maxLon = 122.0000, 
        minLon = 121.0000,
        maxLat = 31.5000,
        minLat = 30.5000;
    var width = maxLon - minLon,
        height = maxLat - minLat,
        widthRatio = 990 / width,
        heightRatio = 990 / height,
        ratio = Math.min(widthRatio, heightRatio);


    var clusterColor = ["#00ff00", "#0000ff", "#ff0000", "#660033",
    "#00FF66", "#3399FF", "#66FF33", "#66FFFF", "#66FFFF", "#CC9999", "#FF00FF", 
    "#FFCC00"];

    //console.log(maxLon, minLon, maxLat, minLat);
    //console.log(width, height);

    var widthOffset = 5,
        heightOffset = 5;

    var n = mapdb.getN(),
        m = mapdb.getM();


    function trans(node) {
        node.lon = node.lon - minLon;
        node.lat = node.lat - minLat;
        node.lat = height - node.lat;
        node.lon = node.lon * ratio;
        node.lat = node.lat * ratio;
        return node;
    }

    for (var i = 0; i < n; i += 1) {
        var node = mapdb.getNode(i);

        node = trans(node);
        
        ctx.fillStyle = clusterColor[mapdb.getCluster(i)];
        ctx.fillRect(node.lon, node.lat, 1, 1);
    }

    var count = 0;
    for (var i = 0; i < m; i += 1) {
        var edge = mapdb.getEdge(i);
        var node_a = mapdb.getNode(edge.a),
            node_b = mapdb.getNode(edge.b);
        node_a = trans(node_a);
        node_b = trans(node_b);
        var color_a = mapdb.getCluster(edge.a),
            color_b = mapdb.getCluster(edge.b);
        if (color_a == color_b) {
            // console.log(color_a);
            ctx.strokeStyle = clusterColor[color_a];
            ctx.beginPath();
            ctx.moveTo(node_a.lon, node_a.lat);
            ctx.lineTo(node_b.lon, node_b.lat);
            ctx.stroke();
        }
        else {
            //console.log(node_a.lon, node_a.lat, "  ", node_b.lon, node_b.lat);
            ++count;
        }
    }

}

mapdb.loadMapFromXML("mapdb/map1.osm");
mapdb.initMapServer({
    "highway": [
        "motoway", "trunk", "primary", "secondary", 
        "tertiaty", "unclassified", "residental", 
        "motoway_link", "trunk_link", "primary_link", 
        "tertiary_link", "road"
    ]
});
drawMap();
console.log('<img src="' + canvas.toDataURL() + '" />');
