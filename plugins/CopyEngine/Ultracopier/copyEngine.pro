QT += widgets systeminfo
TEMPLATE        = lib
CONFIG         += plugin
HEADERS         = copyEngine.h \
                StructEnumDefinition.h \
    scanFileOrFolder.h \
    fileErrorDialog.h \
    fileExistsDialog.h \
    fileIsSameDialog.h \
    factory.h \
    StructEnumDefinition_CopyEngine.h \
    DebugEngineMacro.h \
    Variable.h \
    debugDialog.h \
    TransferThread.h \
    ReadThread.h \
    WriteThread.h \
    RmPath.h \
    MkPath.h \
    folderExistsDialog.h \
    AvancedQFile.h \
    ListThread.h \
    ../../../interface/PluginInterface_CopyEngine.h \
    ../../../interface/OptionInterface.h \
    ../../../interface/FacilityInterface.h \
    Filters.h \
    FilterRules.h \
    RenamingRules.h \
    DriveManagement.h
SOURCES         = copyEngine.cpp \
    scanFileOrFolder.cpp \
    fileErrorDialog.cpp \
    fileExistsDialog.cpp \
    fileIsSameDialog.cpp \
    factory.cpp \
    debugDialog.cpp \
    TransferThread.cpp \
    ReadThread.cpp \
    WriteThread.cpp \
    RmPath.cpp \
    MkPath.cpp \
    folderExistsDialog.cpp \
    AvancedQFile.cpp \
    copyEngine-collision-and-error.cpp \
    ListThread.cpp \
    Filters.cpp \
    FilterRules.cpp \
    RenamingRules.cpp \
    ListThread_InodeAction.cpp \
    DriveManagement.cpp
TARGET          = $$qtLibraryTarget(copyEngine)
include(../../../updateqm.pri)
TRANSLATIONS += Languages/ar/translation.ts \
    Languages/de/translation.ts \
    Languages/el/translation.ts \
    Languages/es/translation.ts \
    Languages/fr/translation.ts \
    Languages/hi/translation.ts \
    Languages/id/translation.ts \
    Languages/it/translation.ts \
    Languages/ja/translation.ts \
    Languages/ko/translation.ts \
    Languages/nl/translation.ts \
    Languages/no/translation.ts \
    Languages/pl/translation.ts \
    Languages/pt/translation.ts \
    Languages/ru/translation.ts \
    Languages/th/translation.ts \
    Languages/tr/translation.ts \
    Languages/zh/translation.ts

include(../../../extratool.pri)
target.path     = $${PREFIX}/lib/ultracopier/$$superBaseName(_PRO_FILE_PWD_)
translations.files = Languages
translations.path = $${PREFIX}/lib/ultracopier/$$superBaseName(_PRO_FILE_PWD_)
infos.files      = informations.xml
infos.path       = $${PREFIX}/lib/ultracopier/$$superBaseName(_PRO_FILE_PWD_)
INSTALLS       += target translations infos

FORMS += \
    options.ui \
    fileErrorDialog.ui \
    fileExistsDialog.ui \
    fileIsSameDialog.ui \
    debugDialog.ui \
    folderExistsDialog.ui \
    Filters.ui \
    FilterRules.ui \
    RenamingRules.ui

OTHER_FILES += informations.xml

RESOURCES += \
    resources.qrc
