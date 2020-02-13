#ifndef CONSTITUENCYWINDOW_H
#define CONSTITUENCYWINDOW_H

#include <QWidget>

namespace Ui {
class ConstituencyWindow;
}

class ConstituencyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConstituencyWindow(QWidget *parent = nullptr);
    ~ConstituencyWindow();

private:
    Ui::ConstituencyWindow *ui;
};

#endif // CONSTITUENCYWINDOW_H
