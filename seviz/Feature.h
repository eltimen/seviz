#pragma once
#include <QObject>
#include <QAction>
#include <QString>
#include <QDockWidget>
#include <QString>

class ISevizPlugin;

struct Feature {

public:
	Feature() = delete;

	Feature(const QString& name,
		const QIcon& icon,
		QDockWidget* window,
		ISevizPlugin* owner,
		bool affectsView = false,
		QMenu* menu = nullptr,
		Qt::DockWidgetArea dockLocation = Qt::BottomDockWidgetArea) :
		m_name(name), m_icon(icon), m_window(window), m_menu(menu), m_owner(owner), m_affectsView(affectsView), m_dockLocation(dockLocation)
	{}

	~Feature() = default;

	//Feature(const Feature& other) = delete;

	inline bool operator== (const Feature& other) const {
		return m_name == other.m_name && m_owner == other.m_owner;
	}

	inline friend bool operator< (const Feature& first, const Feature& second) {
		return first.m_name < second.m_name && first.m_owner < second.m_owner;
	}

	const QString& name() const { return m_name; }
	const QIcon& icon() const { return m_icon; }
	const ISevizPlugin* owner() const { return m_owner; }
	QDockWidget* window() { return m_window; }
	QMenu* menu() const { return m_menu; }
	Qt::DockWidgetArea dockLocation() const { return m_dockLocation; }
	bool affectsView() const { return m_affectsView; }

	void setDockLocation(Qt::DockWidgetArea loc) { m_dockLocation = loc; }

private:

	QString m_name;
	QIcon m_icon;

	ISevizPlugin* m_owner;

	QMenu* m_menu = nullptr;
	QDockWidget* m_window = nullptr;
	Qt::DockWidgetArea m_dockLocation;
	bool m_affectsView = false;
};
