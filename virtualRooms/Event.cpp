#include "Event.h"
#include "Model.h"
using namespace std;

Event::Event(Model& model)
	: d_model(model)
{}

Event::Event(Model& model, string id, string type, string level, string appliesTo, int timeStep, int duration)
	: d_model(model), d_id(id), d_type(type), d_level(level), d_appliesTo(appliesTo), d_timeStep(timeStep), d_duration(duration)
{}

//[ ACCESSORS ]
string const& Event::id() const
{
	return d_id;
}
string const& Event::type() const
{
	return d_type;
}
string const& Event::level() const
{
	return d_level;
}
string const& Event::appliesTo() const
{
	return d_appliesTo;
}
int const Event::timeStep() const
{
	return d_timeStep;
}
int const Event::duration() const
{
	return d_duration;
}
int const Event::end() const
{
	return d_timeStep + d_duration;
}

//[ MODIFIERS ]
void Event::setId(string id)
{
	d_id = id;
}
void Event::setType(string type)
{
	d_type = type;
}
void Event::setLevel(string level)
{
	d_level = level;
}
void Event::setAppliesTo(string appliesTo)
{
	d_appliesTo = appliesTo;
}
void Event::setTimeStep(int timeStep)
{
	d_timeStep = timeStep;
}
void Event::setDuration(int duration)
{
	d_duration = duration;
}

//[ OPERATORS ]
void Event::operator=(const Event& other)
{
	d_id = other.id();
	d_type = other.type();
	d_level = other.level();
	d_appliesTo = other.appliesTo();
	d_timeStep = other.timeStep();
	d_duration = other.duration();
}

bool Event::operator< (const Event& other) const
{
	return d_timeStep < other.timeStep();
}
std::istream& operator>>(std::istream& is, Event& event)
{
	is >> event.d_id;
	is >> event.d_type;
	is >> event.d_level;
	is >> event.d_appliesTo;
	is >> event.d_timeStep;
	is >> event.d_duration;
	return is;
}