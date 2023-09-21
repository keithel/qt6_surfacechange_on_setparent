#include "eventlurker.h"
#include <QWidget>
#include <QEvent>
#include <QPlatformSurfaceEvent>
#include <QWindow>
#include <QString>
#include <QDebug>

EventLurker::EventLurker(QObject *parent)
    : QObject{parent}
{

}

bool EventLurker::eventFilter(QObject *watched, QEvent *event)
{
    QWidget* watchedWidget = qobject_cast<QWidget *>(watched);
    auto watchedWindow = watchedWidget->windowHandle();
    switch (event->type())
    {
    case QEvent::PlatformSurface:
    {
        auto *pe = static_cast<QPlatformSurfaceEvent *>(event);
        QString seTypeStr = pe->surfaceEventType() ? "created" : "about to be destroyed";
        qDebug().noquote() << watched->objectName() << "surface type" << watchedWindow->surfaceType() << seTypeStr;
    }
    break;
    case QEvent::Show:
        if(watchedWindow)
            qDebug().noquote() << watched->objectName() << "with surface type" << watchedWindow->surfaceType() << "about to be shown";
        break;
    default:
        break;
    }

    return false;
}
