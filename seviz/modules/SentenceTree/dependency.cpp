#include "dependency.h"
#include <unordered_set>

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
