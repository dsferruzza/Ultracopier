/** \file CopyListener.h
\brief Define the copy listener
\author alpha_one_x86
\version 0.3
\date 2010
\licence GPL3, see the file COPYING */

#include "CopyListener.h"

#include <QRegularExpression>

CopyListener::CopyListener(OptionDialog *optionDialog)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    this->optionDialog=optionDialog;
    pluginLoader=new PluginLoader(optionDialog);
    //load the options
    tryListen=false;
    QList<QPair<QString, QVariant> > KeysList;
        KeysList.append(qMakePair(QString("CatchCopyAsDefault"),QVariant(true)));
    options->addOptionGroup("CopyListener",KeysList);
    plugins->lockPluginListEdition();
    QList<PluginsAvailable> list=plugins->getPluginsByCategory(PluginType_Listener);
    connect(this,&CopyListener::previouslyPluginAdded,			this,&CopyListener::onePluginAdded,Qt::QueuedConnection);
    connect(plugins,&PluginsManager::onePluginAdded,			this,&CopyListener::onePluginAdded,Qt::QueuedConnection);
    connect(plugins,&PluginsManager::onePluginWillBeRemoved,		this,&CopyListener::onePluginWillBeRemoved,Qt::DirectConnection);
    connect(plugins,&PluginsManager::pluginListingIsfinish,			this,&CopyListener::allPluginIsloaded,Qt::QueuedConnection);
    connect(pluginLoader,&PluginLoader::pluginLoaderReady,			this,&CopyListener::pluginLoaderReady);
    foreach(PluginsAvailable currentPlugin,list)
        emit previouslyPluginAdded(currentPlugin);
    plugins->unlockPluginListEdition();
    last_state=Ultracopier::NotListening;
    last_have_plugin=false;
    last_inWaitOfReply=false;
}

CopyListener::~CopyListener()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    QList<PluginsAvailable> list=plugins->getPluginsByCategory(PluginType_Listener);
    foreach(PluginsAvailable currentPlugin,list)
        onePluginWillBeRemoved(currentPlugin);
    delete pluginLoader;
}

void CopyListener::resendState()
{
    if(plugins->allPluginHaveBeenLoaded())
    {
        sendState(true);
        pluginLoader->resendState();
    }
}

void CopyListener::onePluginAdded(const PluginsAvailable &plugin)
{
    if(plugin.category!=PluginType_Listener)
        return;
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"try load: "+plugin.path+PluginsManager::getResolvedPluginName("listener"));
    //setFileName
    QPluginLoader *pluginOfPluginLoader=new QPluginLoader(plugin.path+PluginsManager::getResolvedPluginName("listener"));
    QObject *pluginInstance = pluginOfPluginLoader->instance();
    if(pluginInstance)
    {
        PluginInterface_Listener *listen = qobject_cast<PluginInterface_Listener *>(pluginInstance);
        //check if found
        int index=0;
        while(index<pluginList.size())
        {
            if(pluginList.at(index).listenInterface==listen)
            {
                ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Warning,QString("Plugin already found %1 for %2").arg(pluginList.at(index).path).arg(plugin.path));
                pluginOfPluginLoader->unload();
                return;
            }
            index++;
        }
        if(listen)
        {
            ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"Plugin correctly loaded");
            #ifdef ULTRACOPIER_DEBUG
            connect(listen,&PluginInterface_Listener::debugInformation,this,&CopyListener::debugInformation);
            #endif // ULTRACOPIER_DEBUG
            connect(listen,&PluginInterface_Listener::newCopyWithoutDestination,		this,&CopyListener::newPluginCopyWithoutDestination);
            connect(listen,&PluginInterface_Listener::newCopy,				this,&CopyListener::newPluginCopy);
            connect(listen,&PluginInterface_Listener::newMoveWithoutDestination,		this,&CopyListener::newPluginMoveWithoutDestination);
            connect(listen,&PluginInterface_Listener::newMove,				this,&CopyListener::newPluginMove);
            PluginListener newPluginListener;
            newPluginListener.listenInterface	= listen;
            newPluginListener.pluginLoader		= pluginOfPluginLoader;
            newPluginListener.path			= plugin.path+PluginsManager::getResolvedPluginName("listener");
            newPluginListener.state			= Ultracopier::NotListening;
            newPluginListener.inWaitOfReply		= false;
            newPluginListener.options=new LocalPluginOptions("Listener-"+plugin.name);
            newPluginListener.listenInterface->setResources(newPluginListener.options,plugin.writablePath,plugin.path,ULTRACOPIER_VERSION_PORTABLE_BOOL);
            optionDialog->addPluginOptionWidget(PluginType_Listener,plugin.name,newPluginListener.listenInterface->options());
            connect(languages,&LanguagesManager::newLanguageLoaded,newPluginListener.listenInterface,&PluginInterface_Listener::newLanguageLoaded);
            pluginList << newPluginListener;
            connect(pluginList.last().listenInterface,&PluginInterface_Listener::newState,this,&CopyListener::newState);
            if(tryListen)
            {
                pluginList.last().inWaitOfReply=true;
                listen->listen();
            }
        }
        else
            ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Warning,"unable to cast the plugin: "+pluginOfPluginLoader->errorString());
    }
    else
        ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Warning,"unable to load the plugin: "+pluginOfPluginLoader->errorString());
}

#ifdef ULTRACOPIER_DEBUG
void CopyListener::debugInformation(const Ultracopier::DebugLevel &level, const QString& fonction, const QString& text, const QString& file, const int& ligne)
{
    DebugEngine::addDebugInformationStatic(level,fonction,text,file,ligne,"Listener plugin");
}
#endif // ULTRACOPIER_DEBUG

bool CopyListener::oneListenerIsLoaded()
{
    return (pluginList.size()>0);
}

void CopyListener::onePluginWillBeRemoved(const PluginsAvailable &plugin)
{
    if(plugin.category!=PluginType_Listener)
        return;
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"unload the current plugin");
    int indexPlugin=0;
    while(indexPlugin<pluginList.size())
    {
        if((plugin.path+PluginsManager::getResolvedPluginName("listener"))==pluginList.at(indexPlugin).path)
        {
            int index=0;
            while(index<copyRunningList.size())
            {
                if(copyRunningList.at(index).listenInterface==pluginList.at(indexPlugin).listenInterface)
                    copyRunningList[index].listenInterface=NULL;
                index++;
            }
            pluginList.at(indexPlugin).listenInterface->close();
            delete pluginList.at(indexPlugin).listenInterface;
            pluginList.at(indexPlugin).pluginLoader->unload();
            delete pluginList.at(indexPlugin).options;
            pluginList.removeAt(indexPlugin);
            sendState();
            return;
        }
        indexPlugin++;
    }
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Warning,"not found");
}

void CopyListener::newState(const Ultracopier::ListeningState &state)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    PluginInterface_Listener *temp=qobject_cast<PluginInterface_Listener *>(QObject::sender());
    if(temp==NULL)
    {
        ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Critical,QString("listener not located!"));
        return;
    }
    int index=0;
    while(index<pluginList.size())
    {
        if(temp==pluginList.at(index).listenInterface)
        {
            pluginList[index].state=state;
            pluginList[index].inWaitOfReply=false;
            ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,QString("new state for the plugin %1: %2").arg(index).arg(state));
            sendState(true);
            return;
        }
        index++;
    }
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Critical,QString("listener not found!"));
}

void CopyListener::listen()
{
    tryListen=true;
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    int index=0;
    while(index<pluginList.size())
    {
        pluginList[index].inWaitOfReply=true;
        pluginList.at(index).listenInterface->listen();
        index++;
    }
    pluginLoader->load();
}

void CopyListener::close()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    tryListen=false;
    pluginLoader->unload();
    int index=0;
    while(index<pluginList.size())
    {
        pluginList[index].inWaitOfReply=true;
        pluginList.at(index).listenInterface->close();
        index++;
    }
    copyRunningList.clear();
}

QStringList CopyListener::stripSeparator(QStringList sources)
{
    int index=0;
    while(index<sources.size())
    {
        sources[index].remove(QRegularExpression("[\\\\/]+$"));
        index++;
    }
    return sources;
}

/** new copy without destination have been pased by the CLI */
void CopyListener::copyWithoutDestination(QStringList sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    emit newCopyWithoutDestination(incrementOrderId(),QStringList() << "file",stripSeparator(sources));
}

/** new copy with destination have been pased by the CLI */
void CopyListener::copy(QStringList sources,QString destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    emit newCopy(incrementOrderId(),QStringList() << "file",stripSeparator(sources),"file",destination);
}

/** new move without destination have been pased by the CLI */
void CopyListener::moveWithoutDestination(QStringList sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    emit newMoveWithoutDestination(incrementOrderId(),QStringList() << "file",stripSeparator(sources));
}

/** new move with destination have been pased by the CLI */
void CopyListener::move(QStringList sources,QString destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    emit newMove(incrementOrderId(),QStringList() << "file",stripSeparator(sources),"file",destination);
}

void CopyListener::copyFinished(const quint32 & orderId,const bool &withError)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    int index=0;
    while(index<copyRunningList.size())
    {
        if(orderId==copyRunningList.at(index).orderId)
        {
            orderList.removeAll(orderId);
            if(copyRunningList.at(index).listenInterface!=NULL)
                copyRunningList.at(index).listenInterface->transferFinished(copyRunningList.at(index).pluginOrderId,withError);
            copyRunningList.removeAt(index);
            return;
        }
        index++;
    }
}

void CopyListener::copyCanceled(const quint32 & orderId)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    int index=0;
    while(index<copyRunningList.size())
    {
        if(orderId==copyRunningList.at(index).orderId)
        {
            orderList.removeAll(orderId);
            if(copyRunningList.at(index).listenInterface!=NULL)
                copyRunningList.at(index).listenInterface->transferCanceled(copyRunningList.at(index).pluginOrderId);
            copyRunningList.removeAt(index);
            return;
        }
        index++;
    }
}

void CopyListener::newPluginCopyWithoutDestination(const quint32 &orderId,const QStringList &sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"sources: "+sources.join(";"));
    PluginInterface_Listener *plugin		= qobject_cast<PluginInterface_Listener *>(sender());
    CopyRunning newCopyInformation;
    newCopyInformation.listenInterface	= plugin;
    newCopyInformation.pluginOrderId	= orderId;
    newCopyInformation.orderId		= incrementOrderId();
    copyRunningList << newCopyInformation;
    emit newCopyWithoutDestination(orderId,QStringList() << "file",stripSeparator(sources));
}

void CopyListener::newPluginCopy(const quint32 &orderId,const QStringList &sources,const QString &destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"sources: "+sources.join(";")+", destination: "+destination);
    PluginInterface_Listener *plugin		= qobject_cast<PluginInterface_Listener *>(sender());
    CopyRunning newCopyInformation;
    newCopyInformation.listenInterface	= plugin;
    newCopyInformation.pluginOrderId	= orderId;
    newCopyInformation.orderId		= incrementOrderId();
    copyRunningList << newCopyInformation;
    emit newCopy(orderId,QStringList() << "file",stripSeparator(sources),"file",destination);
}

void CopyListener::newPluginMoveWithoutDestination(const quint32 &orderId,const QStringList &sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"sources: "+sources.join(";"));
    PluginInterface_Listener *plugin		= qobject_cast<PluginInterface_Listener *>(sender());
    CopyRunning newCopyInformation;
    newCopyInformation.listenInterface	= plugin;
    newCopyInformation.pluginOrderId	= orderId;
    newCopyInformation.orderId		= incrementOrderId();
    copyRunningList << newCopyInformation;
    emit newMoveWithoutDestination(orderId,QStringList() << "file",stripSeparator(sources));
}

void CopyListener::newPluginMove(const quint32 &orderId,const QStringList &sources,const QString &destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"sources: "+sources.join(";")+", destination: "+destination);
    PluginInterface_Listener *plugin		= qobject_cast<PluginInterface_Listener *>(sender());
    CopyRunning newCopyInformation;
    newCopyInformation.listenInterface	= plugin;
    newCopyInformation.pluginOrderId	= orderId;
    newCopyInformation.orderId		= incrementOrderId();
    copyRunningList << newCopyInformation;
    emit newMove(orderId,QStringList() << "file",stripSeparator(sources),"file",destination);
}

quint32 CopyListener::incrementOrderId()
{
    do
    {
        nextOrderId++;
        if(nextOrderId>2000000)
            nextOrderId=0;
    } while(orderList.contains(nextOrderId));
    return nextOrderId;
}

void CopyListener::allPluginIsloaded()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"with value: "+QString::number(pluginList.size()>0));
    sendState(true);
}

void CopyListener::sendState(bool force)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,QString("start, pluginList.size(): %1, force: %2").arg(pluginList.size()).arg(force));
    Ultracopier::ListeningState current_state=Ultracopier::NotListening;
    bool found_not_listen=false,found_listen=false,found_inWaitOfReply=false;
    int index=0;
    while(index<pluginList.size())
    {
        if(current_state==Ultracopier::NotListening)
        {
            if(pluginList.at(index).state==Ultracopier::SemiListening)
                current_state=Ultracopier::SemiListening;
            else if(pluginList.at(index).state==Ultracopier::NotListening)
                found_not_listen=true;
            else if(pluginList.at(index).state==Ultracopier::FullListening)
                found_listen=true;
        }
        if(pluginList.at(index).inWaitOfReply)
            found_inWaitOfReply=true;
        index++;
    }
    if(current_state==Ultracopier::NotListening)
    {
        if(found_not_listen && found_listen)
            current_state=Ultracopier::SemiListening;
        else if(found_not_listen)
            current_state=Ultracopier::NotListening;
        else if(!found_not_listen && found_listen)
            current_state=Ultracopier::FullListening;
        else
            current_state=Ultracopier::SemiListening;
    }
    bool have_plugin=pluginList.size()>0;
    if(force || current_state!=last_state || have_plugin!=last_have_plugin || found_inWaitOfReply!=last_inWaitOfReply)
    {
        ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,QString("send listenerReady(%1,%2,%3)").arg(current_state).arg(have_plugin).arg(found_inWaitOfReply));
        emit listenerReady(current_state,have_plugin,found_inWaitOfReply);
    }
    else
        ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,QString("Skip the signal sending"));
    last_state=current_state;
    last_have_plugin=have_plugin;
    last_inWaitOfReply=found_inWaitOfReply;
}
