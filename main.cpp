/**
 * kcs: Switch color schemes in KDE via command line
 *
 * @author Vishesh Yadav
 * @license BSD Licence
 */

#include <iostream>

#include <QtCore/QFileInfo>
#include <QtDBus/QtDBus>
#include <QCoreApplication>

#include <KDebug>
#include <KGlobal>
#include <KConfigGroup>
#include <KGlobalSettings>
#include <KStandardDirs>

QMap<QString, QString>   availableColorSchemes();
void                     applyColorScheme(const QString &path);


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList arguments = QCoreApplication::arguments();

    QMap<QString, QString> schemes = availableColorSchemes();

    QStringListIterator iter(arguments);
    if (iter.hasNext()) iter.next(); // first is always the commmand

    if (!iter.hasNext()) {
        std::cout << "Usage: kcs <list|switch> [ColorSchemeName]" << std::endl;
        return 0;
    }

    QString command = iter.next();
    if (command == "list") {
        std::cout << "Listing available color schemes..." << std::endl;
        QMap<QString, QString>::const_iterator i = schemes.constBegin();
        for ( ; i != schemes.constEnd(); i++) {
            std::cout << "=> " << i.key().toStdString() << std::endl;
        }
    }
    else if (command == "switch") {
        QString schemeName = iter.next();
        if (!schemes.contains(schemeName)) {
            std::cerr << "<error> '" << schemeName.toStdString() 
                << "' color scheme doesn't exists" << std::endl;
            return EXIT_FAILURE;
        }
        applyColorScheme(schemes[schemeName]);
    }
    else if (command == "current") {
        KSharedConfigPtr globalConfig = KSharedConfig::openConfig("kdeglobals");
        KConfigGroup group(globalConfig, "General");
        std::cout << "Current colorscheme is => " 
            << group.readEntry("ColorScheme").toStdString() << std::endl;
    }
    else if (command == "toggle") {
        //TODO
    }
    else {
        std::cerr << "<error> Invalid command provided";
    }
        
    return EXIT_SUCCESS;

}

QMap<QString, QString> availableColorSchemes()
{
    QMap<QString, QString> result;
    const QStringList schemeFiles = KGlobal::dirs()->findAllResources("data", 
                        "color-schemes/*.colors", KStandardDirs::NoDuplicates);

    foreach (QString file, schemeFiles) {
        KSharedConfigPtr schemeConfig = KSharedConfig::openConfig(file);
        KConfigGroup schemeGroup(schemeConfig, "General");
        QString schemeName = schemeGroup.readEntry("Name");
        result[schemeName] = file;
    }

    return result;
}

void applyColorScheme(const QString &path)
{
    // load global and color scheme config files
    KSharedConfigPtr schemeConfig = KSharedConfig::openConfig(path);
    KSharedConfigPtr globalConfig = KSharedConfig::openConfig("kdeglobals");

    // copy color scheme to kdeglobals
    QStringList schemeConfigGroupList = schemeConfig->groupList();
    foreach (QString group, schemeConfigGroupList) {
        KConfigGroup schemeGroup(schemeConfig, group);
        KConfigGroup globalGroup(globalConfig, group);

        QMap<QString, QString> schemeEntryMap = schemeGroup.entryMap();

        QMap<QString, QString>::const_iterator i = schemeEntryMap.constBegin();
        while (i != schemeEntryMap.constEnd()) {
            if (group == "General" && i.key() == "Name") {
                globalGroup.writeEntry("ColorScheme", i.value());
            }
            else {
                globalGroup.writeEntry(i.key(), i.value());
            }
            ++i;
        }
        globalConfig->sync();
    }

    KGlobalSettings::self()->emitChange(KGlobalSettings::PaletteChanged);
    QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);

    KConfig cfg("kcmdisplayrc", KConfig::NoGlobals);
    KConfigGroup displayGroup(&cfg, "X11");

    bool applyToAlien = true;
    displayGroup.writeEntry("exportKDEColors", applyToAlien );
    cfg.sync();
}

