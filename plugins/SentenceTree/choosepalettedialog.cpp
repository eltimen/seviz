#include "choosepalettedialog.h"
#include "ui_choosepalettedialog.h"

#include <QPushButton>
#include <QGridLayout>

ChoosePaletteDialog::ChoosePaletteDialog(QWidget *parent, const QStringList& relationStrings, int maxRows)
    : QDialog(parent, Qt::Tool),
      ui(new Ui::ChoosePaletteDialog)
{
    ui->setupUi(this);

    int column = 0;
    int row = 0;
    for (int i = 0; i < relationStrings.size(); ++i) {
        QPushButton* button = new QPushButton(relationStrings[i]);
        connect(button, &QPushButton::clicked, this, &ChoosePaletteDialog::onTypeButtonClicked);
        button->setProperty("st_dep_index", QVariant(i));
        ui->gridLayout->addWidget(button, row, column);
        ++row;
        if (row > maxRows) {
            row = 0;
            ++column;
        }
    }
}

ChoosePaletteDialog::~ChoosePaletteDialog() {
    delete ui;
}

int ChoosePaletteDialog::getChoosedIndex() const {
    return m_result;
}

void ChoosePaletteDialog::onTypeButtonClicked() {
    m_result = sender()->property("st_dep_index").toInt();
    this->accept();
}
