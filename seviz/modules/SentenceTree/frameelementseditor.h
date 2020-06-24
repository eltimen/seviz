#ifndef FRAMEELEMENTSEDITOR_H
#define FRAMEELEMENTSEDITOR_H

#include <QDialog>
#include <QComboBox>
#include <unordered_map>
#include <unordered_set>
#include "BookModels.h"

class Frame;

namespace Ui {
class FrameElementsEditor;
}

class FrameElementsEditor : public QDialog
{
    Q_OBJECT

public:
    explicit FrameElementsEditor(QWidget* parent, Frame* frame);
    ~FrameElementsEditor();

private slots:
    void onSave();

private:
    Ui::FrameElementsEditor *ui;
    Frame* m_frame;
    std::map<int, Word> m_words; // ID - Слово
    std::vector<Word> m_wordsPool; // pool слов для заполнения FE // TODO заменить на пул ID-шников!!!!!
    std::unordered_map<QComboBox*,int> m_previousFrom;
    std::unordered_map<QComboBox*,int> m_previousTo;

    std::map<QString, std::shared_ptr<Frame>> m_subFrames;

    std::vector<Word> getPossibleEndsOfWordRangeFrom(int id);
    void takeWordRangeFromPool(int from, int to);
    void restoreWordRangeToPool(int from, int to);
    void setupWidgets();
    void setupWordsWidget();
};

#endif // FRAMEELEMENTSEDITOR_H
