/** \file factory.h
\brief Define the factory, to create instance of the interface
\author alpha_one_x86
\licence GPL3, see the file COPYING */

#ifndef FACTORY_H
#define FACTORY_H

#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QCloseEvent>
#include <QFile>
#include <QIcon>

#include "../../../interface/PluginInterface_Themes.h"
#include "ui_options.h"
#include "interface.h"
#include "Environment.h"

namespace Ui {
	class options;
}

/// \brief Define the factory, to create instance of the interface
class Factory : public PluginInterface_ThemesFactory
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "first-world.info.ultracopier.PluginInterface.ThemesFactory/0.4.0.0" FILE "plugin.json")
	Q_INTERFACES(PluginInterface_ThemesFactory)
public:
	Factory();
	~Factory();
	/// \brief to return the instance of the copy engine
	PluginInterface_Themes * getInstance();
	/// \brief set the resources, to store options, to have facilityInterface
	void setResources(OptionInterface * optionsEngine,const QString &writePath,const QString &pluginPath,FacilityInterface * facilityEngine,const bool &portableVersion);
	/// \brief to get the default options widget
	QWidget * options();
	/// \brief to get a resource icon
	QIcon getIcon(const QString &fileName);
private slots:
	void checkBoxShowSpeedHaveChanged(bool toggled);
	void checkBoxStartWithMoreButtonPushedHaveChanged(bool toggled);
public slots:
	void resetOptions();
	void newLanguageLoaded();
private:
	OptionInterface * optionsEngine;
	Ui::options *ui;
	QWidget *tempWidget;
	FacilityInterface * facilityEngine;
signals:
	void reloadLanguage();
};

#endif // FACTORY_H
