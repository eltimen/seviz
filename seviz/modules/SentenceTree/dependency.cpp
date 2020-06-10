#include "dependency.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include <unordered_set>
#include <algorithm>
#include <vector>

DependencyTree::DependencyTree(const Sentence& sent)
	: m_sentence(sent) 
{
}

bool DependencyTree::insert(int from, int to, DependencyRelation type) {
	assert(from >= 1);
	assert(to <= m_sentence.size());

	if (!canInsertRelation(from, to)) {
		return false;
	}

    m_tree.emplace(from, std::make_pair(to, type));
	return true;
}

void DependencyTree::remove(int from, int to) {
	const auto& outs = m_tree.equal_range(from);
	const auto& relation = std::find_if(outs.first, outs.second, [to](const auto& edge) -> bool { return edge.second.first == to; });
	assert(relation != m_tree.end());
	m_tree.erase(relation);
}

void DependencyTree::change(int from, int to, DependencyRelation type) {
    auto outs = m_tree.equal_range(from);
    auto edge = std::find_if(outs.first, outs.second, [to](const auto& e) -> bool { return e.second.first == to; });
	edge->second.second = type;
}

bool DependencyTree::canInsertRelation(int from, int to) const {
    return !hasEdge(from, to) && !hasLoopWithEdge(from, to);
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

inline bool DependencyTree::hasEdge(int from, int to) const {
	auto iters = m_tree.equal_range(from);
	return std::find_if(iters.first, iters.second, [&to](const auto& tree) {return tree.second.first == to; }) != iters.second;
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

void DependencyTree::fromStanfordCoreNlpJson(const QString& basicDepJson) {
	/* пример входных данных
	{
          "dep": "nsubj",
          "governor": 9,
          "governorGloss": "shouldn",
          "dependent": 12,
          "dependentGloss": "eat"
        },
	*/
	// если в dep сдержится двоеточие - это universal dependencies и тогда берем часть справа от двоеточия
	// если соотношение не найдено в программе - игнорируем эту связь

	m_tree.clear();

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(basicDepJson.toUtf8(), &error);
	if (!doc.isNull()) {
		qDebug() << doc.isArray() << doc.isEmpty() << doc.isNull() << doc.isObject();
		QJsonValue val = doc.object().value("basicDependencies");
		qDebug() << doc.object().keys();
		QJsonArray basicDep = val.toArray();
		for (const QJsonValue& val : basicDep) {
			QString dep = val["dep"].toString();
			int from = val["governor"].toInt();
			int to = val["dependent"].toInt();

			int depIndex = DependencyRelationStr.indexOf(dep);
			if (depIndex >= 0) {
				insert(from, to, static_cast<DependencyRelation>(depIndex));
			}
		}
	}
}

QString DependencyTree::toStanfordCoreNlpJson() const {
	QString ret = "{\"basicDependencies\": [ \n";
	// Qt don't support trailing commas 
	int i = 0;
	for (const auto& dep : m_tree) {
		int fromId = dep.first;
		int toId = dep.second.first;
		QString relation = DependencyRelationStr[static_cast<int>(dep.second.second)];

		if (i) 
			ret.append(",");

		ret += QStringLiteral(R"(
		{
          "dep": "%1",
          "governor": %2,
          "dependent": %3
        }
		)").arg(relation)
			.arg(QString::number(fromId))
			.arg(QString::number(toId));

		++i;
	}

	ret += "]}";

    return ret;
}

QString DependencyTree::toBratJson() const {
	QString entities;
	int lastIndex = 0;
	for (const Word& w : m_sentence) {
		int tokenEndIndex = lastIndex + w.text().length();
		entities += QStringLiteral("\t[\"N%1\", \"%2\", [[%3, %4]]],\n").arg(
			QString::number(w.id()),
			"Token", // TODO POS-tag
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
			DependencyRelationStr[static_cast<int>(edge.second)],
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
