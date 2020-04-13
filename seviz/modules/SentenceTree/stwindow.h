#ifndef STWINDOW_H
#define STWINDOW_H

#include <QWidget>
#include <QWebEngineView>

class SentenceTree;
class Sentence;
struct SentenceData;
class ConstituencyTree;
class DependencyTree;

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
    void onDepCreateEdge(int from, int to);
    void onDepRemoveEdge(int from, int to);
    void onDepChangeEdgeType(int from, int to);

private:
    Ui::STWindow *ui;
    QWebChannel* m_webchannel;

    SentenceTree* m_core;
    QString m_sentenceText;

    void renderConstituency(const ConstituencyTree& tree);
    void renderDependencies(const DependencyTree& tree);
};

#endif // STWINDOW_H
