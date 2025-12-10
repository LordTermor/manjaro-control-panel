include(ECMQmlModule)

ecm_add_qml_module(Components URI "org.manjaro.mcp.components" VERSION 1.0 GENERATE_PLUGIN_SOURCE)

ecm_target_qml_sources(Components SOURCES InstallationDialog.qml)
ecm_target_qml_sources(Components SOURCES ConfirmationDialog.qml)
ecm_target_qml_sources(Components SOURCES Badge.qml)


ecm_finalize_qml_module(Components DESTINATION ${KDE_INSTALL_QMLDIR})
