#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDebug>
#include <QMetaType>

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
        }
        Config(QHash<QString, QVariant> hash);
        QString name() const { return m_name; }
        QString os() const { return m_os; }
        QString id() const { return m_id; }
        bool enabled() const { return m_enabled; }
        QHash<QString, QVariant> serialize();
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
        }
        Platform(QHash<QString, QVariant> hash);
        QString name() const { return m_name; }
        bool enabled() const { return m_enabled; }
        QList<QObject *> configs() const {
                QList<QObject *> retval;
                foreach (auto object, m_configs) {
                        retval << object;
                }
                return retval;
        }
        QHash<QString, QVariant> serialize();
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setEnabled(const bool &arg) { m_enabled = arg; emit enabledChanged(); }
        void addConfig(Config *arg) {
                m_configs << arg;
                emit configsChanged();
                connect(arg, SIGNAL(enabledChanged()), this, SIGNAL(configsChanged()));
        }
signals:
        void nameChanged();
        void enabledChanged();
        void configsChanged();
private:
        QString m_name;
        bool m_enabled;
        QList<Config *> m_configs;
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
        Project(QHash<QString, QVariant> hash);
        QString name() const { return m_name; }
        QString id() const { return m_id; }
        QList<QObject *> platforms() const {
                QList<QObject *> retval;
                foreach (auto object, m_platforms) {
                        retval << object;
                }
                return retval;
        }
        QHash<QString, QVariant> serialize();
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setId(const QString &arg) { m_id = arg; emit idChanged(); }
        void addPlatform(Platform *arg) {
                m_platforms << arg;
                emit platformsChanged();
                connect(arg, SIGNAL(enabledChanged()), this, SIGNAL(platformsChanged()));
                connect(arg, SIGNAL(configsChanged()), this, SIGNAL(platformsChanged()));
        }
signals:
        void nameChanged();
        void idChanged();
        void platformsChanged();
private:
        QString m_name;
        QString m_id;
        QList<Platform *> m_platforms;
};

#endif // PROJECT_H
