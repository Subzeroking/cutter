#include "utils/Helpers.h"
#include "ResourcesWidget.h"
#include <QVBoxLayout>

ResourcesModel::ResourcesModel(QList<ResourcesDescription> *resources, QObject *parent)
    : QAbstractListModel(parent),
      resources(resources)
{
}

int ResourcesModel::rowCount(const QModelIndex &) const
{
    return resources->count();
}

int ResourcesModel::columnCount(const QModelIndex &) const
{
    return Columns::COUNT;
}

QVariant ResourcesModel::data(const QModelIndex &index, int role) const
{
    const ResourcesDescription *res = &resources->at(index.row());
    switch (role)
    {
        case Qt::DisplayRole:
            switch (index.column())
            {
                case NAME:
                    return res->name;
                case VADDR:
                    return RAddressString(res->vaddr);
                case INDEX:
                    return res->index;
                case TYPE:
                    return res->type;
                case SIZE:
                    return qhelpers::formatBytecount(res->size);
                case LANG:
                    return res->lang;
                default:
                    return QVariant();
            }
        default:
            return QVariant();
    }
}

QVariant ResourcesModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case NAME:
            return tr("Name");
        case VADDR:
            return tr("Vaddr");
        case INDEX:
            return tr("Index");
        case TYPE:
            return tr("Type");
        case SIZE:
            return tr("Size");
        case LANG:
            return tr("Lang");
        default:
            return QVariant();
        }
    default:
        return QVariant();
    }
}

void ResourcesModel::beginReload()
{
    beginResetModel();
}

void ResourcesModel::endReload()
{
    endResetModel();
}

ResourcesWidget::ResourcesWidget(QWidget *parent)
    : QDockWidget(parent)
{
    model = new ResourcesModel(&resources, this);

    // Configure widget
    this->setWindowTitle(tr("Resources"));

    // Add resources tree view
    view = new QTreeView(this);
    view->setModel(model);
    view->show();
    this->setWidget(view);

    connect(Core(), SIGNAL(refreshAll()), this, SLOT(refreshResources()));
    connect(view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onDoubleClicked(const QModelIndex &)));
}

void ResourcesWidget::refreshResources()
{
    model->beginReload();
    resources = Core()->getAllResources();
    model->endReload();
}

void ResourcesWidget::onDoubleClicked(const QModelIndex &index)
{
    ResourcesDescription res = index.data().value<ResourcesDescription>();
    CutterCore::getInstance()->seek(res.vaddr);
}
