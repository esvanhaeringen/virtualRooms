#pragma once
#include <vector>
#include <istream>
#include "Emotion.h"
#include "Event.h"

class Emotion;
class OCEAN;
class Agent;
class Model;

class DECADE
{
	Model& d_model;
	int d_idx;
	Emotion d_emotion;
	Emotion d_appraisal;
	Emotion d_contagion;
	Emotion d_expression;
	double d_susceptibility;
	double d_expressivity;
	double d_regulationEffectiveness;
	double d_posStimulusBias;
	double d_negStimulusBias;
	bool d_attentionBiasStrength = true;
	double d_attentionPrefVal;
	double d_attentionPrefAro;
	double d_regThreshold;
	float d_regTimestep = 0;
	float d_maxTime;

	std::vector<std::string> d_labels = { 
		"Neutral",
		"Pleased/relaxed",
		"Sleepy/peaceful",
		"Tired/bored",
		"Miserable/sad",
		"Distressed/frustrated",
		"Angry/afraid",
		"Excited/aroused",
		"Happy/delighted" };
	std::vector<Emotion> const d_labelLoc = {
		Emotion(0.000000000,  0.000000000),			// Neutral
		Emotion(0.923879533, -0.382683432),  		// Pleased/relaxed
		Emotion(0.382683432, -0.923879533),  		// Sleepy/peaceful
		Emotion(-0.382683432, -0.923879533), 		// Tired/bored
		Emotion(-0.923879533, -0.382683432), 		// Miserable/sad
		Emotion(-0.923879533, 0.382683432),			// Distressed/frustrated
		Emotion(-0.382683432, 0.923879533), 		// Angry/afraid
		Emotion(0.382683432, 0.923879533), 			// Aroused/excited
		Emotion(0.923879533, 0.382683432)}; 		// Happy/delighted
	

public:
	DECADE(Model& model, int idx);
	void appraise(Event const& event);
	void contagion();
	void generate();
	void regulate();
	void express();

	//[ ACCESSORS ]
	Emotion const& emotion() const;
	Emotion const& expression() const;
	std::string const& emotionLabel() const;
	double const susceptibility() const;
	double const expressivity() const;
	double const regulationEfficiency() const;
	double const posStimulusBias() const;
	double const negStimulusBias() const;
	bool const attentionBias() const;
	double const attPrefVal() const;
	double const attPrefAro() const;
	int const idx() const;

	//[ MODIFIERS ]
	void setEmotion(Emotion& emotion);
	void setSusceptibility(OCEAN const& ocean);
	void setExpressivity(OCEAN const& ocean);
	void setRegulationEfficiency(OCEAN const& ocean);
	void setPositiveStimulusBias(OCEAN const& ocean);
	void setNegativeStimulusBias(OCEAN const& ocean);
	void setAttentionBias(bool bias, double prefVal, double prefAro);
	void setIdx(int idx);

	//[ OPERATORS ]
	friend std::istream& operator>> (std::istream& is, DECADE& decade);
private:
};