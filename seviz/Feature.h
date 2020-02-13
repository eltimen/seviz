#pragma once
#include <QObject>
#include <QAction>
#include <QString>
#include <QDockWidget>

class AbstractModule;

struct Feature {

public:
	Feature() = delete;

	Feature(const QString& name,
		const QIcon& icon,
		QDockWidget* window,
		AbstractModule* owner,
		bool affectsView = false) :
		name(name), icon(icon), window(window), owner(owner), affectsView(affectsView) 
	{}

	~Feature() = default;

	QString name;
	QIcon icon;

	AbstractModule* owner;

	QDockWidget* window = nullptr;
	bool affectsView = false;
};
