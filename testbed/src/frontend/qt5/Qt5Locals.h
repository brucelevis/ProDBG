#pragma once

#include <QTreeWidget>

struct PDLocals;

namespace prodbg
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Qt5Locals : public QTreeWidget
{
	Q_OBJECT
public:
	Qt5Locals(QWidget* parent);

	void updateLocals(PDLocals* locals, int entires);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

