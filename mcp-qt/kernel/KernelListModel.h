/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <QAbstractListModel>

#include <kernel/Kernel.hpp>

namespace mcp::qt::kernel {

class KernelListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        Name = ::Qt::UserRole + 1,
        IsInstalled,
        Version,
        IsLTS,
        IsRecommended,
        IsRealTime,
        IsEOL,
        IsExperimental,
        IsInUse,
        MajorVersion,
        MinorVersion,
        Category,
        ChangelogUrl,
        ExtraModules
    };
    Q_ENUM(Role)

    explicit KernelListModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    const std::vector<mcp::kernel::Kernel> &list() const;
    void setList(const std::vector<mcp::kernel::Kernel> &newList);
    void setKernels(const std::vector<mcp::kernel::Kernel> &kernels);

Q_SIGNALS:
    void listChanged();

private:
    std::vector<mcp::kernel::Kernel> m_list;
    std::vector<mcp::kernel::Kernel> m_filteredList;

public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};

} // namespace mcp::qt::kernel
