#ifndef ENGINETESTWIDGET_H
#define ENGINETESTWIDGET_H

#include <QWidget>
#include "ModuleManager.h"

namespace Ui {
class EngineTestWidget;
}

class EngineTestWidget : public QWidget
{
    friend class EngineTest;

    Q_OBJECT

public:
    explicit EngineTestWidget(EngineTest& facade);
    ~EngineTestWidget();

    void setText(const QString& str);

private:
    Ui::EngineTestWidget *ui;
    ModuleManager* m_engine = nullptr;
    EngineTest& m_main;

};

#endif // ENGINETESTWIDGET_H
