#include "View.h"
#include <algorithm>
#include "Model.h"
#include "Room.h"
#include <stdexcept>

#include <Windows.h>
#include <iostream>
#include <sstream>
#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

using namespace std;

View::View(Model* model)
	: d_model(model)
{}

void View::update()
{
	d_agentsInView.clear();
	std::vector<Agent*> occupants = d_room->agents();
	std::copy_if(occupants.begin(), occupants.end(), std::back_inserter(d_agentsInView), [this](Agent* agent) {return agent != d_self; });
}

//[ ACCESSORS ]
vector<Agent*>* View::agentsInView()
{
	return &d_agentsInView;
}

Room* const View::room() const
{
	return d_room;
}

Agent* View::self()
{
	return d_self;
}

//[ MODIFIERS ]
void View::setRoom(Room* room)
{
	d_room = room;
}

void View::setSelf(Agent* self)
{
	d_self = self;
}

//[ OPERATORS ]
istream& operator>> (istream& is, View& view)
{
	int roomId;
	is >> roomId;
	auto it = find_if(view.d_model->rooms()->begin(), view.d_model->rooms()->end(), [&roomId](const Room& room) {return room.id() == roomId; });
	if (it != view.d_model->rooms()->end())
	{
		it->add(view.d_self);
		view.d_room = &it[0];
	}
	else
		throw;
	return is;
}