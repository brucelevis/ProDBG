#include <QtGui>
#include "Qt5HexEditWidget.h"
#include "Qt5HexEditInternal.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace prodbg
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Qt5HexEditWidget::Qt5HexEditWidget(QWidget* parent)
: QScrollArea(parent)
, m_internal(new Qt5HexEditInternal(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	
    setWidget(m_internal);
    setWidgetResizable(true);

    connect(m_internal, SIGNAL(currentAddressChanged(int)), this, SIGNAL(currentAddressChanged(int)));
    connect(m_internal, SIGNAL(currentSizeChanged(int)), this, SIGNAL(currentSizeChanged(int)));
    connect(m_internal, SIGNAL(overwriteModeChanged(bool)), this, SIGNAL(overwriteModeChanged(bool)));
    connect(m_internal, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    
    setFocusPolicy(Qt::NoFocus);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::insert(int index, const QByteArray& values)
{
	m_internal->insert(index, values);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::insert(int index, char value)
{
	m_internal->insert(index, value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Qt5HexEditWidget::indexOf(const QByteArray& values, int from) const
{
	return m_internal->indexOf(values, from);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Qt5HexEditWidget::lastIndexOf(const QByteArray& values, int from) const
{
	return m_internal->lastIndexOf(values, from);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::remove(int position, int length)
{
	m_internal->remove(position, length);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::replace(int position, int length, const QByteArray& values)
{
	m_internal->replace(position, length, values);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString Qt5HexEditWidget::getReadableString()
{
	return m_internal->getReadableString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString Qt5HexEditWidget::getReadableStringFromSelection()
{
	return m_internal->getReadableStringFromSelection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setAddressOffset(int offset)
{
	m_internal->setAddressOffset(offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Qt5HexEditWidget::getAddressOffset() const
{
	return m_internal->getAddressOffset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setCursorPosition(int cursorPosition)
{
	// The cursor position calculated in Qt5HexEditInternal is the
	// position without counting blanks. Need byte position * 2
	m_internal->setCursorPosition(cursorPosition * 2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Qt5HexEditWidget::getCursorPosition() const
{
	return m_internal->getCursorPosition() >> 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setData(const QByteArray& data)
{
	m_internal->setData(data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QByteArray Qt5HexEditWidget::getData() const
{
	return m_internal->getData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setAddressAreaColor(const QColor& color)
{
	m_internal->setAddressAreaColor(color);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor Qt5HexEditWidget::getAddressAreaColor() const
{
	return m_internal->getAddressAreaColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setHighlightingColor(const QColor& color)
{
	m_internal->setHighlightingColor(color);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor Qt5HexEditWidget::getHighlightingColor() const
{
	return m_internal->getHighlightingColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setSelectionColor(const QColor& color)
{
	m_internal->setSelectionColor(color);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor Qt5HexEditWidget::getSelectionColor() const
{
	return m_internal->getSelectionColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setOverwriteMode(bool mode)
{
	m_internal->setOverwriteMode(mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Qt5HexEditWidget::getOverwriteMode() const
{
	return m_internal->getOverwriteMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setReadOnly(bool mode)
{
	m_internal->setReadOnly(mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Qt5HexEditWidget::getReadOnly() const
{
	return m_internal->getReadOnly();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const QFont& Qt5HexEditWidget::getFont() const
{
	return m_internal->font();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setFont(const QFont& font)
{
	m_internal->setFont(font);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setAddressWidth(int addressWidth)
{
	m_internal->setAddressWidth(addressWidth);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setAddressArea(bool addressArea)
{
	m_internal->setAddressArea(addressArea);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setAsciiArea(bool asciiArea)
{
	m_internal->setAsciiArea(asciiArea);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::setHighlighting(bool mode)
{
	m_internal->setHighlighting(mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::undo()
{
	m_internal->undo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Qt5HexEditWidget::redo()
{
	m_internal->redo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
