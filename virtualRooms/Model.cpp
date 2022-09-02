#include "Model.h"
#include <sstream>
#include <algorithm>
#include <string>

using namespace std;

Model::Model(string configPath)
	:d_configPath(configPath)
{
	loadConfig();
	if (d_outputPath != "")
		setupOutput();
	d_ready = true;
}

void Model::step()
{
	d_timeStep += 1;
	if (d_events.size() > 0) //appraise external events if applicable
		applyEvents(); 
	for (int idx = 0; idx < d_agents.size(); ++idx)
	{
		d_agents[idx].updateView(); //selects emotionally relevant neighbours for contagion
		if (d_contagion)
			d_agents[idx].emotionContagion();
		d_agents[idx].emotionGeneration();
		d_agents[idx].emotionRegulation();
	}
	for (int idx = 0; idx < d_agents.size(); ++idx) //second loop to synchronize agents before updating expressions 
		d_agents[idx].expressionGeneration();

	if (d_outputPath != "")
		writeLine();
}

void Model::setupOutput()
{
	ofstream outputFile;
	outputFile.open(d_outputPath);

	outputFile << "step" << ',';
	outputFile << "idx" << ',';
	outputFile << "id" << ',';
	outputFile << "room" << ',';
	outputFile << "group" << ',';
	outputFile << "intValence" << ',';
	outputFile << "intArousal" << ',';
	outputFile << "expValence" << ',';
	outputFile << "expArousal" << ',';
	outputFile << "label" << '\n';

	outputFile.close();
}

void Model::writeLine()
{
	ofstream outputFile;
	outputFile.open(d_outputPath, std::ios_base::app);

	for (int idx = 0; idx != d_agents.size(); ++idx) //one agent per row
	{
		outputFile << d_timeStep << ',';
		outputFile << idx << ',';
		outputFile << d_agents[idx].id() << ',';
		outputFile << d_roomIds[d_agents[idx].room()] << ',';
		outputFile << d_groupIds[d_agents[idx].group()] << ',';
		outputFile << d_agents[idx].emotionMdl().emotion().valence() << ',';
		outputFile << d_agents[idx].emotionMdl().emotion().arousal() << ',';
		outputFile << d_agents[idx].emotionMdl().expression().valence() << ',';
		outputFile << d_agents[idx].emotionMdl().expression().arousal() << ',';
		outputFile << d_agents[idx].emotionMdl().emotionLabel() << '\n';
	}
	outputFile.close();
}

void Model::applyEvents()
{
	int idx = 0;
	while (d_timeStep >= d_events[idx].timeStep() and d_timeStep < d_events[idx].end())
	{
		if (d_events[idx].level() == "group")
		{
			for (int j = 0; j != d_agents.size(); ++j)
				if (d_groupIds[d_agents[j].group()] == d_events[idx].appliesTo())
					d_agents[j].cognitiveAppraisal(d_events[idx]);
		}
		else if (d_events[idx].level() == "room")
		{
			for (int j = 0; j != d_agents.size(); ++j)
				if (d_roomIds[d_agents[j].room()] == d_events[idx].appliesTo())
					d_agents[j].cognitiveAppraisal(d_events[idx]);
		}
		else if (d_events[idx].level() == "agent")
			agent(d_events[idx].appliesTo()).cognitiveAppraisal(d_events[idx]);
		if (d_timeStep + 1 == d_events[idx].end()) //event has reached end -> remove and continue with next event, if any
		{
			d_events.erase(d_events.begin() + idx);
			if (d_events.empty())
				return;
		}
		else
			++idx;
	}
}

void Model::sortEvents()
{
	std::sort(d_events.begin(), d_events.end());
}

void Model::loadConfig()
{
	ifstream fs(d_configPath);
	if (fs)
	{
		string line{};
		while (getline(fs, line))
		{
			if (line.find("MODEL {") != string::npos)
				parseModelData(fs, line);
			else if (line.find("ROOMS {") != string::npos)
				parseRoomData(fs, line);
			else if (line.find("GROUPS {") != string::npos)
				parseGroupData(fs, line);
			else if (line.find("AGENTS {") != string::npos)
				parseAgentData(fs, line);
			else if (line.find("EVENTS {") != string::npos)
				parseEventData(fs, line);
		}
		fs.close();
	}
	else
	{
		//error file cannot be opened
	}
}

void Model::parseModelData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		istringstream is(line);
		string tmp;
		is >> d_runName >> d_outputPath >> d_endStep >> tmp;
		d_contagion = tmp == "true" ? true : false;
	}
}

void Model::parseRoomData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		addRoom(line);
	}
}

void Model::parseGroupData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		addGroup(line);
	}
}

void Model::parseAgentData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		istringstream is(line);
		addAgent(Agent(*this, nAgents()));
		is >> d_agents.back();
	}
}

void Model::parseRelationData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		istringstream is(line);
		string agentId1, agentId2;
		double strength1, strength2;
		is >> agentId1 >> agentId2 >> strength1 >> strength2;
		addRelation(agent(agentId1).idx(), agent(agentId2).idx(), strength1);
		addRelation(agent(agentId2).idx(), agent(agentId1).idx(), strength2);
	}
}

void Model::parseEventData(ifstream& fs, string& line)
{
	getline(fs, line); //ignore header line
	while (getline(fs, line))
	{
		if (line.find('}') != string::npos)
			return;
		istringstream is(line);
		addEvent(Event(*this));
		is >> d_events.back();
	}
	sortEvents();
}

//[ ACCESSORS ]
std::vector<Agent> const& Model::agents() const
{
	return d_agents;
}
Agent const& Model::agent(int idx) const
{
	return d_agents[idx];
}
Agent& Model::agent(std::string const& id)
{
	auto it = find_if(d_agents.begin(), d_agents.end(), [id](Agent const& agent) { return agent.id() != id; });
	if (it != d_agents.end())
		return (*it);
	else
		throw;
}
vector<int> const& Model::room(int idx) const
{
	return d_rooms[idx];
}
string const& Model::roomId(int idx) const
{
	return d_roomIds[idx];
}
vector<string> const& Model::roomIds() const
{
	return d_roomIds;
}
string const& Model::groupId(int idx) const
{
	return d_groupIds[idx];
}
vector<string> const& Model::groupIds() const
{
	return d_groupIds;
}
vector<Event> const& Model::events() const
{
	return d_events;
}
int const Model::timeStep() const
{
	return d_timeStep;
}
int const Model::nAgents() const
{
	return d_agents.size();
}
int const Model::nGroups() const
{
	return d_groupIds.size();
}
int const Model::nRooms() const
{
	return d_rooms.size();
}
int const Model::endStep() const
{
	return d_endStep;
}
string const& Model::outputPath() const
{
	return d_outputPath;
}
string const& Model::runName() const
{
	return d_runName;
}
bool const Model::ready() const
{
	return d_ready;
}

//[ MODIFIERS ]
void Model::addAgent(Agent const& agent)
{
	d_agents.push_back(agent);
}
void Model::addRoom(string const& id)
{
	d_roomIds.push_back(id);
	d_rooms.push_back(vector<int>());
}
void Model::addGroup(string const& id)
{
	d_groupIds.push_back(id);
}
void Model::addEvent(Event const& event)
{
	d_events.push_back(event);
}
void Model::addAgentToRoom(int agentIdx, int roomIdx)
{
	d_rooms[roomIdx].push_back(agentIdx);
}
void Model::removeAgentFromRoom(int agentIdx, int roomIdx)
{
	d_rooms[roomIdx].erase(find(d_rooms[roomIdx].begin(), d_rooms[roomIdx].end(), agentIdx));
}
void Model::addRelation(int agent1, int agent2, double strength)
{
	d_agents[agent1].addRelation(agent2, strength);
}