#ifndef CallStackWidget_H
#define CallStackWidget_H

#include <QListView>
#include <QStringListModel>

class CCallStackWidget : public  QListView
{
	Q_OBJECT
public:
	CCallStackWidget(QWidget* pParent  = nullptr);
	
	void addItem(QString const& sItem);
	void clear();

signals:
	void sigItemCliked(QString const& sItem);

private	slots:
	void onDoubleClicked(const QModelIndex &index);

private:
	QStringListModel *m_pModel;
	QStringList m_lstItems;
};




#endif // CallStackWidget_H