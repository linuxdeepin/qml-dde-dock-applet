#include "dock_quick_window.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QUuid>

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

void DockApplet::setMenu(DockMenu* m)
{
    if (m_menu) {
        disconnect(m_menu, SIGNAL(contentChanged(QString)), this, SLOT(setMenuContent(QString)));
    }
    m_menu = m;
    connect(m_menu, SIGNAL(contentChanged(QString)), this, SLOT(setMenuContent(QString)));
    setMenuContent(m_menu->content());
}
void DockApplet::handleMenuItem(qint32 id)
{
    if (m_menu) {
        Q_EMIT m_menu->activate(id);
    }
}

void DockApplet::setMenuContent(const QString &c)
{
    m_dbus_proxyer->setData("menu", c);
}

void DockApplet::setIcon(const QString& v)
{
    m_icon = v;
    Q_EMIT iconChanged(v);
    qDebug() << "icon Changed:" << v;
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

void DockApplet::setData(QString key, QString value)
{
    m_dbus_proxyer->setData(key, value);
}
void DockApplet::setWindow(DockQuickWindow* w) 
{
    m_window = w;
    m_dbus_proxyer->setData("app-xids", QString("[{\"Xid\":%1,\"Title\":\"\"}]").arg(w->winId()));
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
    m_id = QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
    QDBusConnection::sessionBus().registerService("dde.dock.entry.Applet" + m_id);
    qDebug() << "Register:" << QDBusConnection::sessionBus().registerObject("/dde/dock/entry/v1/Applet" + m_id, parent);
}
DockAppletDBus::~DockAppletDBus()
{
    QDBusConnection::sessionBus().unregisterService("dde.dock.entry.Applet" + m_id);
    qDebug() << "Unregister:" << "dde.dock.entry.Applet" + m_id;
}

void DockAppletDBus::HandleMenuItem(qint32 id)
{
    m_parent->handleMenuItem(id);
}

void DockAppletDBus::HandleDragDrop(qint32 x, qint32 y, const QString &data)
{
    Q_EMIT m_parent->dragdrop(x, y, data);
}

void DockAppletDBus::HandleDragEnter(qint32 x, qint32 y, const QString &data)
{
    Q_EMIT m_parent->dragenter(x, y, data);
}

void DockAppletDBus::HandleDragLeave(qint32 x, qint32 y, const QString &data)
{
    Q_EMIT m_parent->dragleave(x, y, data);
}

void DockAppletDBus::HandleDragOver(qint32 x, qint32 y, const QString &data)
{
    Q_EMIT m_parent->dragover(x, y, data);
}

void DockAppletDBus::HandleMouseWheel(qint32 x, qint32 y, qint32 angleDelta)
{
    Q_EMIT m_parent->mousewheel(x, y, angleDelta);
}

void DockAppletDBus::Activate(qint32 x, qint32 y)
{
    Q_EMIT m_parent->activate(x, y);
}

void DockAppletDBus::SecondaryActivate(qint32 x, qint32 y)
{
    Q_EMIT m_parent->secondaryActivate(x, y);
}

void DockAppletDBus::ContextMenu(qint32 x, qint32 y)
{
    qDebug() << "Hasn't support" << x << y;
}

DockMenu::DockMenu(QQuickItem *parent)
    :QQuickItem(parent)
{

}

DockMenu::~DockMenu()
{
}
