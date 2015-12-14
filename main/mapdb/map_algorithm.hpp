#include "mapdb.hpp"


namespace map_algorithm {

    using namespace std;

    set<pair<string, string>> filter;
    vector<size_t> d;
    vector<vector<size_t>> block_edge;
    size_t max_block_size;
    size_t max_out_node_size;

    struct DisjointSet {
        
        vector<size_t> v;
        vector<size_t> b_size;
        vector<size_t> o_ct;

        size_t& get_parent(const size_t& x) {
            if (x != v[x])
                v[x] = get_parent(v[x]);
            return v[x];
        }

        size_t& operator[](const size_t& x) {
            return get_parent(x);
        }

        void clear(const size_t& n) {
            v.clear();
            v.resize(n);
            b_size.clear();
            b_size.resize(n);
            o_ct.clear();
            o_ct.resize(n);
            for (size_t i = 0; i < n; ++i) {
                v[i] = i;
                b_size[i] = 1;
                o_ct[i] = d[i];
            }
        }

    } disjoint_set;

    template<typename Edge, typename Point, typename Cluster>
    void init(const vector<Point>& v, const vector<Edge>& e, Cluster& cluster, set<pair<string, string>>&& _filter) {
        filter = _filter;
        auto &f = disjoint_set;

        d.clear();
        d.resize(v.size());
        
        max_block_size = 10000; 
        max_out_node_size = 10;

        for (size_t i = 0; i < e.size(); ++i)
            if (filter.find(make_pair("highway", e[i].road_type)) != filter.end()) {
                int a = e[i].a,
                    b = e[i].b;
                ++d[a];
                ++d[b];
            }

        f.clear(v.size());

        srand(time(0));

        for (size_t i = 0; i < e.size(); ++i) {
            if (filter.find(make_pair("highway", e[i].road_type)) != filter.end()) {
                int a = f[e[i].a],
                        b = f[e[i].b];
                if (a == b) {
                    f.o_ct[a] -= 2;
                }
                else {
                    if (f.o_ct[a] + f.o_ct[b] - 2 <= max_out_node_size &&
                        f.b_size[a] + f.b_size[b] <= max_block_size) {
                        f[a] = b;
                        f.o_ct[b] += f.o_ct[a] - 2;
                        f.b_size[b] += f.b_size[a];
                    }
                }
            }
        }


        auto & be = block_edge;
        be.clear();
        be.resize(v.size());
        cluster.clear();
        cluster.resize(v.size(), -1);

        for (size_t i = 0; i < e.size(); ++i) {
            int a = f[e[i].a],
                b = f[e[i].b];
            if (a != b) {
                be[a].push_back(b);
                be[b].push_back(a);
            }
        }

        static queue<int> Q;
        while (!Q.empty()) Q.pop();


        size_t count = 0, out_point_count = 0, max_block_size = 0, max_out_point_count = 0;
        for (size_t i = 0; i < v.size(); ++i)
            if (f[i] == i && f.b_size[i] > 1) {
                ++count;
                out_point_count += f.o_ct[i];
                max_block_size = max(max_block_size, f.b_size[i]);
                max_out_point_count = max(max_out_point_count, f.o_ct[i]);
                Q.push(i);
            }

        for ( ; !Q.empty(); Q.pop()) {
            int x = Q.front();
            if (cluster[x] != -1)
                continue;
            static vector<bool> used;
            used.clear();
            used.resize(max_out_node_size, 0);
            for (size_t i = 0; i < be[x].size(); ++i)
                if (cluster[be[x][i]] != -1)
                    used[cluster[be[x][i]]] = 1;
            for (size_t i = 0; i < max_out_node_size; ++i)
                if (!used[i]) {
                    cluster[x] = i;
                    break;
                }
        }


        int max_cluster_id = 0;
        for (size_t i = 0; i < v.size(); ++i) {
            if (f[i] == i)
                max_cluster_id = max(max_cluster_id, cluster[i]);
            cluster[i] = cluster[f[i]];
        }

        int key_point = 0;
        for (size_t i = 0; i < v.size(); ++i)
            if (d[i] > 2)
                ++key_point;

        printf("key point: %d\n", key_point);
        printf("max cluster: %d\n", max_cluster_id);
        printf("max block size: %d\n", max_block_size);
        printf("max out point count: %d\n", max_out_point_count);
        printf("out point count: %d\n", out_point_count);
        printf("block count: %d\n", count);
    }
}
