#include "consistenthashing.h"
#include <functional>

ConsistentHashRing::ConsistentHashRing(int vNodes)
    : virtualNodes(vNodes) {}

size_t ConsistentHashRing::hash(const std::string& key) const {
    return std::hash<std::string>{}(key);
}

void ConsistentHashRing::addNode(int nodeId) {
    for (int i = 0; i < virtualNodes; i++) {
        std::string vnodeKey = "NODE_" + std::to_string(nodeId) + "_" + std::to_string(i);
        ring[hash(vnodeKey)] = nodeId;
    }
}

int ConsistentHashRing::getNode(const std::string& key) const {
    if (ring.empty()) return -1;

    size_t h = hash(key);
    auto it = ring.lower_bound(h);

    if (it == ring.end()) {
        return ring.begin()->second;
    }
    return it->second;
}
