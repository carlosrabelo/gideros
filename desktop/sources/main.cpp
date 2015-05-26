#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <gfile.h>
#include <unzip.h>
#include <pluginmanager.h>
#include <QDebug>
#include <QLibrary>
#include <lua.hpp>
#include <ghttp.h>
#include <gpath.h>
#include <gstdio.h>
#include <gvfs-native.h>
#include <gui.h>
#include <ginput.h>
#include <gevent.h>
#include <ggeolocation.h>
#include <gtexture.h>
#include <gapplication.h>
#include <gaudio.h>
#include <memory.h>
#include <glog.h>
#include "constants.cpp"

static void loadPlugins(){
	QDir dir = QDir::currentPath();

    #if defined(Q_OS_MAC)
        dir.cd("Plugins");
        QStringList files = dir.entryList(QStringList() << "*.dylib");
    #else
        dir.cd("Plugins");
        QStringList files = dir.entryList(QStringList() << "*.dll");
    #endif

    for(int i = 0; i < files.size(); ++i){
		QString filename = dir.absoluteFilePath(files[i]);
		QLibrary library(filename);
        void* plugin = (void*)library.resolve("g_pluginMain");
		if (plugin)
			PluginManager::instance().registerPlugin((void*(*)(lua_State*, int))plugin);
	}
}

int main(int argc, char *argv[]){
    QCoreApplication::setOrganizationName(Constants::ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(Constants::ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Constants::DESK_APPLICATION_NAME);

	QApplication a(argc, argv);

	QDir dir = QCoreApplication::applicationDirPath();

    #if defined(Q_OS_MAC)
        dir.cdUp();
    #endif

	QDir::setCurrent(dir.absolutePath());

    static const char *codeKey_   = "312e68c04c6fd22922b5b232ea6fb3e1"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            ;
    static const char *assetsKey_ = "312e68c04c6fd22922b5b232ea6fb3e2"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            ;

    gpath_init();

    gpath_addDrivePrefix(0, "|R|");
    gpath_addDrivePrefix(0, "|r|");
    gpath_addDrivePrefix(1, "|D|");
    gpath_addDrivePrefix(1, "|d|");
    gpath_addDrivePrefix(2, "|T|");
    gpath_addDrivePrefix(2, "|t|");

    gpath_setDriveFlags(0, GPATH_RO | GPATH_REAL);
    gpath_setDriveFlags(1, GPATH_RW | GPATH_REAL);
    gpath_setDriveFlags(2, GPATH_RW | GPATH_REAL);

    gpath_setAbsolutePathFlags(GPATH_RW | GPATH_REAL);

    gpath_setDefaultDrive(0);

    gvfs_init();
    gvfs_setCodeKey(codeKey_ + 32);
    gvfs_setAssetsKey(assetsKey_ + 32);

    gevent_Init();

    gapplication_init();

    ginput_init();

    ggeolocation_init();

    ghttp_Init();

    gui_init();

    gtexture_init();

    gaudio_Init();

    loadPlugins();

	int result = 0;
	{
		MainWindow w;
        w.show();
		result = a.exec();
	}

    gaudio_Cleanup();

    gtexture_cleanup();

    gui_cleanup();

    ghttp_Cleanup();

    ggeolocation_init();

    ginput_cleanup();

    gapplication_cleanup();

    gevent_Cleanup();

    gvfs_cleanup();

    gpath_cleanup();

	return result;
}