#ifndef QUICKLAYOUT_H
#define QUICKLAYOUT_H

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

inline void SetMargins(QLayout* pLayout)
{
    pLayout->setContentsMargins(3, 3, 3, 3);
}

inline void SetMarginsRows(QLayout* pLayout)
{
    pLayout->setContentsMargins(3, 1, 0, 3);
}

inline QGroupBox*    CreateHorizLayout(QWidget* parent, QWidget* pWidgets[])
{
    QHBoxLayout* pLayout = new QHBoxLayout;
    SetMargins(pLayout);
    while (*pWidgets != nullptr)
    {
        pLayout->addWidget(*pWidgets);
        ++pWidgets;
    }

    QGroupBox* pBaseWidget = new QGroupBox(parent);
    pBaseWidget->setFlat(true);
    pBaseWidget->setLayout(pLayout);
    return pBaseWidget;
}

inline QGroupBox*    CreateVertLayout(QWidget* parent, QWidget* pWidgets[])
{
    QVBoxLayout* pLayout = new QVBoxLayout;
    SetMargins(pLayout);
    while (*pWidgets != nullptr)
    {
        pLayout->addWidget(*pWidgets);
        ++pWidgets;
    }

    QGroupBox* pBaseWidget = new QGroupBox(parent);
    pBaseWidget->setFlat(true);
    pBaseWidget->setLayout(pLayout);
    return pBaseWidget;
}


#endif // QUICKLAYOUT_H
