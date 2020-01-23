#ifndef SYNTAXTREEWINDOW_H
#define SYNTAXTREEWINDOW_H

#include <QWidget>

namespace Ui {
class SyntaxTreeWindow;
}

class SyntaxTreeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SyntaxTreeWindow(QWidget *parent = nullptr);
    ~SyntaxTreeWindow();

private:
    Ui::SyntaxTreeWindow *ui;
};

#endif // SYNTAXTREEWINDOW_H
