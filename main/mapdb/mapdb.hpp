#pragma once

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <ctime>

#include "node.h"

#define RAPIDXML_NO_EXCEPTIONS 1
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"


namespace std {
    template <typename T>
    T move(T& in) {
        return (T&&)in;
    }
}

namespace store {}
namespace map_algorithm {}
