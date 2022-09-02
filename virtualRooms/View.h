#pragma once
#include <vector>
#include <istream>

class Agent;
class Model;
class Room;

class View
{
	Model* d_model;
	Agent* d_self = nullptr;
	std::vector<Agent*> d_agentsInView;
	Room* d_room;

public:
	View(Model* model);
	void update();

	//[ ACCESSORS ]
	std::vector<Agent*>* agentsInView();
	Room* const room() const;
	Agent* self();

	//[ MODIFIERS ]
	void setRoom(Room* room);
	void setSelf(Agent* self);

	//[ OPERATORS ]
	friend std::istream& operator>> (std::istream& is, View& view);
private:

};


//#pragma once
//#include "Agent.h"
//#include "Object.h"
//#include <vector>
//
//class View
//{
//	std::vector<Agent*> d_agentsInView;
//	std::vector<Object*> d_objectsInView;
//	double d_angle;
//	double d_distance;
//
//public:
//	View(double angle, double distance);
//	void update(std::vector<Agent>* agents, std::vector<Object>* objects);
//
//	std::vector<Agent*>* agentsInView();
//	std::vector<Object*>* objectsInView();
//
//private:
//
//};
//
//inline std::vector<Agent*>* View::agentsInView()
//{
//	return &d_agentsInView;
//}
//
//inline std::vector<Object*>* View::objectsInView()
//{
//	return &d_objectsInView;
//}