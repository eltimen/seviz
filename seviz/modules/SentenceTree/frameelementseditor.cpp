#include "frameelementseditor.h"
#include "ui_frameelementseditor.h"

#include <QLabel>
#include <QRadioButton>
#include <QComboBox>
#include "framenet.h"
#include "frameelement.h"

FrameElementsEditor::FrameElementsEditor(QWidget* parent, Frame* frame) :
    QDialog(parent),
    ui(new Ui::FrameElementsEditor),
    m_frame(frame)
{
    ui->setupUi(this);
    fillWindow();

    connect(ui->saveButton, &QPushButton::clicked, this, &FrameElementsEditor::onSave);
}

FrameElementsEditor::~FrameElementsEditor()
{
    delete ui;
}

void FrameElementsEditor::onSave() {
    std::map<WordRange, FrameElement>& elements();

    QLayout* elems = ui->feGroupBox->layout();
    for (const QGroupBox* gb : ui->feGroupBox->findChildren<QGroupBox*>()) {
        QRadioButton* wordRadioButton = ui->feGroupBox->findChild<QRadioButton*>("wordRadioButton");
        QRadioButton* subFrameRadioButton = ui->feGroupBox->findChild<QRadioButton*>("subFrameRadioButton");
        QComboBox* wordComboBox = ui->feGroupBox->findChild<QComboBox*>("wordComboBox");
        QComboBox* subFrameComboBox = ui->feGroupBox->findChild<QComboBox*>("subFrameComboBox");

        if (gb->isChecked() && wordRadioButton->isChecked()) {
            FrameElement fe(gb->title(), { m_words[wordComboBox->currentIndex()] });
            m_frame->setElement(fe);
        }
        //
    }

    this->accept();
}

void FrameElementsEditor::fillWindow() {
    // слова фрейма сверху окна
    QStringList words;
    QLayout* wordsLayout = ui->wordsGroupBox->layout();
    for (const Word& w : m_frame->words()) {
        QLabel* l = new QLabel(w.text(), ui->wordsGroupBox);
        wordsLayout->addWidget(l);
        if (w == m_frame->lu()) {
            l->setText("<b>" + w.text() + "</b>");
        } else {
            words << w.text();
            m_words.emplace_back(w);
        }
    }
    wordsLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    // FE
    QStringList feNames = m_frame->elementsList();
    for (QString& fe : feNames) {
        QGroupBox* gb = new QGroupBox(fe, ui->feGroupBox);
        gb->setCheckable(true);
        gb->setChecked(false);
        QHBoxLayout* layout = new QHBoxLayout(gb);

        QVBoxLayout* verticalLayout = new QVBoxLayout();
        QRadioButton* wordRadioButton = new QRadioButton("Слово:", gb);
        wordRadioButton->setObjectName("wordRadioButton");
        wordRadioButton->setChecked(true);
        verticalLayout->addWidget(wordRadioButton);
        QComboBox* wordComboBox = new QComboBox(gb);
        wordComboBox->setObjectName("wordComboBox");
        wordComboBox->addItems(words);
        verticalLayout->addWidget(wordComboBox);

        layout->addLayout(verticalLayout);

        QVBoxLayout* verticalLayout_2 = new QVBoxLayout();
        QRadioButton* subFrameRadioButton = new QRadioButton("Подфрейм:", gb);
        subFrameRadioButton->setObjectName("subFrameRadioButton");
        verticalLayout_2->addWidget(subFrameRadioButton);
        QComboBox* subFrameComboBox = new QComboBox(gb);
        subFrameComboBox->setObjectName("subFrameComboBox");
        subFrameComboBox->addItem(QString("---"));
        subFrameComboBox->setEnabled(false);
        verticalLayout_2->addWidget(subFrameComboBox);

        layout ->addLayout(verticalLayout_2);
        ui->verticalLayout_3->addWidget(gb);

        connect(wordRadioButton, &QRadioButton::toggled, [=](bool ch) {
            subFrameComboBox->setEnabled(!ch);
        });
        connect(subFrameRadioButton, &QRadioButton::toggled, [=](bool ch) {
            wordComboBox->setEnabled(!ch);
        });

    }
    
}
