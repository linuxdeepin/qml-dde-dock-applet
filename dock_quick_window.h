#ifndef MYITEM_H
#define MYITEM_H

#include <QQuickItem>
#include <QQuickWindow>
#include <QDBusAbstractAdaptor>
#include <QRect>

class DockAppletDBus;
class DockApplet;
class DockQuickWindow: public QQuickWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(DockQuickWindow)

public:
    DockQuickWindow(QQuickWindow *parent = 0);
    ~DockQuickWindow();

    bool firstShow;
    void destroyed(QObject *);
};

class DockApplet : public QQuickItem {
    Q_OBJECT
    Q_DISABLE_COPY(DockApplet)

    Q_PROPERTY(QString menu READ menu WRITE setMenu NOTIFY menuChanged)
    Q_PROPERTY(QString appid READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(qint32 status READ status WRITE setStatus NOTIFY statusChanged)
public:
    DockApplet(QQuickItem *parent = 0);
    ~DockApplet();
    Q_PROPERTY(DockQuickWindow* window READ window WRITE setWindow)

    void setWindow(DockQuickWindow*);
    DockQuickWindow* window();

    const QString& id() { return m_id; }
    void setId(const QString& v) { m_id = v; Q_EMIT idChanged(v);}
    Q_SIGNAL void idChanged(QString);


    const QString& menu() { return m_menu; }
    void setMenu(const QString& m);
    Q_SIGNAL void menuChanged(QString);

    const QString& icon() {return m_icon; }
    void setIcon(const QString& v);
    Q_SIGNAL void iconChanged(QString);

    const QString& title() {return m_title; }
    void setTitle(const QString& v);
    Q_SIGNAL void titleChanged(QString);

    qint32 status() {return m_status; }
    void setStatus(const qint32 v);
    Q_SIGNAL void statusChanged(qint32);

private:
    QString m_menu;
    QString m_id;
    QString m_icon;
    QString m_title;
    qint32 m_status;
    DockAppletDBus* m_dbus_proxyer;
};

typedef QMap<QString,QString> StringMap;
Q_DECLARE_METATYPE(StringMap)

class DockAppletDBus : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dde.dock.Entry")

    Q_PROPERTY(QString Id READ id)
    Q_PROPERTY(QString Type READ type)
    Q_PROPERTY(StringMap Data READ data)

public:
    DockAppletDBus(DockApplet* parent);

    const QMap<QString,QString>& data() {
	return m_data;
    }
    const QString type() { return "Applet"; }
    const QString id() {return m_parent->id(); }

    void setData(const QString& k, const QString& v) {
	m_data[k] = v;
	Q_EMIT DataChanged(k,v);
	qDebug() << "SetData" <<  k <<  m_data[k];
    }

    Q_SIGNAL void DataChanged(QString,QString);
private:
    StringMap  m_data;
    DockApplet* m_parent;
};

#endif // MYITEM_H
