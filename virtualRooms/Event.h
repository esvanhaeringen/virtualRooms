#pragma once
#include <string>
#include <istream>

class Model;

class Event
{
	Model& d_model;
	std::string d_id;
	std::string d_type;
	std::string d_level;
	std::string d_appliesTo;
	int d_timeStep = -1;
	int d_duration = -1;

public:
	Event(Model& model);
	Event(Model& model, std::string id, std::string type, std::string level, std::string appliesTo, int timeStep, int duration);

	//[ ACCESSORS ]
	std::string const& id() const;
	std::string const& type() const;
	std::string const& level() const;
	std::string const& appliesTo() const;
	int const timeStep() const;
	int const duration() const;
	int const end() const;

	//[ MODIFIERS ]
	void setId(std::string id);
	void setType(std::string type);
	void setLevel(std::string type);
	void setAppliesTo(std::string appliesTo);
	void setTimeStep(int timeStep);
	void setDuration(int duration);

	//[ OPERATORS ]
	void operator=(const Event& other);
	bool operator<(const Event& other) const;
	friend std::istream& operator>>(std::istream& is, Event& event);
};

