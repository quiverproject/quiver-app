#include "project.h"

Project::Project(QHash<QString, QVariant> hash) {
        setId(hash.value("id").toString());
        setName(hash.value("name").toString());
        foreach (auto variant, hash.value("platforms").toList()) {
                addPlatform(new Platform(variant.value<QHash<QString, QVariant>>()));
        }
}

Platform::Platform(QHash<QString, QVariant> hash) {
        setName(hash.value("name").toString());
        setEnabled(hash.value("enabled").toBool());
        foreach (auto variant, hash.value("configs").toList()) {
                addConfig(new Config(variant.value<QHash<QString, QVariant>>()));
        }
}

Config::Config(QHash<QString, QVariant> hash) {
        setName(hash.value("name").toString());
        setOs(hash.value("os").toString());
        setId(hash.value("id").toString());
        setEnabled(hash.value("enabled").toBool());
}


QHash<QString, QVariant> Project::serialize() {
        QHash<QString, QVariant> hash;

        hash["id"] = id();
        hash["name"] = name();

        QVariantList platforms_list;
        foreach (auto platform_object, platforms()) {
                auto platform = qobject_cast<Platform *>(platform_object);
                platforms_list << platform->serialize();
        }
        hash["platforms"] = platforms_list;

        return hash;
}

QHash<QString, QVariant> Platform::serialize() {
        QHash<QString, QVariant> hash;

        hash["name"] = name();
        hash["enabled"] = enabled();

        QVariantList configs_list;
        foreach (auto config_object, configs()) {
                auto config = qobject_cast<Config *>(config_object);
                configs_list << config->serialize();
        }
        hash["configs"] = configs_list;

        return hash;
}

QHash<QString, QVariant> Config::serialize() {
        QHash<QString, QVariant> hash;

        hash["name"] = name();
        hash["os"] = os();
        hash["id"] = id();
        hash["enabled"] = enabled();

        return hash;
}
