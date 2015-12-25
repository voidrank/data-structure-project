var Canvas = require('canvas'),
    Image = Canvas.Image,
    mapdb = require("./mapdb/build/Release/mapdb"),
    fs = require("fs");

mapdb.loadMapFromXML("mapdb/map.osm");

var HIGH_WAY =      JSON.parse(fs.readFileSync("./config/HIGH_WAY.json", "utf8")),
    WAY_COLOR =     JSON.parse(fs.readFileSync("./config/WAY_COLOR.json", "utf8")),
    WAY_VISIBLE =   JSON.parse(fs.readFileSync("./config/WAY_VISIBLE.json", "utf8")),
    LINE_WIDTH =    JSON.parse(fs.readFileSync("./config/LINE_WIDTH.json", "utf8")),
    WIDTH_SCALE =   JSON.parse(fs.readFileSync("./config/WIDTH_SCALE.json", "utf8")),
    HEIGHT_SCALE =  JSON.parse(fs.readFileSync("./config/HEIGHT_SCALE.json", "utf8"));

mapdb.initMapServer({
    "highway": [
        "motorway", "trunk", "primary", "secondary",
        "tertiary", "unclassified", "residential",
        "motorway_link", "trunk_link", "primary_link",
        "secondary_link", "tertiary_link", "road"
    ]
});





function drawMap(scale) {
    var HEIGHT = HEIGHT_SCALE[scale],
        WIDTH = WIDTH_SCALE[scale];

    var canvas = new Canvas(WIDTH, HEIGHT),
        ctx = canvas.getContext('2d');

    var maxLon = 122.0883,
        minLon = 121.1572,
        maxLat = 31.4287,
        minLat = 30.6639;

    this.minLon = minLon;
    this.maxLon = maxLon;
    this.minLat = minLat;
    this.maxLat = maxLat;


    this.LINE_WIDTH = LINE_WIDTH[scale];


    var width = maxLon - minLon,
        height = maxLat - minLat,
        widthRatio = WIDTH / width,
        heightRatio = HEIGHT / height,
        ratio = Math.min(widthRatio, heightRatio);

    this.width = width;
    this.height = height;
    this.widthRatio = widthRatio;
    this.heightRatio = heightRatio;
    this.ratio = ratio;


    var clusterColor = ["#00ff00", "#0000ff", "#ff0000", "#660033",
    "#00FF66", "#3399FF", "#66FF33", "#66FFFF", "#66FFFF", "#CC9999", "#FF00FF",
    "#FFCC00"];

    //console.log(maxLon, minLon, maxLat, minLat);
    //console.log(width, height);

    var n = mapdb.getN(),
        m = mapdb.getM(),
        w = mapdb.getW();






    for (var i = 0; i < w; ++i) {
        var way = mapdb.getWay(i);
        if (HIGH_WAY.indexOf(way.roadType) > -1 && WAY_VISIBLE[scale][way.roadType]) {
            var op = way.op,
                ed = way.ed;
            for (var j = op; j < ed; ++j) {
                var edge = mapdb.getEdge(j),
                    node_a = this.trans(mapdb.getNode(edge.a)),
                    node_b = this.trans(mapdb.getNode(edge.b));
                ctx.strokeStyle = WAY_COLOR[way.roadType];
                ctx.beginPath();
                ctx.moveTo(node_a.lon, node_a.lat);
                ctx.lineTo(node_b.lon, node_b.lat);
                ctx.lineWidth = LINE_WIDTH[scale];
                ctx.stroke();
            }
        }
    }

    this.canvas = canvas;
    this.ctx = ctx;
};

drawMap.prototype.toDataURL = function () {
    return this.canvas.toDataURL();
};

drawMap.prototype.trans = function (node) {
    node.lon = node.lon - this.minLon;
    node.lat = node.lat - this.minLat;
    node.lat = this.height - node.lat;
    node.lon = node.lon * this.ratio;
    node.lat = node.lat * this.ratio;
    return node;
}

drawMap.prototype.crop = function(lon, lat) {
    var lu = this.trans({lon: lon, lat: lat});
    var retCanvas = new Canvas(1000, 500),
        retCtx = retCanvas.getContext("2d");
    retCtx.putImageData(this.ctx.getImageData(lu.lon, lu.lat, 1000, 500), 0, 0);
    return retCanvas.toDataURL();
};

drawMap.prototype.drawPoint = function(lon, lat) {
    var ctx = this.ctx;
    ctx.strokeStyle = "green";
    var node = this.trans({
        lon: lon,
        lat: lat
    });
    ctx.rect(node.lon, node.lat, 3, 3);
    ctx.stroke();
}

drawMap.prototype.drawShortestPath = function(x, y) {
    // key point
    var result = mapdb.queryPath(x, y);
    var ctx = this.ctx;
    ctx.strokeStyle = "black";
    ctx.beginPath();
    var first_node = this.trans(mapdb.getNode(result.path[0]));
    ctx.moveTo(first_node.lon, first_node.lat);
    ctx.lineWidth = this.LINE_WIDTH;
    for (var i = 1; i < result.path.length; ++i) {
        var node = this.trans(mapdb.getNode(result.path[i]));
        ctx.lineTo(node.lon, node.lat);
    }
    ctx.stroke();
}

module.exports = drawMap;
