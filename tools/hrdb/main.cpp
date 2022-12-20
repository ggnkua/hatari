#include "ui/mainwindow.h"

#include <QApplication>
#include <QSettings>
#include "hrdbapplication.h"
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    // These are used in settings
    QCoreApplication::setOrganizationName("hrdb");
    QCoreApplication::setApplicationName("hrdb");
    QCoreApplication::setApplicationVersion(VERSION_STRING);
    QSettings::setDefaultFormat(QSettings::Format::IniFormat);

    // This creates the app and the session (including loading session settings)
    HrdbApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("hrdb -- a Hatari Remote DeBugger UI");
    parser.addHelpOption();
    parser.addVersionOption();

    // A boolean option (-q, --quicklaunch)
    QCommandLineOption quickLaunchOption(QStringList() << "q" << "quicklaunch",
                                         "Launch Hatari with previously-saved UI settings.");
    parser.addOption(quickLaunchOption);
    parser.process(app);

    // Build the UI
    MainWindow w(app.m_session);
    w.show();

    // Kick off hatari if requested
    if (parser.isSet(quickLaunchOption))
    {
       if (!LaunchHatari(app.m_session.GetLaunchSettings(), &app.m_session))
       {
            QTextStream(stderr) << QString("ERROR: quicklaunch: Unable to run hatari\n");
            return 1;
       }
    }

    return app.exec();
}
