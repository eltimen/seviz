#ifndef FRAMEELEMENTSEDITOR_H
#define FRAMEELEMENTSEDITOR_H

#include <QDialog>
#include <unordered_map>
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
    std::vector<Word> m_words; // слова фрейма (без LU)
    std::vector<std::shared_ptr<Frame>> m_subFrames;
    std::unordered_map<int, int> m_wordIndexById;
    std::map<QString, int> m_subFrameIndexByFrameName;

    void setupWidgets();
    void setupWordsWidget();
};

#endif // FRAMEELEMENTSEDITOR_H
