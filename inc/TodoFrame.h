#ifndef _TODO_FRAME_H_
#define _TODO_FRAME_H_

#include <FBase.h>
#include <FUi.h>


class TodoFrame
	: public Tizen::Ui::Controls::Frame
{
public:
	TodoFrame(void);
	virtual ~TodoFrame(void);

public:
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
};

#endif  // _TODO_FRAME_H_
