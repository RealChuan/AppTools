#include "messbox.h"

#include <QtWidgets>

class MessBoxPrivate{
public:
    MessBoxPrivate(CommonWidget *parent) : owner(parent){
        iconLabel = new QLabel(owner);
        messageLabel = new QLabel(owner);
        messageLabel->setObjectName("MessageLabel");
        //messageLabel->setWordWrap(true);
        closeButton = new QPushButton(QObject::tr("Close"), owner);
        closeButton->setObjectName("MessCloseButton");
    }
    CommonWidget *owner;
    QLabel *iconLabel;
    QLabel *messageLabel;
    QPushButton *closeButton;
};

MessBox::MessBox(QWidget *parent) : CommonWidget(parent)
  , d(new MessBoxPrivate(this))
{
    setWindowFlags(Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    setMinButtonVisible(false);
    setRestoreMaxButtonVisible(false);
    setupUI();
    buildConnect();
    resize(450, 280);
}

MessBox::~MessBox()
{
    delete d;
}

void MessBox::Info(QWidget *parent, const QString &msg)
{
    MessBox messBox(parent);
    messBox.setIconLabelObjectName("InfoLabel");
    messBox.setMessage(msg);
    messBox.exec();
}

void MessBox::Warning(QWidget *parent, const QString &msg)
{
    MessBox messBox(parent);
    messBox.setIconLabelObjectName("WarningLabel");
    messBox.setMessage(msg);
    messBox.exec();
}

void MessBox::exec()
{
    show();
    QEventLoop loop(this);
    connect(this, &MessBox::aboutToclose, &loop, &QEventLoop::quit);
    loop.exec();
}

void MessBox::setMessage(const QString &msg)
{
    d->messageLabel->setText(msg);
}

void MessBox::setIconLabelObjectName(const QString &objectName)
{
    d->iconLabel->setObjectName(objectName);
}

void MessBox::setupUI()
{
    QHBoxLayout *mLayout = new QHBoxLayout;
    mLayout->setSpacing(20);
    mLayout->addStretch(0);
    mLayout->addWidget(d->iconLabel);
    mLayout->addWidget(d->messageLabel);
    mLayout->addStretch(0);

    QWidget *btnWidget = new QWidget(this);
    btnWidget->setObjectName("MessBtnWidget");
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setContentsMargins(3, 3, 10, 3);
    btnLayout->addStretch(0);
    btnLayout->addWidget(d->closeButton);

    QWidget *widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(mLayout);
    layout->addWidget(btnWidget);

    setCentralWidget(widget);
}

void MessBox::buildConnect()
{
    connect(d->closeButton, &QPushButton::clicked, this, &MessBox::aboutToclose);
}
