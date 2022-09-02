#pragma once
#include "Agent.h"
#include <vector>
#include <string>
#include <fstream>
#include <omp.h>

class Agent;
class Object;
class Config;

class Model
{
	std::vector<Agent> d_agents;
	std::vector<std::string> d_roomIds;
	std::vector<std::vector<int>> d_rooms;
	std::vector<std::string> d_groupIds;
	std::vector<Event> d_events;
	int d_timeStep = 0;
	int d_endStep = 0;
	std::string d_runName;
	std::string d_configPath;
	std::string d_outputPath;
	bool d_ready = false;
	bool d_contagion;

public:
	Model(std::string configPath);
	void step();
		
	//[ ACCESSORS ]
	std::vector<Agent> const& agents() const;
	Agent const& agent(int idx) const;
	Agent& agent(std::string const& id);
	std::vector<int> const& room(int idx) const;
	std::string const& roomId(int idx) const;
	std::vector<std::string> const& roomIds() const;
	std::string const& groupId(int idx) const;
	std::vector<std::string> const& groupIds() const;
	std::vector<Event> const& events() const;

	int const timeStep() const;
	int const endStep() const;
	int const nAgents() const;
	int const nGroups() const;
	int const nRooms() const;
	std::string const& outputPath() const;
	std::string const& runName() const;
	bool const ready() const;

	//[ MODIFIERS ]
	void addAgent(Agent const& agent);
	void addRoom(std::string const& id);
	void addGroup(std::string const& id);
	void addEvent(Event const& event);
	void addAgentToRoom(int agentIdx, int roomIdx);
	void removeAgentFromRoom(int agentIdx, int roomIdx);
	void addRelation(int agent1, int agent2, double strength);

private:
	void loadConfig();
	void parseModelData(std::ifstream& fs, std::string& line);
	void parseRoomData(std::ifstream& fs, std::string& line);
	void parseGroupData(std::ifstream& fs, std::string& line);
	void parseAgentData(std::ifstream& fs, std::string& line);
	void parseRelationData(std::ifstream& fs, std::string& line);
	void parseEventData(std::ifstream& fs, std::string& line);
	void sortEvents();
	void setupOutput();
	void writeLine();
	void applyEvents();

};

