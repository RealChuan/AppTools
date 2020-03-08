TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    libs \
    plugins \
    tests \
    app

TRANSLATIONS += \
    translations/language.zh_cn.ts \
    translations/language.zh_en.ts
