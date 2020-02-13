#ifndef FRAMENETWINDOW_H
#define FRAMENETWINDOW_H

#include <QWidget>

namespace Ui {
class FramenetWindow;
}

class FramenetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FramenetWindow(QWidget *parent = nullptr);
    ~FramenetWindow();

private:
    Ui::FramenetWindow *ui;
};

#endif // FRAMENETWINDOW_H
