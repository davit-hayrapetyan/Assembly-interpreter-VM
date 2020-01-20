#include "CallStackWidget.h"

CCallStackWidget::CCallStackWidget(QWidget* pParent) 
	: QListView(pParent)
{
	m_pModel = new QStringListModel();
	setModel(m_pModel);
	connect(this, SIGNAL(doubleClicked(QModelIndex const&)), this, SLOT(onDoubleClicked(QModelIndex const&)));
}

void CCallStackWidget::addItem(QString const& sItem)
{
	m_lstItems.append(sItem);
	m_pModel->setStringList(m_lstItems);
}

void CCallStackWidget::clear()
{
	m_pModel->setStringList(QStringList());
}

void CCallStackWidget::onDoubleClicked(const QModelIndex &index)
{
	emit sigItemCliked(index.data(Qt::DisplayRole).toString());
}

