#pragma once
#include <string>
#include <map>
#include <vector>

class ConsistentHashRing {
public:
    ConsistentHashRing(int virtualNodes = 3);

    void addNode(int nodeId);
    int getNode(const std::string& key) const;

private:
    int virtualNodes;
    std::map<size_t, int> ring; // hash -> nodeId

    size_t hash(const std::string& key) const;
};

