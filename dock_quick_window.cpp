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
    qDebug() << "Window is destroy...";
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
void DockApplet::handleMenuItem(QString id)
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
    //qDebug() << "icon Changed:" << v;
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
    if (m_window){
        Monitor::instance().remove(m_window->winId(), this);
        disconnect(m_window, SIGNAL(screenDestroyed));
    }

    m_window = w;
    m_dbus_proxyer->setData("app-xids", QString("[{\"Xid\":%1,\"Title\":\"\"}]").arg(w->winId()));

    if (m_window) {
        Monitor::instance().add(w->winId(), this);
        connect(m_window, SIGNAL(screenChanged(QScreen *)), this, SLOT(notifyQt5ScreenDestroyed(QScreen*)));
    }
    Q_EMIT windowChanged(w);
}

DockApplet::DockApplet(QQuickItem *parent)
    :QQuickItem(parent)
{
    this->m_dbus_proxyer= new DockAppletDBus(this);
}

DockApplet::~DockApplet()
{
    if (m_window) {
        Monitor::instance().remove(m_window->winId(), this);
        m_window->setParent(NULL);
        m_window->deleteLater();
        m_window = NULL;
    }
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

void DockAppletDBus::HandleMenuItem(QString id)
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

void DockAppletDBus::ShowQuickWindow()
{
    DockQuickWindow * window = m_parent->window();
    if (window) {
        QScreen * pScreen = window->screen();
        // TODO: Bugfix, remove when qt fix this bug
        if (NULL == pScreen) {
            return;
        }
        window->show();
    }
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

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <QMap>
#include <QProcess>


Monitor& Monitor::instance() {
    static Monitor s_monitor;
    return s_monitor;
}

Monitor::Monitor()
    : QAbstractNativeEventFilter()
{
    if (QGuiApplication *gui = dynamic_cast<QGuiApplication*>(QCoreApplication::instance())) {
        gui->installNativeEventFilter(this);
        qDebug()<<"Monitor Instance OK";
    }
}

void Monitor::add(WId wid, DockApplet* dockApplet){
    m_dockApplets[wid] = dockApplet;
}

void Monitor::remove(WId wid, DockApplet* dockApplet){
    Q_UNUSED(dockApplet);
    m_dockApplets.remove(wid);
}

bool Monitor::nativeEventFilter(const QByteArray &eventType, void *message, long *result){
    Q_UNUSED(result);
    if (eventType=="xcb_generic_event_t") {
        xcb_generic_event_t *event = static_cast<xcb_generic_event_t*>(message);
        const uint8_t responseType = event->response_type & ~0x80;
        if (responseType == XCB_DESTROY_NOTIFY) {
            xcb_destroy_notify_event_t *ev = reinterpret_cast<xcb_destroy_notify_event_t*>(event);
            DockApplet * dockApplet = m_dockApplets[ev->window];
            if (dockApplet) {
                dockApplet->nativeWindowDestroyed();
            }
        }
    }
    return false;
}
