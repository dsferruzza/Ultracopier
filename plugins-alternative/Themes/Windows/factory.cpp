/** \file factory.cpp
\brief Define the factory core
\author alpha_one_x86
\version 0.3
\date 2010 */

#include "factory.h"

QRegularExpression Factory::slashEnd;
QRegularExpression Factory::isolateName;

Factory::Factory()
{
    slashEnd=QRegularExpression("/$");
    isolateName=QRegularExpression("^.*/([^/]+)$");
}

PluginInterface_Themes * Factory::getInstance()
{
    Themes * newInterface=new Themes(facilityEngine);
    connect(this,&Factory::reloadLanguage,newInterface,&Themes::newLanguageLoaded);
    return newInterface;
}

void Factory::setResources(OptionInterface * options,const QString &writePath,const QString &pluginPath,FacilityInterface * facilityEngine,const bool &portableVersion)
{
    Q_UNUSED(options)
    Q_UNUSED(writePath)
    Q_UNUSED(pluginPath)
    this->facilityEngine=facilityEngine;
    Q_UNUSED(portableVersion)
}

QWidget * Factory::options()
{
        return NULL;
}

QIcon Factory::getIcon(const QString &fileName)
{
    if(fileName=="SystemTrayIcon/exit.png")
    {
        QIcon tempIcon=QIcon::fromTheme("application-exit");
        if(!tempIcon.isNull())
            return tempIcon;
    }
    if(fileName=="SystemTrayIcon/add.png")
    {
        QIcon tempIcon=QIcon::fromTheme("list-add");
        if(!tempIcon.isNull())
            return tempIcon;
    }
    if(fileName=="SystemTrayIcon/informations.png")
    {
        QIcon tempIcon=QIcon::fromTheme("help-about");
        if(!tempIcon.isNull())
            return tempIcon;
    }
    if(fileName=="SystemTrayIcon/options.png")
    {
        QIcon tempIcon=QIcon::fromTheme("applications-system");
        if(!tempIcon.isNull())
            return tempIcon;
    }
    return QIcon(":/resources/"+fileName);
}

void Factory::resetOptions()
{
}

void Factory::newLanguageLoaded()
{
    emit reloadLanguage();
}
