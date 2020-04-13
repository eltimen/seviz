#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include <QMessageBox>
#include "BookModels.h"
#include "SentenceTree.h"
#include "constituency.h"
#include "dependency.h"
#include "edgetypechoosedialog.h"

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
}
void STWindow::onDepCreateEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    if (tree.canInsertRelation(from, to)) {
        QScopedPointer<EdgeTypeChooseDialog> chooser(new EdgeTypeChooseDialog(this, DependencyRelationStr));
        if (chooser->exec()) {
            DependencyRelation rel = chooser->getChoosedDepType();
            if (tree.canInsertRelation(from, to)) {
                tree.insert(from, to, rel);
                renderDependencies(tree);
            }
        }
    } else {
        QMessageBox::critical(this, "Ошибка", "Дерево зависимостей не может содержать циклов или параллельных связей");
    }
}

void STWindow::onDepRemoveEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    tree.remove(from, to);
    renderDependencies(tree);
}

void STWindow::onDepChangeEdgeType(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;

    QScopedPointer<EdgeTypeChooseDialog> chooser(new EdgeTypeChooseDialog(this, DependencyRelationStr));
    if (chooser->exec()) {
        DependencyRelation rel = chooser->getChoosedDepType();
        tree.change(from, to, rel);
        // TODO может лучше перерендеривать на уровне JS? например, через forceRedraw() или просто скопипастить код из renderDependencies()
        renderDependencies(tree);
    }
}
void STWindow::renderConstituency(const ConstituencyTree& tree) {
    QString data = tree.toTreantJson();
    ui->constituencyView->page()->runJavaScript("var constituency = " + data + "; render(constituency);");
}
void STWindow::renderDependencies(const DependencyTree& tree) {
    QString docData = tree.toBratJson();
    ui->dependencyView->page()->runJavaScript("docData=" + docData + "; render();");
}
