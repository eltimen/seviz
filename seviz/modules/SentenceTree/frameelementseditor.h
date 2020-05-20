#ifndef FRAMEELEMENTSEDITOR_H
#define FRAMEELEMENTSEDITOR_H

#include <QDialog>
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
    std::vector<Word> m_words;

    void fillWindow();
};

#endif // FRAMEELEMENTSEDITOR_H
