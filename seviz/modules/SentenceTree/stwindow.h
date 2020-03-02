#ifndef STWINDOW_H
#define STWINDOW_H

#include <QWidget>
#include <QWebEngineView>

namespace Ui {
class STWindow;
}

class STWindow : public QWidget
{
    Q_OBJECT

public:
    explicit STWindow(QWidget *parent = nullptr);
    ~STWindow();

private:
    Ui::STWindow *ui;

    QWebChannel* m_webchannel;
};

#endif // STWINDOW_H
