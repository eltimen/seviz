#pragma once
#include <QObject>
#include <QAction>
#include <QString>
#include <QDockWidget>

class AbstractModule;

struct Feature {

public:
	Feature() = delete;

	Feature(const QString& id,
		const QIcon& icon,
		const QString& tooltipText,
		QDockWidget* window,
		AbstractModule* owner,
		bool affectsView = false) :
		id(id), icon(icon), text(tooltipText), window(window), owner(owner), affectsView(affectsView) 
	{}

	~Feature() = default;

	QString id;
	QIcon icon;
	QString text;

	AbstractModule* owner;

	QDockWidget* window = nullptr;
	bool affectsView = false;
};
