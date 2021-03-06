
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2014 Igor Mironchik

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

// QtMWidgets include.
#include "lineedit.hpp"
#include "private/cursorshifter.hpp"

// Qt include.
#include <QMouseEvent>


namespace QtMWidgets {

//
// LineEditPrivate
//

class LineEditPrivate {
public:
	LineEditPrivate( LineEdit * parent )
		:	q( parent )
		,	shifter( 0 )
		,	mousePressed( false )
	{
	}

	void init();

	LineEdit * q;
	CursorShifter * shifter;
	bool mousePressed;
}; // class LineEditPrivate

void
LineEditPrivate::init()
{
	shifter = new CursorShifter( q );

	QObject::connect( shifter, &CursorShifter::posChanged,
		q, &LineEdit::_q_cursorShifterPosChanged );
}


//
// LineEdit
//

LineEdit::LineEdit( QWidget * parent )
	:	QLineEdit( parent )
	,	d( new LineEditPrivate( this ) )
{
	d->init();
}

LineEdit::LineEdit( const QString & text, QWidget * parent )
	:	QLineEdit( text, parent )
	,	d( new LineEditPrivate( this ) )
{
	d->init();
}

LineEdit::~LineEdit()
{
}

void
LineEdit::keyPressEvent( QKeyEvent * e )
{
	d->shifter->immediatelyHide();

	QLineEdit::keyPressEvent( e );
}

void
LineEdit::mousePressEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
		d->mousePressed = true;

	QLineEdit::mousePressEvent( e );
}

void
LineEdit::mouseReleaseEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		if( d->mousePressed && !isReadOnly() && !text().isEmpty() )
		{
			const QRect cr = cursorRect();

			const QPoint pos = mapToGlobal( QPoint( cr.center().x(),
				cr.y() + cr.height() ) );

			d->shifter->setCursorPos( pos );
			d->shifter->popup();
		}

		d->mousePressed = false;
	}

	QLineEdit::mouseReleaseEvent( e );
}

void
LineEdit::focusOutEvent( QFocusEvent * e )
{
	d->shifter->immediatelyHide();

	QLineEdit::focusOutEvent( e );
}

void
LineEdit::_q_cursorShifterPosChanged( const QPoint & pos )
{
	const int oldPos = cursorPosition();
	const int newPos = cursorPositionAt( mapFromGlobal( pos ) );

	if( oldPos != newPos )
	{
		setCursorPosition( newPos );

		repaint();

		const QRect cr = cursorRect();

		d->shifter->setCursorPos( mapToGlobal( QPoint( cr.center().x(),
			cr.y() + cr.height() ) ) );
	}
}

} /* namespace QtMWidgets */
