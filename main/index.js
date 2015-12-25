var app = require("koa")(),
    session = require("koa-session"),
    router = new require("koa-router")(),
    configRouter = new require("koa-router")(),
    staticRouter = new require("koa-router")(),
    serve = new require("koa-static"),
    fs = require("fs"),
    bodyParser = require("koa-bodyparser"),
    views = require("koa-views");


var mapdb = require("./mapdb/build/Release/mapdb");

require("koa-qs")(app, "strict");

var Map = require("./draw");

var canvas = [];
for (var i = 0; i < 5; ++i)
    canvas.push(new Map(i));

app
    .use(function * (next) {
        console.log(this.url);
        yield next;
    })
    .use(bodyParser())
    .use(session(app))
    .use(router.routes())
    .use(configRouter.routes())
    .use(staticRouter.routes());

router
    .use(views("views", {
        map: {
            html: "underscore"
        }
    }))
    .get("/", function * (next) {
        this.state = {
            session: this.session,
            title: "map"
        };

        yield this.render("./index.html");
    })
    .get("/getMap", function * (next) {
        var lon = parseFloat(this.query["lon"][0]),
            lat = parseFloat(this.query["lat"][0]),
            scale = parseFloat(this.query["scale"][0]);

        this.body = canvas[scale].crop(lon, lat);
    })
    .get("/getNearestPoint", function * (next) {
        var lon = parseFloat(this.query["lon"][0]),
            lat = parseFloat(this.query["lat"][0]);
        var index = mapdb.getNearestPoint({
            lon: lon,
            lat: lat
        });
        var node = mapdb.getNode(index);
        lon = node.lon; lat = node.lat;
        for (var i = 0; i < 5; ++i) {
            canvas[i].drawPoint(lon, lat);
        }
        this.body = index;
    })
    .get("/calShortest", function * (next) {
        var node_a = parseFloat(this.query["a"][0]),
            node_b = parseFloat(this.query["b"][0]);
        console.log(node_a, node_b);
        for (var i = 0; i < 5; ++i)
            canvas[i].drawShortestPath(node_a, node_b);
        this.body = mapdb.query(node_a, node_b);
    })
    .get("/testShortest", function * (next) {
        var node_a = parseFloat(this.query["a"][0]),
            node_b = parseFloat(this.query["b"][0]);
        console.log(node_a, node_b);
        try {
            this.body = mapdb.query(node_a, node_b);
        }
        catch (err) {
            this.body = "123";
        }
    });



function read(filename) {
    return new Promise(function(solv, rej) {
        fs.readFile(filename, "utf8", function(err, data) {
            if (err) rej(err);
            solv(data);
        })
    });
}


configRouter.get("/config/:filename", function * (next) {
    this.body = yield read("./config/" + this.params.filename);
});
staticRouter.get("/static/:filename", function * (next) {
    this.body = yield read("./static/" + this.params.filename);
});


console.log("server up");
app.listen(3000);