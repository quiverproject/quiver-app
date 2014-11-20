#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDebug>

class Config : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QString os READ os WRITE setOs NOTIFY osChanged)
        Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
        Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
        explicit Config(QString name = QString()
                        , QString os = QString()
                        , QString id = QString()
                        , bool enabled = false
                        , QObject *parent = 0) : QObject(parent)
              , m_name(name)
              , m_os(os)
              , m_id(id)
              , m_enabled(enabled)
        {
                connect(this, &Config::enabledChanged, [=]() {
                        qDebug() << m_name << m_id << m_enabled;
                });
        }
        QString name() const { return m_name; }
        QString os() const { return m_os; }
        QString id() const { return m_id; }
        bool enabled() const { return m_enabled; }
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setOs(const QString &arg) { m_os = arg; emit osChanged(); }
        void setId(const QString &arg) { m_id = arg; emit idChanged(); }
        void setEnabled(const bool &arg) { m_enabled = arg; emit enabledChanged(); }
signals:
        void nameChanged();
        void osChanged();
        void idChanged();
        void enabledChanged();
private:
        QString m_name;
        QString m_os;
        QString m_id;
        bool m_enabled;
};

class Platform : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
        Q_PROPERTY(QList<QObject *> configs READ configs NOTIFY configsChanged)
public:
        explicit Platform(QString name = QString()
                        , bool enabled = false
                        , QObject *parent = 0) : QObject(parent)
              , m_name(name)
              , m_enabled(enabled)
        {
                connect(this, &Platform::enabledChanged, [=]() {
                        qDebug() << m_name << m_enabled;
                });
        }
        QString name() const { return m_name; }
        bool enabled() const { return m_enabled; }
        QList<QObject *> configs() const { return m_configs; }
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setEnabled(const bool &arg) { m_enabled = arg; emit enabledChanged(); }
        void addConfig(Config *arg) { m_configs << arg; emit configsChanged(); }
signals:
        void nameChanged();
        void enabledChanged();
        void configsChanged();
private:
        QString m_name;
        bool m_enabled;
        QList<QObject *> m_configs;
};

class Project : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
        Q_PROPERTY(QList<QObject *> platforms READ platforms NOTIFY platformsChanged)
public:
        explicit Project(QString name = QString()
                        , QString id = QString()
                        , QObject *parent = 0) : QObject(parent)
              , m_name(name)
              , m_id(id)
        {
        }
        QString name() const { return m_name; }
        QString id() const { return m_id; }
        QList<QObject *> platforms() const { return m_platforms; }
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setId(const QString &arg) { m_id = arg; emit idChanged(); }
        void addPlatform(Platform *arg) { m_platforms << arg; emit platformsChanged(); }
signals:
        void nameChanged();
        void idChanged();
        void platformsChanged();
private:
        QString m_name;
        QString m_id;
        QList<QObject *> m_platforms;
};

#endif // PROJECT_H
