#include "mapdb.hpp"


FILE *log_file = fopen("log", "w");

namespace xml_parser {
    
    using namespace std;
    using namespace rapidxml;

    xml_document<> doc;
    char charpool[1024*1024*512];

    void loadMap(const string filename = "map.in") {
        FILE *file = fopen(filename.c_str(), "r");
        //size_t length = ftell(file);
        //fprintf(log_file, "%d\n", length);
        //fclose(log_file);
        memset(charpool, 0, sizeof charpool);
        
        fread(charpool, sizeof charpool[0], sizeof charpool / sizeof charpool[0], file);
        char *p = charpool;
        doc.parse<0>(p);
    }

}


namespace store {

    using namespace std;
    

    struct Point {
        double lat, lon;
        long long id;
        string road_type;
    };


    struct Edge {
        long long a, b;
        string road_type;
    };

    struct Way {
        int op, ed;
        string name, road_type;
    };

    int n, m;
    vector<Point> v;
    vector<Edge> e;
    vector<Way> w;
    vector<int> cluster;
    map<long long, int> mapping;


    void loadMap(const string filename = "map.in") {
        FILE *file = fopen(filename.c_str(), "r");
        fscanf(file, "%d", &n);
        v.clear();
        mapping.clear();
        for (int i = 0; i < n; ++i) {
            long long id;
            double lat, lon;
            fscanf(file, "%lld%lf%lf", &id, &lat, &lon);
            mapping[id] = v.size();
            v.push_back(Point{lat, lon, id, ""});
        }
        fscanf(file, "%d", &m);
        for (int i = 0; i < n; ++i) {
            long long x, y;
            fscanf(file, "%lld%lld", &x, &y);
            e.push_back(Edge{x, y, ""});
        }
    }

    void loadMapFromXML(const string filename = "map.osm") {

        xml_parser::loadMap(filename);

        auto& doc = xml_parser::doc;
        auto *root = doc.first_node();
        v.clear();
        e.clear();
        mapping.clear();


        for (auto* node = root->first_node("node");
                node;
                node = node->next_sibling("node")) 
        {
            mapping[atoll(node->first_attribute("id")->value())]= v.size();
            v.push_back(Point{
                    atof(node->first_attribute("lat")->value()), 
                    atof(node->first_attribute("lon")->value()),
                    atoll(node->first_attribute("id")->value()),
                    ""
                    });
        }
        n = v.size();



        for (auto* way = root->first_node("way");
                way;
                way = way->next_sibling("way")) 
        {
            string road_type;
            auto *tag = way->first_node("tag");
            for ( ; tag; tag = tag->next_sibling("tag"))
                if (tag->first_attribute("k") && strcmp(tag->first_attribute("k")->value(), "highway") == 0) {
                    road_type = tag->first_attribute("v")->value();
                    break;
                }

            int op = e.size(), ed = e.size();
            auto *edge = way->first_node("nd");
            for (auto* nedge = edge->next_sibling("nd"); 
                    nedge;
                    edge = nedge, nedge = nedge->next_sibling("nd"))
            {
                ++ed;
                e.push_back(Edge{
                        mapping[atoll(edge->first_attribute("ref")->value())],
                        mapping[atoll(nedge->first_attribute("ref")->value())],
                        road_type});
            }

            string name;
            tag = way->first_node("tag");
            for ( ; tag && strcmp(tag->name(), "name") == 0; tag = tag->next_sibling("tag"));
            w.push_back(Way{op, ed, tag?(tag->value()):"no name", road_type});

        }
        m = e.size();

    }

    void loadCluster(const string filename = "test.out") {
        FILE *file = fopen(filename.c_str(), "r");
        cluster.clear();
        cluster.resize(n);
        for (int i = 0; i < n; ++i) {
            int x, y;
            fscanf(file, "%d%d", &x, &y);
            cluster[x] = y;
        }
    }


    int getN() {
        return v.size();
    }

    int getM() {
        return e.size();
    }

    int getW() {
        return w.size();
    }

    Point getPoint(int index) {
        return v[index];
    }

    Point getPointById(long long id) {
        return v[mapping[id]];
    }

    Edge getEdge(int index) {
        return e[index];
    }
    
    Way getWay(int index) {
        return w[index];
    }

    int getCluster(int index) {
        return cluster[index];
    }

    int getClusterById(int index) {
        return cluster[mapping[index]];
    }

    double getMaxLat() {
        double ret = -1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = max(ret, v[i].lat);
        return ret;
    }

    double getMinLat() {
        double ret = 1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = min(ret, v[i].lat);
        return ret;
    }

    double getMaxLon() {
        double ret = -1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = max(ret, v[i].lon);
        return ret;
    }

    double getMinLon() {
        double ret = 1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = min(ret, v[i].lon);
        return ret;
    }
             
}
