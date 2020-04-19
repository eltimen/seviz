#ifndef EDGETYPECHOOSEDIALOG_H
#define EDGETYPECHOOSEDIALOG_H

#include <QDialog>
#include "dependency.h"

namespace Ui {
class ChoosePaletteDialog;
}

class ChoosePaletteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChoosePaletteDialog(QWidget *parent, const QStringList& relationStrings, int maxRows=12);
    ~ChoosePaletteDialog();

    int getChoosedIndex() const;

    template <typename T>
    T getChoosedAsEnum() const {
        return static_cast<T>(m_result);
    }

private slots:
    void onTypeButtonClicked();

private:
    int m_result;

    Ui::ChoosePaletteDialog *ui;
};

#endif // EDGETYPECHOOSEDIALOG_H
