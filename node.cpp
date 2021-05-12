//
// Created by Jay on 4/22/21.
//

#include "node.h"


node::node(string &name, int gate_type) {
    node_name = name;
    gtype = gate_type;
}


node::node(string &name) {
    node_name = name;
}


node::~node() {
    unodes.clear();
    dnodes.clear();
}

