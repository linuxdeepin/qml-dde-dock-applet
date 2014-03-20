#include "dock_quick_window.h"
#include <QDBusConnection>

DockQuickWindow::DockQuickWindow(QQuickWindow *parent):
    QQuickWindow(parent),
    firstShow(true)
{
    QSurfaceFormat sformat;
    sformat.setAlphaBufferSize(8);
    this->setFormat(sformat);
    this->setClearBeforeRendering(true);

    this->applet = new DockApplet(this, winId());
}

DockQuickWindow::~DockQuickWindow()
{
    delete this->applet;
}



DockApplet::DockApplet(DockQuickWindow* parent, int xid) :
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    QString id  = QString::number(xid);
    QDBusConnection::sessionBus().registerService("dde.dock.entry.Applet" + id);
    qDebug() << "Register:" << QDBusConnection::sessionBus().registerObject("/dde/dock/entry/Applet" + id, parent);

    QObject::connect(m_parent, SIGNAL(iconChanged(QString)), this, SIGNAL(iconChanged(QString)));
    QObject::connect(m_parent, SIGNAL(menuChanged(QString)), this, SIGNAL(menuChanged(QString)));
    QObject::connect(m_parent, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    QObject::connect(m_parent, SIGNAL(statusChanged(qint32)), this, SIGNAL(statusChanged(qint32)));

    qDebug() << "CreateProxyer for:" <<  xid;
}
