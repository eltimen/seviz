#ifndef STWINDOW_H
#define STWINDOW_H

#include <QWidget>
#include <QWebEngineView>

class SentenceTree;
class Sentence;
struct SentenceData;
class ConstituencyTree;
class DependencyTree;
class FrameTree;
class IEngine;

namespace Ui {
class STWindow;
}

class STWindow : public QWidget
{
    Q_OBJECT

public:
    explicit STWindow(SentenceTree* core);
    ~STWindow();

    void showSentence(const Sentence& sent, const SentenceData& data);

public slots:
    void onConstituencyCreateNode(int from, int to);
    void onConstituencyChangeNodeType(int id);
    void onConstituencyDeleteNode(int id);
    void onDepCreateEdge(int from, int to);
    void onDepRemoveEdge(int from, int to);
    void onDepChangeEdgeType(int from, int to);
    void onFrameInsert(IEngine* engine);
    void onFrameEdit(int id);

private:
    Ui::STWindow *ui;
    QWebChannel* m_webchannel;

    SentenceTree* m_core;
    QString m_sentenceText;

    void renderConstituency(const ConstituencyTree& tree);
    void renderDependencies(const DependencyTree& tree);
    void renderFrameNet(const FrameTree& tree);
};

#endif // STWINDOW_H
