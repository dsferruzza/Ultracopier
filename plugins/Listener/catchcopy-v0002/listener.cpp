#include "listener.h"

CatchCopyPlugin::CatchCopyPlugin()
{
    server.setName(tr("Ultracopier"));
    connect(&server,&ServerCatchcopy::newCopyWithoutDestination,		this,&CatchCopyPlugin::copyWithoutDestination);
    connect(&server,&ServerCatchcopy::newCopy,                          this,&CatchCopyPlugin::copy);
    connect(&server,&ServerCatchcopy::newMoveWithoutDestination,		this,&CatchCopyPlugin::moveWithoutDestination);
    connect(&server,&ServerCatchcopy::newMove,                          this,&CatchCopyPlugin::move);
    connect(&server,&ServerCatchcopy::error,                            this,&CatchCopyPlugin::error);
    connect(&server,&ServerCatchcopy::clientName,                       this,&CatchCopyPlugin::clientName);
}

void CatchCopyPlugin::listen()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    if(server.listen())
        emit newState(Ultracopier::FullListening);
    else
        emit newState(Ultracopier::NotListening);
}

void CatchCopyPlugin::close()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    server.close();
    emit newState(Ultracopier::NotListening);
}

const QString CatchCopyPlugin::errorString()
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start");
    return server.errorString();
}

void CatchCopyPlugin::setResources(OptionInterface * options,const QString &writePath,const QString &pluginPath,const bool &portableVersion)
{
    Q_UNUSED(options);
    Q_UNUSED(writePath);
    Q_UNUSED(pluginPath);
    Q_UNUSED(portableVersion);
}

/// \brief to get the options widget, NULL if not have
QWidget * CatchCopyPlugin::options()
{
    return NULL;
}

void CatchCopyPlugin::transferFinished(const quint32 &orderId,const bool &withError)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start, orderId: "+QString::number(orderId)+", withError: "+QString::number(withError));
    server.copyFinished(orderId,withError);
}

void CatchCopyPlugin::transferCanceled(const quint32 &orderId)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Notice,"start, orderId: "+QString::number(orderId));
    server.copyCanceled(orderId);
}

/// \brief to reload the translation, because the new language have been loaded
void CatchCopyPlugin::newLanguageLoaded()
{
}

void CatchCopyPlugin::error(QString error)
{
    Q_UNUSED(error);
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Warning,"warning emited from Catchcopy lib: "+error);
}

void CatchCopyPlugin::clientName(quint32 client,QString name)
{
    Q_UNUSED(client);
    Q_UNUSED(name);
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Information,QString("clientName: %1, for the id: %2").arg(name).arg(client));
}

void CatchCopyPlugin::copyWithoutDestination(const quint32 &orderId,const QStringList &sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Information,QString("copyWithoutDestination(%1,%2)").arg(orderId).arg(sources.join(";")));
    emit newCopyWithoutDestination(orderId,sources);
}

void CatchCopyPlugin::copy(const quint32 &orderId,const QStringList &sources,const QString &destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Information,QString("copy(%1,%2,%3)").arg(orderId).arg(sources.join(";")).arg(destination));
    emit newCopy(orderId,sources,destination);
}

void CatchCopyPlugin::moveWithoutDestination(const quint32 &orderId,const QStringList &sources)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Information,QString("moveWithoutDestination(%1,%2)").arg(orderId).arg(sources.join(";")));
    emit newMoveWithoutDestination(orderId,sources);
}

void CatchCopyPlugin::move(const quint32 &orderId,const QStringList &sources,const QString &destination)
{
    ULTRACOPIER_DEBUGCONSOLE(Ultracopier::DebugLevel_Information,QString("move(%1,%2,%3)").arg(orderId).arg(sources.join(";")).arg(destination));
    emit newMove(orderId,sources,destination);
}
