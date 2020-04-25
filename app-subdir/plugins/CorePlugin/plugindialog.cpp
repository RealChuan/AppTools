/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "plugindialog.h"

#include <controls/dialog.h>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/plugindetailsview.h>
#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginspec.h>

//#include <utils/fancylineedit.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

namespace Core {
namespace Internal {

static bool s_isRestartRequired = false;

PluginDialog::PluginDialog(QWidget *parent)
    : Dialog(parent),
      m_view(new ExtensionSystem::PluginView(this))
{
    QFrame *frame = new QFrame(this);
    QVBoxLayout *vl = new QVBoxLayout(frame);

    auto filterLayout = new QHBoxLayout;
    vl->addLayout(filterLayout);
    //auto filterEdit = new Utils::FancyLineEdit(this);
    //filterEdit->setFiltering(true);
    //connect(filterEdit, &Utils::FancyLineEdit::filterChanged,
    //        m_view, &ExtensionSystem::PluginView::setFilter);
    //filterLayout->addWidget(filterEdit);
    m_view->setShowHidden(false);
    auto showHidden = new QCheckBox(tr("Show all"));
    showHidden->setToolTip(tr("Show all installed plugins, including base plugins "
                              "and plugins that are not available on this platform."));
    showHidden->setChecked(m_view->isShowingHidden());
    connect(showHidden, &QCheckBox::stateChanged,
            m_view, &ExtensionSystem::PluginView::setShowHidden);
    filterLayout->addWidget(showHidden);

    vl->addWidget(m_view);

    m_detailsButton = new QPushButton(tr("Details"), this);
    m_detailsButton->setObjectName("BlueButton");
    m_errorDetailsButton = new QPushButton(tr("Error Details"), this);
    m_errorDetailsButton->setObjectName("RedButton");
    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setObjectName("BlueButton");
    m_detailsButton->setEnabled(false);
    m_errorDetailsButton->setEnabled(false);
    m_closeButton->setEnabled(true);
    m_closeButton->setDefault(true);

    m_restartRequired = new QLabel(tr("Restart required."), this);
    if (!s_isRestartRequired)
        m_restartRequired->setVisible(false);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(m_detailsButton);
    hl->addWidget(m_errorDetailsButton);
    hl->addSpacing(10);
    hl->addWidget(m_restartRequired);
    hl->addStretch(5);
    hl->addWidget(m_closeButton);

    vl->addLayout(hl);

    setCentralWidget(frame);

    resize(650, 400);
    setWindowTitle(tr("Installed Plugins"));

    connect(m_view, &ExtensionSystem::PluginView::currentPluginChanged,
            this, &PluginDialog::updateButtons);
    connect(m_view, &ExtensionSystem::PluginView::pluginActivated,
            this, &PluginDialog::openDetails);
    connect(m_view, &ExtensionSystem::PluginView::pluginSettingsChanged,
            this, &PluginDialog::updateRestartRequired);
    connect(m_detailsButton, &QAbstractButton::clicked,
            [this]  { openDetails(m_view->currentPlugin()); });
    connect(m_errorDetailsButton, &QAbstractButton::clicked,
            this, &PluginDialog::openErrorDetails);
    connect(m_closeButton, &QAbstractButton::clicked,
            this, &PluginDialog::closeDialog);
    updateButtons();
}

void PluginDialog::closeDialog()
{
    ExtensionSystem::PluginManager::writeSettings();
    accept();
}

void PluginDialog::updateRestartRequired()
{
    // just display the notice all the time after once changing something
    s_isRestartRequired = true;
    m_restartRequired->setVisible(true);
}

void PluginDialog::updateButtons()
{
    ExtensionSystem::PluginSpec *selectedSpec = m_view->currentPlugin();
    if (selectedSpec) {
        m_detailsButton->setEnabled(true);
        m_errorDetailsButton->setEnabled(selectedSpec->hasError());
    } else {
        m_detailsButton->setEnabled(false);
        m_errorDetailsButton->setEnabled(false);
    }
}

void PluginDialog::openDetails(ExtensionSystem::PluginSpec *spec)
{
    if (!spec)
        return;

    Dialog dialog(this);
    dialog.setTitle(tr("Plugin Details of %1").arg(spec->name()));

    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    closeButton->setObjectName("BlueButton");
    connect(closeButton, &QPushButton::clicked, &dialog, &Dialog::aboutToclose);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addStretch(0);
    h1->addWidget(closeButton);

    ExtensionSystem::PluginDetailsView *details = new ExtensionSystem::PluginDetailsView(&dialog);
    details->update(spec);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(details);
    layout->addLayout(h1);

    dialog.setCentralWidget(widget);
    dialog.resize(400, 500);
    dialog.exec();
}

void PluginDialog::openErrorDetails()
{
    ExtensionSystem::PluginSpec *spec = m_view->currentPlugin();
    if (!spec)
        return;
    Dialog dialog(this);
    dialog.setTitle(tr("Plugin Details of %1").arg(spec->name()));

    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    closeButton->setObjectName("BlueButton");
    connect(closeButton, &QPushButton::clicked, &dialog, &Dialog::aboutToclose);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addStretch(0);
    h1->addWidget(closeButton);

    ExtensionSystem::PluginErrorView *errors = new ExtensionSystem::PluginErrorView(&dialog);
    errors->update(spec);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(errors);
    layout->addLayout(h1);

    dialog.setCentralWidget(widget);
    dialog.resize(400, 500);
    dialog.exec();
}

} // namespace Internal
} // namespace Core
