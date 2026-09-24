cmake_minimum_required(VERSION 3.19)
project(client LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

set(TEMPLATE_DIR  template-editor)
set(TOC_DIR       toc-editor)
set(GENERATOR_DIR generator)
set(SCANNER_DIR   scanner)
set(SEARCH_DIR    search)
set(SETTINGS_DIR  settings)
set(MAIN_DIR      main)
set(UTILS_DIR     utils)
set(PROJECT_DIR   project)

set(TEMPLATE_SOURCES
    ${TEMPLATE_DIR}/templatedialog.ui
    ${TEMPLATE_DIR}/templatedialog.cpp
    ${TEMPLATE_DIR}/templatedialog.h
    ${TEMPLATE_DIR}/docstab.cpp
    ${TEMPLATE_DIR}/stagestab.cpp
    ${TEMPLATE_DIR}/categoriestab.cpp
    ${TEMPLATE_DIR}/categoriestablemodel.h 
    ${TEMPLATE_DIR}/categoriestablemodel.cpp
    ${TEMPLATE_DIR}/docstablemodel.h 
    ${TEMPLATE_DIR}/docstablemodel.cpp
    ${TEMPLATE_DIR}/stagestablemodel.h 
    ${TEMPLATE_DIR}/stagestablemodel.cpp
    ${TEMPLATE_DIR}/categorycombodelegate.h 
    ${TEMPLATE_DIR}/categorycombodelegate.cpp
    ${TEMPLATE_DIR}/stagecombodelegate.h 
    ${TEMPLATE_DIR}/stagecombodelegate.cpp
    ${TEMPLATE_DIR}/categorycolorbuttondelegate.h 
    ${TEMPLATE_DIR}/categorycolorbuttondelegate.cpp
    ${TEMPLATE_DIR}/stagecolorbuttondelegate.h 
    ${TEMPLATE_DIR}/stagecolorbuttondelegate.cpp
)

set(TOC_SOURCES
    ${TOC_DIR}/tocdialog.ui
    ${TOC_DIR}/tocdialog.cpp
    ${TOC_DIR}/tocdialog.h
    ${TOC_DIR}/sectionslistmodel.h
    ${TOC_DIR}/sectionslistmodel.cpp
)

set(GENERATOR_SOURCES
    ${GENERATOR_DIR}/generatordialog.ui
    ${GENERATOR_DIR}/generatordialog.cpp
    ${GENERATOR_DIR}/generatordialog.h
    ${GENERATOR_DIR}/generatortablemodel.h
    ${GENERATOR_DIR}/generatortablemodel.cpp
    ${GENERATOR_DIR}/toccombodelegate.h 
    ${GENERATOR_DIR}/toccombodelegate.cpp
)

set(SCANNER_SOURCES
    ${SCANNER_DIR}/scannerdialog.ui
    ${SCANNER_DIR}/scannerdialog.cpp
    ${SCANNER_DIR}/scannerdialog.h
    ${SCANNER_DIR}/scanner.cpp
    ${SCANNER_DIR}/scanner.h
    ${SCANNER_DIR}/issuestreemodel.h
    ${SCANNER_DIR}/issuestreemodel.cpp
)

set(SEARCH_SOURCES
    ${SEARCH_DIR}/searchdialog.ui
    ${SEARCH_DIR}/searchdialog.cpp
    ${SEARCH_DIR}/searchdialog.h
    ${SEARCH_DIR}/search.cpp
    ${SEARCH_DIR}/search.h
    ${SEARCH_DIR}/foundtreemodel.h
    ${SEARCH_DIR}/foundtreemodel.cpp
)

set(SETTINGS_SOURCES
    ${SETTINGS_DIR}/settingsdialog.ui
    ${SETTINGS_DIR}/settingsdialog.cpp
    ${SETTINGS_DIR}/settingsdialog.h
    ${SETTINGS_DIR}/settings.cpp
    ${SETTINGS_DIR}/settings.h
    ${SETTINGS_DIR}/theme.h
    ${SETTINGS_DIR}/theme.cpp
    ${SETTINGS_DIR}/languagestab.cpp
    ${SETTINGS_DIR}/editorstab.cpp
    ${SETTINGS_DIR}/themestab.cpp
    ${SETTINGS_DIR}/editorstablemodel.h
    ${SETTINGS_DIR}/editorstablemodel.cpp
    ${SETTINGS_DIR}/editor.h
    ${SETTINGS_DIR}/editor.cpp
)
	
set(MAIN_SOURCES
    ${MAIN_DIR}/main.cpp
    ${MAIN_DIR}/mainwindow.cpp
    ${MAIN_DIR}/mainwindow.h
    ${MAIN_DIR}/mainwindow.ui
)

set(UTILS_SOURCES
    ${UTILS_DIR}/translator.h
    ${UTILS_DIR}/translator.cpp
    ${UTILS_DIR}/translation.h
    ${UTILS_DIR}/translation.cpp
    ${UTILS_DIR}/loader.h 
    ${UTILS_DIR}/loader.cpp
)

set(PROJECT_SOURCES
    ${PROJECT_DIR}/matrixview.h
    ${PROJECT_DIR}/matrixview.cpp
    ${PROJECT_DIR}/documentblockitem.h
    ${PROJECT_DIR}/documentblockitem.cpp
    ${PROJECT_DIR}/template.h
    ${PROJECT_DIR}/template.cpp
    ${PROJECT_DIR}/project.h
    ${PROJECT_DIR}/project.cpp
    ${PROJECT_DIR}/matrixwidget.h
    ${PROJECT_DIR}/matrixwidget.cpp
)

qt_add_executable(client
    WIN32 MACOSX_BUNDLE
    resources.qrc

    ${TEMPLATE_SOURCES}
    ${TOC_SOURCES}
    ${GENERATOR_SOURCES}
    ${SEARCH_SOURCES}
    ${SCANNER_SOURCES}
    ${SETTINGS_SOURCES}
    ${MAIN_SOURCES}
    ${UTILS_SOURCES}
    ${PROJECT_SOURCES}
)

target_include_directories(client PRIVATE
    ${TEMPLATE_DIR}
    ${TOC_DIR}
    ${GENERATOR_DIR}
    ${SEARCH_DIR}
    ${SCANNER_DIR}
    ${SETTINGS_DIR}
    ${MAIN_DIR}
    ${UTILS_DIR}
    ${PROJECT_DIR}
)

target_link_libraries(client
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS client
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET client
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
