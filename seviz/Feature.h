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
		bool affectsView = false,
		Qt::DockWidgetArea dockLocation = Qt::BottomDockWidgetArea) :
		name(name), icon(icon), window(window), owner(owner), affectsView(affectsView), dockLocation(dockLocation)
	{}

	~Feature() = default;

	inline bool operator== (const Feature& other) const {
		return name == other.name && owner == other.owner;
	}

	inline friend bool operator< (const Feature& first, const Feature& second) {
		return first.name < second.name && first.owner < second.owner;
	}

	QString name;
	QIcon icon;

	AbstractModule* owner;

	QDockWidget* window = nullptr;
	Qt::DockWidgetArea dockLocation;
	bool affectsView = false;
};
