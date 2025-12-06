/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Standalone entry point for the Classic Qt Widgets Kernel module.
 * Provides a traditional widget-based UI for lightweight DEs (XFCE, LXQt, etc).
 */

#include "../KernelListModel.h"
#include "../KernelViewModel.h"
#include "KernelWidgetView.h"

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QIcon>
#include <QMessageBox>

using namespace mcp::qt::kernel;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MCP Kernel Manager");
    app.setApplicationDisplayName(QObject::tr("Kernel Manager"));
    app.setOrganizationName("Manjaro");
    app.setOrganizationDomain("manjaro.org");
    
#ifdef MCP_VERSION
    app.setApplicationVersion(MCP_VERSION);
#endif

    // Create the model and view model
    KernelListModel model;
    KernelViewModel viewModel(model);

    // Create main window
    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QObject::tr("Manjaro Kernel Manager"));
    mainWindow.setMinimumSize(700, 500);
    mainWindow.resize(850, 600);

    // Create and set the central widget
    auto* kernelView = new KernelWidgetView(&viewModel);
    mainWindow.setCentralWidget(kernelView);

    // Menu bar
    QMenuBar* menuBar = mainWindow.menuBar();
    
    QMenu* fileMenu = menuBar->addMenu(QObject::tr("&File"));
    QAction* quitAction = fileMenu->addAction(QObject::tr("&Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
    
    QMenu* helpMenu = menuBar->addMenu(QObject::tr("&Help"));
    QAction* aboutAction = helpMenu->addAction(QObject::tr("&About"));
    QObject::connect(aboutAction, &QAction::triggered, [&mainWindow]() {
        QMessageBox::about(&mainWindow,
            QObject::tr("About Manjaro Kernel Manager"),
            QObject::tr("<h3>Manjaro Kernel Manager</h3>"
                        "<p>Version %1</p>"
                        "<p>A tool for managing Linux kernels on Manjaro.</p>"
                        "<p>© 2025 Manjaro Team</p>")
#ifdef MCP_VERSION
                .arg(MCP_VERSION)
#else
                .arg("dev")
#endif
        );
    });
    
    QAction* aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));
    QObject::connect(aboutQtAction, &QAction::triggered, &app, &QApplication::aboutQt);

    // Status bar
    mainWindow.statusBar()->showMessage(QObject::tr("Ready"));

    mainWindow.show();

    return app.exec();
}
