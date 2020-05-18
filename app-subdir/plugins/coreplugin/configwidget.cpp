#include "configwidget.h"

#include <utils/utils.h>

#include <QtWidgets>

class ConfigWidgetPrivate{
public:
    ConfigWidgetPrivate(QWidget *parent) : owner(parent){
        languageBox = new QComboBox(owner);
    }
    QWidget *owner;
    QComboBox *languageBox;
};

ConfigWidget::ConfigWidget(QWidget *parent) : QWidget(parent)
  , d(new ConfigWidgetPrivate(this))
{
    setupUI();
    initWindow();
    setWindowParam();
}

ConfigWidget::~ConfigWidget()
{
    save();
}

void ConfigWidget::setupUI()
{
    QFormLayout *fromLayout = new QFormLayout(this);
    fromLayout->addRow(tr("Language(Requires Restart): "), d->languageBox);
}

void ConfigWidget::initWindow()
{
    d->languageBox->addItem(tr("Chinese"), Utils::Chinese);
    d->languageBox->addItem(tr("English"), Utils::English);
}

void ConfigWidget::setWindowParam()
{
    d->languageBox->setCurrentIndex(Utils::getCurrentLanguage());
}

void ConfigWidget::save()
{
    Utils::saveLanguage(Utils::Language(d->languageBox->currentData().toInt()));
}
