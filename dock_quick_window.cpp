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
}

DockQuickWindow::~DockQuickWindow()
{
}
void DockQuickWindow::destroyed(QObject *)
{
    qDebug() << "HEHEH Destroy" << winId();
}

void DockApplet::setMenu(const QString& m)
{
    m_menu = m;
    Q_EMIT menuChanged(m);
    m_dbus_proxyer->setData("menu", m);
}
void DockApplet::setIcon(const QString& v)
{
    m_icon = v;
    Q_EMIT iconChanged(v);
    m_dbus_proxyer->setData("icon", v);
}
void DockApplet::setTitle(const QString& v)
{
    m_title= v;
    Q_EMIT titleChanged(v);
    m_dbus_proxyer->setData("title", v);
}
void DockApplet::setStatus(const qint32 v)
{
    m_status = v;
    Q_EMIT statusChanged(v);
    m_dbus_proxyer->setData("status", QString::number(v));
}
void DockApplet::setWindow(DockQuickWindow* w) 
{
    m_dbus_proxyer->setData("app-xids", QString("[{\"Xid\":%1,\"Title\":\"\"}]").arg(w->winId()));
    w->show();
}

DockApplet::DockApplet(QQuickItem *parent)
    :QQuickItem(parent)
{
    this->m_dbus_proxyer= new DockAppletDBus(this);
}

DockApplet::~DockApplet()
{
    delete this->m_dbus_proxyer;
}


DockAppletDBus::DockAppletDBus(DockApplet* parent) :
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    qDBusRegisterMetaType<StringMap>();
    QString id  = "xx" + parent->id();
    QDBusConnection::sessionBus().registerService("dde.dock.entry.Applet" + id);
    qDebug() << "Register:" << QDBusConnection::sessionBus().registerObject("/dde/dock/entry/v1/Applet" + id, parent);
}
