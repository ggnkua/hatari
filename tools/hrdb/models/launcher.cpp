#pragma once
#include "launcher.h"
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QSettings>

#include "session.h"

void LaunchSettings::loadSettings(QSettings& settings)
{
    // Save launcher settings
    settings.beginGroup("LaunchSettings");
    m_hatariFilename = settings.value("exe", QVariant("hatari")).toString();
    m_argsTxt = settings.value("args", QVariant("")).toString();
    m_prgFilename = settings.value("prg", QVariant("")).toString();
    m_workingDirectory = settings.value("workingDirectory", QVariant("")).toString();
    m_breakMode = settings.value("breakMode", QVariant("0")).toInt();
    settings.endGroup();
}

void LaunchSettings::saveSettings(QSettings &settings) const
{
    settings.beginGroup("LaunchSettings");
    settings.setValue("exe", m_hatariFilename);
    settings.setValue("args", m_argsTxt);
    settings.setValue("prg", m_prgFilename);
    settings.setValue("workingDirectory", m_workingDirectory);
    settings.setValue("breakMode", m_breakMode);
    settings.endGroup();
}

bool LaunchHatari(const LaunchSettings& settings, const Session* pSession)
{
    // Create a copy of the args that we can adjust
    QStringList args;
    QString otherArgsText = settings.m_argsTxt;
    otherArgsText = otherArgsText.trimmed();
    if (otherArgsText.size() != 0)
        args = otherArgsText.split(" ");

    // First make a temp file for breakpoints etc
    if (settings.m_breakMode != LaunchSettings::BreakMode::kNone)
    {
        QString tmpContents;
        QTextStream ref(&tmpContents);

        // Generate some commands for
        // Break at boot/start commands
        if (settings.m_breakMode == LaunchSettings::BreakMode::kBoot)
            ref << QString("b pc ! 0 : once\r\n");
        else if (settings.m_breakMode == LaunchSettings::BreakMode::kProgStart)
            ref << QString("b pc=TEXT && pc<$e00000 : once\r\n");

        // Create the temp file
        // In theory we need to be careful about reuse?
        QTemporaryFile& tmp(*pSession->m_pStartupFile);
        if (!tmp.open())
            return false;

        tmp.setTextModeEnabled(true);
        tmp.write(tmpContents.toUtf8());
        tmp.close();

        // Prepend the "--parse N" part (backwards!)
        args.push_front(tmp.fileName());
        args.push_front("--parse");
    }

    // Executable goes as last arg
    args.push_back(settings.m_prgFilename);

    // Actually launch the program
    QProcess proc;
    proc.setProgram(settings.m_hatariFilename);
    proc.setArguments(args);

    // Redirect outputs to NULL so that Hatari's own spew doesn't cause lockups
    // if hrdb is killed and restarted (temp file contention?)
    proc.setStandardOutputFile(QProcess::nullDevice());
    proc.setStandardErrorFile(QProcess::nullDevice());
    proc.setWorkingDirectory(settings.m_workingDirectory);
    return proc.startDetached();
}
