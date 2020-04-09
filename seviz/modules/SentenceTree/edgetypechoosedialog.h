#ifndef EDGETYPECHOOSEDIALOG_H
#define EDGETYPECHOOSEDIALOG_H

#include <QDialog>
#include "dependency.h"

namespace Ui {
class EdgeTypeChooseDialog;
}

class EdgeTypeChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EdgeTypeChooseDialog(QWidget *parent, const QStringList& relationStrings);
    ~EdgeTypeChooseDialog();

    DependencyRelation getChoosedDepType() const;

private slots:
    void onTypeButtonClicked();

private:
    DependencyRelation m_res;

    Ui::EdgeTypeChooseDialog *ui;
};

#endif // EDGETYPECHOOSEDIALOG_H
