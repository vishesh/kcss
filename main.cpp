/**
 * kcs: Switch color schemes in KDE via command line
 *
 * @author Vishesh Yadav
 * @license BSD Licence
 */

#include <iostream>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtDBus/QtDBus>
#include <QCoreApplication>

#include <KDebug>
#include <KGlobal>
#include <KConfigGroup>
#include <KGlobalSettings>
#include <KStandardDirs>
#include <KUrl>

QMap<QString, QString>   availableColorSchemes();
void                     applyColorScheme(const QString &path);
void                     toggleColorScheme();
QString                  currentColorScheme();


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList arguments = QCoreApplication::arguments();

    QStringListIterator iter(arguments);
    if (iter.hasNext()) iter.next(); // first is always the commmand

    if (!iter.hasNext()) {
        std::cout << "Usage: kcs <list|switch> [ColorSchemeName]" << std::endl;
        return 0;
    }
    
    QMap<QString, QString> schemes = availableColorSchemes();

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
        std::cout << "Current colorscheme is => " 
            << currentColorScheme().toStdString() << std::endl;
    }
    else if (command == "toggle") {
        toggleColorScheme();
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

        if (group.startsWith("Color") || group.startsWith("WM")) {
            globalGroup.deleteGroup();
            globalGroup = KConfigGroup(globalConfig, group);
        }

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

    KConfig cfg("kcmdisplayrc", KConfig::NoGlobals);
    KConfigGroup displayGroup(&cfg, "X11");

    bool applyToAlien = true;
    displayGroup.writeEntry("exportKDEColors", applyToAlien );
    cfg.sync();

    KGlobalSettings::self()->emitChange(KGlobalSettings::PaletteChanged);
    QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void toggleColorScheme()
{
    KUrl cfp(QDir::homePath());
    cfp.addPath(".kcss-profilerc");

    KSharedConfigPtr profileConfig = KSharedConfig::openConfig(cfp.path());
    KConfigGroup dark(profileConfig, "Dark");
    KConfigGroup light(profileConfig, "Light");
    KConfigGroup def(profileConfig, "Default");

    QString darkCs = dark.readEntry("ColorScheme");
    QString lightCs = light.readEntry("ColorScheme");
    QString defCs = def.readEntry("Profile");
    QString currentCs = currentColorScheme();

    kDebug() << "Current color scheme is " << currentCs;

    QMap<QString, QString> schemes = availableColorSchemes();

    if (currentCs == darkCs) {
        kDebug() << "Applying " << lightCs;
        applyColorScheme(schemes[lightCs]);
    }
    else if (currentCs == lightCs) {
        kDebug() << "Applying " << darkCs;
        applyColorScheme(schemes[darkCs]);
    }
    else if (defCs == "Dark") {
        kDebug() << "Applying " << darkCs;
        applyColorScheme(schemes[darkCs]);
    }
    else if(defCs == "Light") {
        kDebug() << "Applying " << lightCs;
        applyColorScheme(schemes[lightCs]);
    }
    else {
        std::cerr << "Invalid config?" << std::endl;
    }
}

QString currentColorScheme()
{
    KSharedConfigPtr globalConfig = KSharedConfig::openConfig("kdeglobals");
    KConfigGroup group(globalConfig, "General");
    return group.readEntry("ColorScheme");
}

