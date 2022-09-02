#pragma once
#include "DECADE.h"
#include "OCEAN.h"
#include <istream>
#include <vector>
#include <utility>

class DECADE;
class OCEAN;
class Model;
class Event;

class Agent
{
	Model& d_model;
	DECADE d_emotionMdl;
	OCEAN d_personalityMdl;
	std::string d_id;
	int d_idx = -1;
	int d_group = -1;
	int d_room = -1;
	std::vector<int> d_agentsInView;
	std::vector<std::pair<int, double>> d_relations;  //first is index of other agent, second is type/strength of relationship

public:
	Agent(Model& model, int idx);
	void updateView();
	void cognitiveAppraisal(Event const& event);
	void emotionContagion();
	void emotionGeneration();
	void emotionRegulation();
	void expressionGeneration();

	//[ ACCESSORS ]
	DECADE const& emotionMdl() const;
	OCEAN const& personalityMdl() const;
	std::string const& id() const;
	int const idx() const;
	int const group() const;
	int const room() const;
	std::vector<int> const& agentsInView() const;
	std::pair<int, double> const& relation(int idx) const;
	std::vector<std::pair<int, double>> const& relations() const;

	//[ MODIFIERS ]
	void setId(std::string id);
	void setIdx(int idx);
	void setGroup(int group);
	void setRoom(int room);
	void addRelation(int agent, double strength);

	//[ OPERATORS ]
	bool operator==(Agent const& other);
	friend std::istream& operator>>(std::istream& is, Agent& agent);
};