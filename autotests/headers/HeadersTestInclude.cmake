set(REQUIRED_QT_VERSION "5.2.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core Widgets)
#TODO: test KPROPERTY_KF
find_package(KF5 5.7.0 REQUIRED Config CoreAddons WidgetsAddons GuiAddons)
find_package(KPropertyCore ${PROJECT_VERSION} REQUIRED)
find_package(KPropertyWidgets ${PROJECT_VERSION} REQUIRED)
