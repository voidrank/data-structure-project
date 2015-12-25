#include "mapdb.hpp"


namespace map_algorithm {

    using namespace std;
#define _1 first
#define _2 second

    const double eps = 1e-12;
    const double MAX_DISTANCE = 1e12;

    set<pair<string, string>> filter;

    // road points' edges
    vector<size_t> d;
    vector<vector<size_t>> ve;
    vector<vector<double>> de;

    // key points' edges
    vector<vector<size_t>> kve;
    vector<vector<double>> kde;

    // road points (link to key points)
    vector<vector<size_t>> rve;
    vector<vector<double>> rde;
    vector<size_t> rid;

    // global distance
    vector<vector<double>> gdis;
    // shortest way previous point
    vector<vector<size_t>> prev;
    // shortest way previous edge
    vector<vector<size_t>> prev_e;

    // key points
    vector<size_t> key_point;
    map<size_t, size_t> point_to_key_point;
    auto &ptkp = point_to_key_point;
    vector<bool> is_key_point;



    struct DisjointSet {
        
        vector<size_t> v;
        vector<double> dis;

        size_t& get_parent(const size_t& x) {
            int p = v[x];
            if (x != v[x]) {
                v[x] = get_parent(v[x]);
                dis[x] += dis[p];
            }
            return v[x];
        }

        size_t& operator[](const size_t& x) {
            return get_parent(x);
        }

        void clear(const size_t& n) {
            v.resize(n);
            for (size_t i = 0; i < n; ++i)
                v[i] = i;
            dis.resize(n, 0);
        }

    } disjoint_set[2];


    vector<int> cluster;
    vector<int> l, r;
    vector<double> dis_l, dis_r;



    template<typename Point>
    double cal_dis(Point a, Point b) {
        double dlat = a.lat - b.lat,
                dlon = a.lon - b.lon;
        return sqrt(dlat * dlat + dlon * dlon);
    }



    int getCluster(int index) {
        return cluster[index];
    }



    template<typename Global_Distance, typename Edge_Distance, typename Edge_Point>
    void sp(const size_t& root, Global_Distance& gdis, const Edge_Distance& de, const Edge_Point& ve) {

        static priority_queue< \
                pair<double, size_t>, \
                vector<pair<double, size_t>>, \
                greater<pair<double, size_t> > > Q;
        auto &dis = gdis[root];
        auto &last = prev[root];
        auto &last_e = prev_e[root];
        dis.resize(gdis.size(), 0);
        last.resize(gdis.size(), 0);
        last_e.resize(gdis.size(), 0);

        while (!Q.empty()) Q.pop();

        Q.push(make_pair(0, root));

        for (size_t i = 0; i < gdis.size(); ++i)
            dis[i] = MAX_DISTANCE;

        dis[root] = 0;

        for ( ; !Q.empty(); Q.pop()) {
            int x = Q.top()._2;
            double d = Q.top()._1;
            if (d != dis[x])
                continue;

            for (size_t i = 0; i != ve[x].size(); ++i)
                if (dis[x] + de[x][i] < dis[ve[x][i]]){
                    dis[ve[x][i]] = dis[x] + de[x][i];
                    last[ve[x][i]] = x;
                    last_e[ve[x][i]] = i;
                    Q.push(make_pair(dis[ve[x][i]], ve[x][i]));
                }
        }

    }



    pair<vector<size_t>, double> search_end(size_t origin, size_t edge_index, const bool& with_detail = 0) {
        double accum_dis = de[origin][edge_index];
        size_t now = ve[origin][edge_index], next_n, prev = origin;
        vector<size_t> ret;

        for (; d[now] == 2; prev = now, now = next_n) {
            next_n = (ve[now][1] == prev) ? ve[now][0] : ve[now][1];
            accum_dis += (ve[now][1] == prev) ? de[now][0] : de[now][1];
            if (with_detail)
                ret.push_back(now);
        }
        ret.push_back(now);

        return make_pair(ret, accum_dis);
    }



    template<typename Edge, typename Point>
    void init(const vector<Point>& v, const vector<Edge>& e, const set<pair<string, string>>& filter) {

        d.clear();
        d.resize(v.size(), 0);

        ve.clear();
        ve.resize(v.size());

        de.clear();
        de.resize(v.size());

        rde.clear();
        rde.resize(v.size());

        rve.clear();
        rve.resize(v.size());

        rid.resize(v.size(), -1);

        cluster.resize(v.size(), 7);

        key_point.clear();

        point_to_key_point.clear();

        is_key_point.resize(v.size(), 0);


        l.resize(v.size(), -1);
        r.resize(v.size(), -1);
        dis_l.resize(v.size(), 0);
        dis_r.resize(v.size(), 0);


        for (size_t i = 0; i < e.size(); ++i)
            if (filter.find(make_pair("highway", e[i].road_type)) != filter.end()) {
                // Degree
                ++d[e[i].a];
                ++d[e[i].b];

                // Edge
                ve[e[i].a].push_back(e[i].b);
                ve[e[i].b].push_back(e[i].a);

                // Value
                double val = cal_dis(v[e[i].a], v[e[i].b]);
                de[e[i].a].push_back(val);
                de[e[i].b].push_back(val);
            }

        for (size_t i = 0; i < v.size(); ++i)
            if (d[i] != 2 && d[i] != 0) {
                point_to_key_point[i] = key_point.size();
                key_point.push_back(i);
                is_key_point[i] = 1;
            }

        kve.clear();
        kve.resize(key_point.size());

        kde.clear();
        kde.resize(key_point.size());

        puts("Extract key route..");

        size_t road_count = 0;
        // key point
        for (size_t i = 0; i < v.size(); ++i)
            if (d[i] != 2 && d[i] != 0) {

                for (int j = 0; j < 2; ++j) {
                    rve[i].push_back(i);
                    rde[i].push_back(0);
                }

                for (size_t j = 0; j < ve[i].size(); ++j) {

                    auto result = search_end(i, j, 1);
                    size_t now = result._1.back();
                    double dis = result._2, accum_dis = cal_dis(v[i], v[result._1[0]]);
                    for (long k = 0; k < long(result._1.size()) - 1; ++k) {
                        if (ve[result._1[k]][1] == result._1[k+1]) {
                            rve[result._1[k]].push_back(i);
                            rve[result._1[k]].push_back(now);
                            rde[result._1[k]].push_back(accum_dis);
                            rde[result._1[k]].push_back(dis - accum_dis);
                        }
                        else {
                            rve[result._1[k]].push_back(now);
                            rve[result._1[k]].push_back(i);
                            rde[result._1[k]].push_back(dis - accum_dis);
                            rde[result._1[k]].push_back(accum_dis);
                        }
                        rid[result._1[k]] = road_count;
                        accum_dis += cal_dis(v[result._1[k]], v[result._1[k+1]]);
                    }

                    ++road_count;
                    kve[point_to_key_point[i]].push_back(point_to_key_point[now]);
                    kde[point_to_key_point[i]].push_back(dis);
                }
            }
        printf("Road count: %zu\n", road_count);
        printf("Key point cout: %zu/%zu\n", key_point.size(), v.size());
        puts("Finished..");

        gdis.clear();
        gdis.resize(key_point.size());

        prev.clear();
        prev.resize(key_point.size());

        prev_e.clear();
        prev_e.resize(key_point.size());

        printf("kde: %lu, kve: %lu, key_point: %lu, gdis: %lu\n", kde.size(), kve.size(), key_point.size(), gdis.size());

        puts("Calculating Shortest Path...");
        for (size_t i = 0; i < key_point.size(); ++i) {
            sp(i, gdis, kde, kve);
        }
        puts("Finished");


        size_t t = 0;
        for (size_t i = 0; i < key_point.size(); ++i)
            for (size_t j = 0; j < key_point.size(); ++j)
                if (gdis[i][j] >= MAX_DISTANCE || gdis[i][j] < eps)
                    ++t;
        printf("all: %lu; non: %lu\n", key_point.size() * key_point.size(), t);

        /*
        for (size_t i = 0; i < v.size(); ++i)
            if (d[i] == 2) {
                if (d[l[i]] == 2)
                    puts("error");
                if (d[r[i]] == 2)
                    puts("error");
            }
        */
    }



    pair<double, vector<size_t> > fail() {
        return make_pair(0, vector<size_t>());
    }



    pair<double,vector<size_t> > query(size_t index_a, size_t index_b, bool with_path = false) {

        if (index_a >= d.size() || index_b >= d.size())
            return fail();
        if (d[index_a] == 0 || d[index_b] == 0)
            return fail();

        size_t u, v;
        double minValue = MAX_DISTANCE;
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                if (gdis[point_to_key_point[rve[index_a][i]]][point_to_key_point[rve[index_b][j]]] + rde[index_a][i] + rde[index_b][j] < minValue) {
                    minValue = gdis[point_to_key_point[rve[index_a][i]]][point_to_key_point[rve[index_b][j]]] + rde[index_a][i] + rde[index_b][j];
                    printf("%.8f\n", minValue);
                    u = rve[index_a][i];
                    v = rve[index_b][j];
                }


        // TODO: 2 points are on the same road

        vector<size_t> ret;
        ret.push_back(index_b);

        if (with_path) {

            if (!is_key_point[index_b]) {
                for (int i = 0; i < 2; ++i)
                    if (rve[index_b][i] == v) {
                        puts("found1");
                        auto result = search_end(index_b, i, 1);
                        for (long j = 0; j < long(result._1.size()) - 1; ++j)
                            ret.push_back(result._1[j]);
                        break;
                    }
            }

            size_t key_u = ptkp[u],
                key_v = ptkp[v];

            for ( ; key_v != key_u; key_v = prev[key_u][key_v]) {
                size_t prev_n = prev[key_u][key_v];
                auto result = search_end(key_point[prev_n], prev_e[key_u][key_v], 1);
                while (!result._1.empty()) {
                    ret.push_back(result._1.back());
                    result._1.pop_back();
                }
            }


            if (!is_key_point[index_a]) {
                puts("found2");
                for (int i = 0; i < 2; ++i)
                    if (rve[index_a][i] == u) {
                        auto result = search_end(index_a, i, 1);
                        while (!result._1.empty()) {
                            ret.push_back(result._1.back());
                            result._1.pop_back();
                        }
                    }
            }

        }

        return make_pair(minValue, ret);

    }


    template<typename Point>
    size_t get_nearest_point(Point o, vector<Point> &v) {

        printf("%.4f %.4f\n", o.lon, o.lat);
        size_t id = 0;
        double min_dis = MAX_DISTANCE;
        for (size_t i = 0; i < d.size(); ++i)
            if (d[i] > 0) {
                if (min_dis > cal_dis(o, v[i])) {
                    min_dis = cal_dis(o, v[i]);
                    id = i;
                    printf("%d %.4f %.4f    %.4f\n", i, v[i].lon, v[i].lat, min_dis);
                }
            }

        return id;
    }

}
