#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include "BookModels.h"
#include "SentenceTree.h"
#include "dependency.h"

STWindow::STWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::STWindow),
    m_webchannel(new QWebChannel(this))
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

void STWindow::renderDependencies(const DependencyTree& tree) {
    QString docData = QStringLiteral(R"(
        { 
            "source_files": ["ann", "txt"],
            "text": "%1"
        }       
    )").arg(m_sentenceText);

    ui->dependencyView->page()->runJavaScript("docData=" + docData + "; render();");
}
