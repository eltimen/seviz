#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include "BookModels.h"

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
    ui->idLabel->setText(QStringLiteral("ID: %1").arg(sent.id()));
}
