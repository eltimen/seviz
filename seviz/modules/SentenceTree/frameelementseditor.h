#ifndef FRAMEELEMENTSEDITOR_H
#define FRAMEELEMENTSEDITOR_H

#include <QDialog>

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

private:
    Ui::FrameElementsEditor *ui;
    Frame* m_frame;

    void fillWindow();
};

#endif // FRAMEELEMENTSEDITOR_H
