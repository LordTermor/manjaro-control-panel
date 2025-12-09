/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelPage.h"
#include "../KernelListModel.h"
#include "../KernelViewModel.h"

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QIcon>
#include <QMessageBox>

/*
 * Standalone entry point for Classic Qt Widgets Kernel Manager.
 * Provides traditional widget-based UI for lightweight DEs.
 */

using namespace mcp::qt::kernel;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MCP Kernel Manager");
    app.setApplicationDisplayName(QObject::tr("Manjaro Kernel Manager"));
    app.setOrganizationName("Manjaro");
    app.setOrganizationDomain("manjaro.org");
    
#ifdef MCP_VERSION
    app.setApplicationVersion(MCP_VERSION);
#endif

    app.setWindowIcon(QIcon::fromTheme("preferences-system-linux-kernel",
                                       QIcon(":/images/resources/tux-manjaro.png")));

    KernelListModel model;
    KernelViewModel viewModel(model);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QObject::tr("Manjaro Kernel Manager"));
    mainWindow.setWindowIcon(app.windowIcon());
    mainWindow.setMinimumSize(700, 500);
    mainWindow.resize(850, 600);

    auto* kernelPage = new KernelPage(&viewModel);
    mainWindow.setCentralWidget(kernelPage);

    QMenuBar* menuBar = mainWindow.menuBar();
    
    QMenu* fileMenu = menuBar->addMenu(QObject::tr("&File"));
    
    QAction* refreshAction = fileMenu->addAction(QObject::tr("&Refresh"));
    refreshAction->setShortcut(QKeySequence::Refresh);
    refreshAction->setIcon(QIcon::fromTheme("view-refresh"));
    // TODO: Connect to viewModel refresh method when available
    
    fileMenu->addSeparator();
    
    QAction* quitAction = fileMenu->addAction(QObject::tr("&Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    quitAction->setIcon(QIcon::fromTheme("application-exit"));
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
    
    QMenu* helpMenu = menuBar->addMenu(QObject::tr("&Help"));
    
    QAction* aboutAction = helpMenu->addAction(QObject::tr("&About"));
    aboutAction->setIcon(QIcon::fromTheme("help-about"));
    QObject::connect(aboutAction, &QAction::triggered, [&mainWindow]() {
        QMessageBox::about(&mainWindow,
            QObject::tr("About Manjaro Kernel Manager"),
            QObject::tr("<h3>Manjaro Kernel Manager</h3>"
                        "<p>Version %1</p>"
                        "<p>A tool for managing Linux kernels on Manjaro.</p>"
                        "<p>Install, remove, and manage different kernel versions "
                        "with an easy-to-use interface.</p>"
                        "<p>© 2025 Manjaro Team</p>")
#ifdef MCP_VERSION
                .arg(MCP_VERSION)
#else
                .arg("dev")
#endif
        );
    });
    
    QAction* aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));
    aboutQtAction->setIcon(QIcon::fromTheme("help-about"));
    QObject::connect(aboutQtAction, &QAction::triggered, &app, &QApplication::aboutQt);

    mainWindow.statusBar()->showMessage(QObject::tr("Ready"));

    mainWindow.show();

    return app.exec();
}
