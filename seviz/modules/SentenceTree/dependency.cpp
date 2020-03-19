#include "dependency.h"
#include <unordered_set>
#include <algorithm>

void DependencyTree::insert(int from, int to, DependencyRelation type) {
	assert(from >= 1);
	assert(to <= m_sentence.size());

	if (hasLoopWithRelation(from, to)) {
		throw std::runtime_error("loop");
	}

    m_tree.emplace(from, std::make_pair(to, type));
}

void DependencyTree::remove(int from, int to) {
	const auto& outs = m_tree.equal_range(from);
	const auto& relation = std::find_if(outs.first, outs.second, [to](const auto& edge) -> bool { return edge.first == to; });
	assert(relation != m_tree.end());
	m_tree.erase(relation);
}

void DependencyTree::change(int from, int to, DependencyRelation type) {
    auto outs = m_tree.equal_range(from);
    auto edge = std::find_if(outs.first, outs.second, [to](const auto& e) -> bool { return e.second.first == to; });
	edge->second.second = type;
}

DependencyTree::DependencyTree(const Sentence& sent) :
	m_sentence(sent) 
{
	for (const Word& w : m_sentence) {
		
	}
}

bool DependencyTree::hasLoopWithRelation(int from, int to) const {
	std::unordered_set<int> visited = { to };
	// если в какой-то из узлов дерева входит больше двух ребер - цикл обнаружен
	for (const auto& edge : m_tree) {
		int dest = edge.second.first;
		if (visited.count(dest) == 0) {
			visited.insert(dest);
		} else {
			return true;
		}
	}
	return false;
}
