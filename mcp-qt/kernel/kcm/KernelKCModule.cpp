#include "KernelKCModule.h"
#include "../KernelViewModel.h"
#include "VersionInfo.h"

#include <KAboutData>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KCMKernelFactory, "kcm_mcp_kernel.json", registerPlugin<KernelKCModule>();)

KernelKCModule::KernelKCModule(QObject *parent,
                               const KPluginMetaData &data,
                               const QVariantList &args)
    : KQuickConfigModule(parent, data)
{
    setButtons(NoAdditionalButton);

    qmlRegisterSingletonType<mcp::qt::common::VersionInfo>(
        "org.manjaro.mcp.components", 1, 0, "VersionInfo",
        [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new mcp::qt::common::VersionInfo();
        });
   
    // Create model and view model in constructor so they're available when QML loads
    m_model = std::make_unique<KernelListModel>();
    m_viewModel = std::make_unique<KernelViewModel>(*m_model);
    setVm(m_viewModel.get());
}

void KernelKCModule::load()
{
    KQuickConfigModule::load();
}

KernelViewModel *KernelKCModule::vm() const
{
    return m_vm;
}

void KernelKCModule::setVm(KernelViewModel *newVm)
{
    if (m_vm == newVm)
        return;
    m_vm = newVm;
    Q_EMIT vmChanged();
}

#include "KernelKCModule.moc"
