#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include <QMessageBox>
#include <QDebug>
#include "BookModels.h"
#include "SentenceTree.h"
#include "constituency.h"
#include "dependency.h"
#include "choosepalettedialog.h"

STWindow::STWindow(SentenceTree* core) :
    QWidget(nullptr),
    ui(new Ui::STWindow),
    m_webchannel(new QWebChannel(this)),
    m_core(core)
{
    ui->setupUi(this);

    m_webchannel->registerObject("core", this);

    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);  
    ui->dependencyView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->dependencyView->page()->setWebChannel(m_webchannel);
    ui->dependencyView->setUrl(QUrl("file:///SentenceTree_resources/dep.html"));

    ui->constituencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->constituencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true); 
    ui->constituencyView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->constituencyView->page()->setWebChannel(m_webchannel);
    ui->constituencyView->setUrl(QUrl("file:///SentenceTree_resources/constituency.html"));
}

STWindow::~STWindow()
{
    m_webchannel->deregisterObject(this);
    delete ui;
}

void STWindow::showSentence(const Sentence& sent, const SentenceData& data) {
    m_sentenceText.clear();
    for (const Word& w : sent) {
        m_sentenceText.append(w.text() + " ");
    }
    ui->idLabel->setText(QStringLiteral("ID: %1").arg(sent.id()));

    renderConstituency(data.constituency);
    renderDependencies(data.dependency); 
}// ------------------ constituency tree event handlers ---------------------
void STWindow::onConstituencyCreateNode(int from, int to) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    if (tree.canInsertNodeWithRange(from, to)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, ConstituencyLabelStr, 4));
        chooser->setWindowTitle("Выберите тип узла");
        if (chooser->exec()) {
            ConstituencyLabel label = chooser->getChoosedAsEnum<ConstituencyLabel>();
            tree.insert(std::make_pair(from, to), label);
            renderConstituency(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Дерево составляющих не может содержать пересекающиеся узлы");
    }

    qDebug() << tree.toBracedString();
}

void STWindow::onConstituencyChangeNodeType(int id) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    if (tree.canChangeOrDeleteNode(id)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, ConstituencyLabelStr, 4));
        chooser->setWindowTitle("Выберите тип узла");
        if (chooser->exec()) {
            ConstituencyLabel rel = static_cast<ConstituencyLabel>(chooser->getChoosedIndex());
            tree.change(id, rel);
            renderConstituency(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Нельзя менять тип узла корня или элементов предложения");
    }
}

void STWindow::onConstituencyDeleteNode(int id) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    if (tree.canChangeOrDeleteNode(id)) {
         tree.remove(id);
         renderConstituency(tree);
    } else {
        QMessageBox::warning(this, "Ошибка", "Нельзя удалить корень или элемент предложения");
    }
}

// ------------------ dependency tree event handlers ---------------------
void STWindow::onDepCreateEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    if (tree.canInsertRelation(from, to)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, DependencyRelationStr));
        if (chooser->exec()) {
            DependencyRelation rel = static_cast<DependencyRelation>(chooser->getChoosedIndex());
            tree.insert(from, to, rel);
            renderDependencies(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Дерево зависимостей не может содержать циклов или параллельных связей");
    }
}

void STWindow::onDepRemoveEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    tree.remove(from, to);
    renderDependencies(tree);
}

void STWindow::onDepChangeEdgeType(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;

    QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, DependencyRelationStr));
    if (chooser->exec()) {
        DependencyRelation rel = static_cast<DependencyRelation>(chooser->getChoosedIndex());
        tree.change(from, to, rel);
        // TODO может лучше перерендеривать на уровне JS? например, через forceRedraw() или просто скопипастить код из renderDependencies()
        renderDependencies(tree);
    }
}

// --------------------- render methods ----------------------------
void STWindow::renderConstituency(const ConstituencyTree& tree) {
    QString data = tree.toTreantJson();
    ui->constituencyView->page()->runJavaScript("var constituency = " + data + "; render(constituency);");
}
void STWindow::renderDependencies(const DependencyTree& tree) {
    QString docData = tree.toBratJson();
    ui->dependencyView->page()->runJavaScript("docData=" + docData + "; render();");
}
