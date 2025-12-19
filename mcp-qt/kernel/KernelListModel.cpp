/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "KernelListModel.h"

#include <QMetaEnum>
#include "KernelData.h"

namespace mcp::qt::kernel {

int KernelListModel::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    return static_cast<int>(m_filteredList.size());
}

QVariant KernelListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || static_cast<size_t>(index.row()) >= m_filteredList.size())
        return QVariant{};
    
    const auto &el = m_filteredList[static_cast<size_t>(index.row())];

    switch (role) {
    case Name:
        return QString::fromStdString(el.package_name);
    case IsInstalled:
        return el.is_installed();
    case Version:
        return QString::fromStdString(el.version.to_string());
    case IsLTS:
        return el.is_lts();
    case IsRecommended:
        return el.is_recommended();
    case IsRealTime:
        return el.flags.real_time;
    case IsEOL:
        return el.flags.not_supported;
    case IsExperimental:
        return el.flags.experimental;
    case IsInUse:
        return el.is_in_use();
    case MajorVersion:
        return el.version.major;
    case MinorVersion:
        return el.version.minor;
    case Category:
        if (el.is_in_use())
            return tr("In use");
        if (el.is_installed())
            return tr("Installed");
        if (el.is_lts())
            return tr("LTS");
        return tr("Other");
    case ChangelogUrl:
        return QString::fromStdString(el.changelog_url);
    case ExtraModules: {
        QStringList modules;
        for (const auto& mod : el.extra_modules) {
            modules.append(QString::fromStdString(mod));
        }
        return modules;
    }
    case KernelData: {
        mcp::qt::kernel::KernelData kernelData;
        kernelData.name = QString::fromStdString(el.package_name);
        kernelData.version = QString::fromStdString(el.version.to_string());
        kernelData.isInUse = el.is_in_use();
        kernelData.isRecommended = el.is_recommended();
        kernelData.isInstalled = el.is_installed();
        kernelData.majorVersion = el.version.major;
        kernelData.minorVersion = el.version.minor;
        kernelData.changelogUrl = QString::fromStdString(el.changelog_url);
        kernelData.isLTS = el.is_lts();
        
        QStringList extraModsList;
        for (const auto& mod : el.extra_modules) {
            extraModsList.append(QString::fromStdString(mod));
        }
        kernelData.extraModules = extraModsList;
        
        return QVariant::fromValue(kernelData);
    }
    }

    return QVariant{};
}

const std::vector<mcp::kernel::Kernel> &KernelListModel::list() const
{
    return m_list;
}

void KernelListModel::setList(const std::vector<mcp::kernel::Kernel> &newList)
{
    if (m_list == newList)
        return;
    beginResetModel();

    m_list = newList;

    auto installedEnd = std::partition(m_list.begin(), m_list.end(), [](const auto &a) {
        return a.is_in_use();
    });
    
    auto ltsStart = std::partition(installedEnd, m_list.end(), [](const auto &a) {
        return a.is_installed() && !a.is_in_use();
    });
    
    auto othersStart = std::partition(ltsStart, m_list.end(), [](const auto &a) {
        return !a.is_installed() && a.is_lts();
    });
    
    std::sort(ltsStart, othersStart, [](const auto &a, const auto &b) {
        return a.version > b.version;
    });
    std::sort(othersStart, m_list.end(), [](const auto &a, const auto &b) {
        return a.version > b.version;
    });

    m_filteredList.clear();
    for (const auto &kernel : m_list) {
        if (!kernel.is_in_use() && !kernel.is_recommended()) {
            m_filteredList.push_back(kernel);
        }
    }

    endResetModel();

    Q_EMIT listChanged();
}

void KernelListModel::setKernels(const std::vector<mcp::kernel::Kernel> &kernels)
{
    setList(kernels);
}

QHash<int, QByteArray> KernelListModel::roleNames() const
{
    QHash<int, QByteArray> result;
    auto enumeration = QMetaEnum::fromType<Role>();
    for (int i = 0; i < enumeration.keyCount(); i++) {
        result[i + ::Qt::UserRole + 1] = enumeration.key(i);
        auto &bytes = result[i + ::Qt::UserRole + 1];
        if (!bytes.isEmpty()) {
            bytes[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(bytes[0])));
        }
    }
    return result;
}
} // namespace mcp::qt::kernel
