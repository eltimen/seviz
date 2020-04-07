#include "dependency.h"
#include <unordered_set>
#include <algorithm>
#include <vector>

DependencyTree::DependencyTree(const Sentence& sent) :
	m_sentence(sent) 
{
    // TEST
	if (sent.size() > 0) {
		insert(sent.first().id(), sent.last().id(), nsubj);
		change(sent.first().id(), sent.last().id(), advd);
	}
}

bool DependencyTree::insert(int from, int to, DependencyRelation type) {
	assert(from >= 1);
	assert(to <= m_sentence.size());

	if (hasLoopWithEdge(from, to)) {
		return false;
	}

    m_tree.emplace(from, std::make_pair(to, type));
	return true;
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

bool DependencyTree::hasLoopWithEdgeUtil(int v, std::vector<bool>& visited, std::vector<bool>& recStack, int from, int to) const {
	if (!visited[v]) {
		visited[v] = true;
		recStack[v] = true;

		// по всем вершинам, смежным с v 
		auto iters = m_tree.equal_range(v+1);
		for (auto& subtreeIter = iters.first; subtreeIter != iters.second; ++subtreeIter) {
			int i = subtreeIter->second.first - 1; // индекс текущей смежной вершины
			if (!visited[i] && hasLoopWithEdgeUtil(i, visited, recStack, from, to))
				return true;
			else if (recStack[i])
				return true;
		}
		// а также повторяем шаг, если бы действительно существовало ребро от from до to
		if (v == (from-1)) {
			if (!visited[to-1] && hasLoopWithEdgeUtil(to-1, visited, recStack, from, to))
				return true;
			else if (recStack[to-1])
				return true;
		}
	}
	recStack[v] = false;  
	return false;
}

bool DependencyTree::hasLoopWithEdge(int from, int to) const {
	int verticesCount = m_sentence.size();
	std::vector<bool> visited(verticesCount, false);
	std::vector<bool> recStack(verticesCount, false);

	for (int i = 0; i < verticesCount; i++)
		if (hasLoopWithEdgeUtil(i, visited, recStack, from, to))
			return true;

	return false;
}

QString DependencyTree::toBratJson() const {
	QString entities;
	int lastIndex = 0;
	for (const Word& w : m_sentence) {
		int tokenEndIndex = lastIndex + w.text().length();
		entities += QStringLiteral("\t[\"N%1\", \"%2\", [[%3, %4]]],\n").arg(
			QString::number(w.id()),
			"Word", // TODO POS-tag
			QString::number(lastIndex),
			QString::number(tokenEndIndex)
		);
		lastIndex = tokenEndIndex + 1; // + пробел
	}

	QString relations;
	int i = 0;
	for (const auto& adjacency : m_tree) {
		const auto& from = adjacency.first;
		const auto& edge = adjacency.second;

		relations += QStringLiteral("\t[\"R%1\", \"%2\", [[\"From\", \"N%3\"], [\"To\", \"N%4\"]]],\n").arg(
			QString::number(i),
			string(edge.second),
			QString::number(from),
			QString::number(edge.first)
		);
		++i;
	}

	QString sentenceText;
	for (const Word& w : m_sentence) {
		sentenceText.append(w.text() + " ");
	}

	QString docData = QStringLiteral(R"(
        { 
            "source_files": ["ann", "txt"],
            "entities": [
			%1
			],
			"relations": [
			%2
			],
			"text": "%3"
        }       
    )").arg(entities, relations, sentenceText);


	return docData;
}

QString DependencyTree::string(DependencyRelation rel) const {
	#define MAKE_STRINGS(VAR) QStringLiteral(#VAR),
	QString const relationStrings[] = {
		DEPENDENCY_RELATION(MAKE_STRINGS)
	};
	#undef MAKE_STRINGS
	return relationStrings[rel];
}
