#ifndef STWINDOW_H
#define STWINDOW_H

#include "BookModels.h"
#include <QWidget>
#include <QWebEngineView>
#include <QStringList>

class SentenceTree;
class Sentence;
struct SentenceData;
class ConstituencyTree;
class DependencyTree;
class FrameTree;
class IEngine;
struct FrameInsertionData;
class Frame;

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

    static std::vector<Word> getWordsInsideFrameRange(IEngine* engine, const Position& from, const Position& to);

public slots:
    void onConstituencyCreateNode(int from, int to);
    void onConstituencyChangeNodeType(int id);
    void onConstituencyDeleteNode(int id);
    void onDepCreateEdge(int from, int to);
    void onDepRemoveEdge(int from, int to);
    void onDepChangeEdgeType(int from, int to);
    void onFrameInsert(IEngine* engine);
    void onFrameEdit(int id);
    void onFrameRemove(int id);

private:
    Ui::STWindow *ui;
    QWebChannel* m_webchannel;

    SentenceTree* m_core;
    QString m_sentenceText;

    void renderConstituency(const ConstituencyTree& tree);
    void renderDependencies(const DependencyTree& tree);
    void renderFrameNet(const FrameTree& tree);

    // возвращает допустимые фреймы, определяя в процессе предполагаемые начальные формы слова. 
    // Внимание: реализовано через отрезание -s, -ed, -ing и т.д. => неправильные глаголы пока не поддерживаются
    QString getInitialFormOfWord(const QString& word) const;

    QStringList generateFrameChoosingPalette(const std::vector<Word>& frameWords, std::vector<std::pair<Word, QString>>& possibleFrames, FrameInsertionData& insertionContext);
    // спрашивает, куда вставлять фреймы, оказавшиеся внутри добавляемого фрейма
    std::map<QString, QString> askFrameElementsForSubframes(const FrameInsertionData& insertionData, Frame* newFrame);
    // спрашивает, в какой FE верхнего фрейма вставлять добавляемый фрейм
    QString askParentFrameElementForNewFrame(const FrameInsertionData& insertionData, Frame* newFrame);
};

#endif // STWINDOW_H
