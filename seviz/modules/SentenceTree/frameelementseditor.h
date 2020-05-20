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
    std::unordered_map<int, int> m_wordIndexById;

    void setupWidgets();
};

#endif // FRAMEELEMENTSEDITOR_H
