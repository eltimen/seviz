#ifndef DEPENDENCYWINDOW_H
#define DEPENDENCYWINDOW_H

#include <QWidget>

namespace Ui {
class DependencyWindow;
}

class DependencyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DependencyWindow(QWidget *parent = nullptr);
    ~DependencyWindow();

private:
    Ui::DependencyWindow *ui;
};

#endif // DEPENDENCYWINDOW_H
