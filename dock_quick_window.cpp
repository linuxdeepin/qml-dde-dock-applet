#include "dock_quick_window.h"
#include <QDBusConnection>
#include <QDBusMetaType>

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

void DockQuickWindow::setMenu(const QString& m)
{
    m_menu = m;
    Q_EMIT menuChanged(m);
    applet->setData("menu", m);
}
void DockQuickWindow::setIcon(const QString& v)
{
    m_icon = v;
    Q_EMIT iconChanged(v);
    applet->setData("icon", v);
}
void DockQuickWindow::setTitle(const QString& v)
{
    QQuickWindow::setTitle(v);
    m_title= v;
    Q_EMIT titleChanged(v);
    applet->setData("title", v);
}
void DockQuickWindow::setStatus(const qint32 v)
{
    m_status = v;
    Q_EMIT statusChanged(v);
    applet->setData("status", QString::number(v));
}



DockApplet::DockApplet(DockQuickWindow* parent, int xid) :
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    qDBusRegisterMetaType<StringMap>();
    QString id  = QString::number(xid);
    QDBusConnection::sessionBus().registerService("dde.dock.entry.Applet" + id);
    qDebug() << "Register:" << QDBusConnection::sessionBus().registerObject("/dde/dock/entry/v1/Applet" + id, parent);

    setData("app-xids", QString("[{\"Xid\":%1,\"Title\":\"\"}]").arg(id));
    qDebug() << "CreateProxyer for:" <<  xid;
}

