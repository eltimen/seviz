#ifndef STWINDOW_H
#define STWINDOW_H

#include <QWidget>
#include <QWebEngineView>

class Sentence;
struct SentenceData;
class DependencyTree;

namespace Ui {
class STWindow;
}

class STWindow : public QWidget
{
    Q_OBJECT

public:
    explicit STWindow(QWidget *parent = nullptr);
    ~STWindow();

    void showSentence(const Sentence& sent, const SentenceData& data);

private:
    Ui::STWindow *ui;
    QWebChannel* m_webchannel;

    QString m_sentenceText;

    void renderDependencies(const DependencyTree& tree);
};

#endif // STWINDOW_H
