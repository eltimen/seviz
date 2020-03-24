#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include <QMessageBox>
#include "BookModels.h"
#include "SentenceTree.h"
#include "dependency.h"

STWindow::STWindow(SentenceTree* core) :
    QWidget(nullptr),
    ui(new Ui::STWindow),
    m_webchannel(new QWebChannel(this)),
    m_core(core)
{
    ui->setupUi(this);

    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    m_webchannel->registerObject(QStringLiteral("core"), this);
    ui->dependencyView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->dependencyView->page()->setWebChannel(m_webchannel);
    ui->dependencyView->setUrl(QUrl("file:///SentenceTree_resources/dep.html"));
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

    renderDependencies(data.dependency);
}
void STWindow::onDepCreateEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    DependencyRelation rel = nsubj; // TODO диалог выбора типа связи
    if (tree.insert(from, to, rel)) {
        renderDependencies(tree);
    } else {
        QMessageBox::critical(this, "Ошибка", "Дерево зависимостей не должно содержать циклов");
        renderDependencies(tree); // TODO после тестирования убрать 
    } 
}

void STWindow::onDepRemoveEdge(int from, int to) {
    QMessageBox::information(this, "Тест", "remove " + QString::number(from) + " " + QString::number(to));
    renderDependencies(m_core->currentSentenceData().dependency);
}
void STWindow::onDepChangeEdgeType(int from, int to) {
    QMessageBox::information(this, "Тест", "change " + QString::number(from) + " " + QString::number(to));
    renderDependencies(m_core->currentSentenceData().dependency);
}
void STWindow::renderDependencies(const DependencyTree& tree) {
    QString docData = tree.toBratJson();
    ui->dependencyView->page()->runJavaScript("docData=" + docData + "; render();");
}
