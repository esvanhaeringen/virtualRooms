#include "agent.h"
#include "Model.h"
#include "Event.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Agent::Agent(Model& model, int idx)
	: d_model(model), d_idx(idx), d_emotionMdl(DECADE(model, idx))
{}
void Agent::updateView()
{
	d_agentsInView.clear();
	copy_if(d_model.room(d_room).begin(), d_model.room(d_room).end(), back_inserter(d_agentsInView), [this](int idx) { return idx != d_idx; });
}
void Agent::cognitiveAppraisal(Event const& event)
{
	d_emotionMdl.appraise(event);
}
void Agent::emotionContagion()
{
	d_emotionMdl.contagion();
}
void Agent::emotionGeneration()
{
	d_emotionMdl.generate();
}
void Agent::emotionRegulation()
{
	d_emotionMdl.regulate();
}
void Agent::expressionGeneration()
{
	d_emotionMdl.express();
}

//[ ACCESSORS ]
DECADE const& Agent::emotionMdl() const
{
	return d_emotionMdl;
}
OCEAN const& Agent::personalityMdl() const
{
	return d_personalityMdl;
}
string const& Agent::id() const
{
	return d_id;
}
int const Agent::idx() const
{
	return d_idx;
}
int const Agent::group() const
{
	return d_group;
}
int const Agent::room() const
{
	return d_room;
}
vector<int> const& Agent::agentsInView() const
{
	return d_agentsInView;
}
pair<int, double> const& Agent::relation(int idx) const
{
	return d_relations[idx];
}
vector<pair<int, double>> const& Agent::relations() const
{
	return d_relations;
}

//[ MODIFIERS ]
void Agent::setId(string id)
{
	d_id = id;
}
void Agent::setIdx(int idx)
{
	d_idx = idx;
}
void Agent::setGroup(int group)
{
	d_group = group;
}
void Agent::setRoom(int room)
{
	if (d_room > -1)
		d_model.removeAgentFromRoom(d_idx, d_room);
	d_model.addAgentToRoom(d_idx, room);
	d_room = room;
}
void Agent::addRelation(int agent, double strength)
{
	d_relations.push_back(pair<int, double>(agent, strength));
}

//[ OPERATORS ]
bool Agent::operator==(Agent const& other)
{
	return d_idx == other.d_idx;
}

std::istream& operator>>(std::istream& is, Agent& agent)
{
	is >> agent.d_id;
	string groupId;
	is >> groupId;
	auto itG = find(agent.d_model.groupIds().begin(), agent.d_model.groupIds().end(), groupId);
	if (itG != agent.d_model.groupIds().end())
	{
		agent.d_group = itG - agent.d_model.groupIds().begin();
	}
	string roomId;
	is >> roomId;
	auto itR = find(agent.d_model.roomIds().begin(), agent.d_model.roomIds().end(), roomId);
	if (itR != agent.d_model.roomIds().end())
	{
		agent.d_room = itR - agent.d_model.roomIds().begin();
		agent.d_model.addAgentToRoom(agent.d_idx, agent.d_room);
	}
	is >> agent.d_personalityMdl;
	is >> agent.d_emotionMdl;
	return is;
}