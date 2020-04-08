#include "edgetypechoosedialog.h"
#include "ui_edgetypechoosedialog.h"

#include <QPushButton>
#include <QGridLayout>

EdgeTypeChooseDialog::EdgeTypeChooseDialog(QWidget *parent, const QStringList& relationStrings)
    : QDialog(parent, Qt::Tool),
      ui(new Ui::EdgeTypeChooseDialog)
{
    ui->setupUi(this);

    int column = 0;
    int row = 0;
    for (int i = 0; i < relationStrings.size(); ++i) {
        QPushButton* button = new QPushButton(relationStrings[i]);
        connect(button, &QPushButton::clicked, this, &EdgeTypeChooseDialog::onTypeButtonClicked);
        button->setProperty("st_dep_index", QVariant(i));
        ui->gridLayout->addWidget(button, row, column);
        ++row;
        if (row > 12) {
            row = 0;
            ++column;
        }
    }
}

EdgeTypeChooseDialog::~EdgeTypeChooseDialog() {
    delete ui;
}

DependencyRelation EdgeTypeChooseDialog::getChoosedDepType() const {
    return m_res;
}

void EdgeTypeChooseDialog::onTypeButtonClicked() {
    m_res = static_cast<DependencyRelation>(sender()->property("st_dep_index").toInt());
    this->accept();
}
