#include "DECADE.h"
#include "Model.h"
#include "OCEAN.h"
#include "Agent.h"
#include "Emotion.h"
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

DECADE::DECADE(Model& model, int idx)
	: d_model(model), d_idx(idx)
{}

void DECADE::appraise(Event const& event)
{
	//Emotional stimulus is drawn from a normal distribution of which the mean depends on the event type
	std::random_device dev;
	std::mt19937 rng(dev());
	if (event.type() == "win")
	{
		std::normal_distribution<> winVal(0.3, 0.2);
		std::normal_distribution<> winAro(0.3, 0.2);
		d_appraisal = Emotion(winVal(rng), winAro(rng));
	}
	else if (event.type() == "loss")
	{
		std::normal_distribution<> lossVal(0, 0.2);
		std::normal_distribution<> lossAro(0.3, 0.2);
		d_appraisal = Emotion(lossVal(rng), lossAro(rng));
	}
	else if (event.type() == "draw")
	{
		std::normal_distribution<> drawVal(0.0, 0.2);
		std::normal_distribution<> drawAro(0.3, 0.2);
		d_appraisal = Emotion(drawVal(rng), drawAro(rng));
	}
}

void DECADE::contagion()
{
	//Contagion only occurs if there are emotionally influential neighbours
	size_t othersInView = d_model.agent(d_idx).agentsInView().size();
	if (othersInView == 0)
		return;

	//[ CONNECTION STRENGTH ]
	//First we determine the attention for each neighbour and the total attention for all neighbours
	vector<double> thetas(othersInView);	
	for (int idx = 0; idx < othersInView; ++idx)
	{
		Agent const& other = d_model.agent(d_model.agent(d_idx).agentsInView()[idx]);
		if (d_attentionBiasStrength)
		{
			double valComp = abs(other.emotionMdl().expression().valence()) +
				d_attentionPrefVal * (1.0 - (1.0 + other.emotionMdl().expression().valence()) / 2.0) +
				(1.0 - d_attentionPrefVal) * (1.0 + other.emotionMdl().expression().valence()) / 2.0;
			double aroComp = abs(other.emotionMdl().expression().arousal()) +
				d_attentionPrefVal * (1.0 - (1.0 + other.emotionMdl().expression().arousal()) / 2.0) +
				(1.0 - d_attentionPrefVal) * (1.0 + other.emotionMdl().expression().arousal()) / 2.0;
			thetas.push_back(sqrt(valComp * valComp + aroComp * aroComp));
		}
		else
			thetas.push_back(other.emotionMdl().expressivity());
	}
	double const totalTheta = accumulate(thetas.begin(), thetas.end(), 0.0);
	
	//Then we calculate the individual and total channel strength (gamma in paper) between the receiver and its neighbours.
	vector<double> channelStrength;
	for (int idx = 0; idx < othersInView; ++idx)
	{
		double const connectionStrength = 1; //alpha in the paper
		double const k1 = 1;
		double const weightedAttention = (1.0 / othersInView + k1 * thetas[idx]) / (1.0 + k1 * totalTheta);
		channelStrength.push_back(weightedAttention * connectionStrength * d_susceptibility);
	}
	double const totalchannelStrength = accumulate(channelStrength.begin(), channelStrength.end(), 0.0);

	//[ EMOTIONAL INFLUENCE ]
	//Next we calculate the emotional influence of each dimension (e.g. valence) and the change in emotion due to contagion
	//For this the expression of the sender is first weighed against that of the other senders by the channel strength
	Emotion weightedGroupExpr(0.0, 0.0);
	for (int idx = 0; idx < othersInView; ++idx)
	{
		Agent const& other = d_model.agent(d_model.agent(d_idx).agentsInView()[idx]);
		weightedGroupExpr += (channelStrength[idx] / totalchannelStrength) * other.emotionMdl().expression();
	}

	//Then the influence is calculated per dimension, depending on which of the three conditions applies
	Emotion emotionalInfluence(0.0, 0.0);
	Emotion const PI = 1.0 - (1.0 - abs(weightedGroupExpr)) * (1.0 - abs(d_emotion));
	Emotion const NI = weightedGroupExpr * d_emotion;
	float amplification = (weightedGroupExpr.valence() + 1) / 2; //amplification or dampening depending on the valence direction of the weighted expression
	if (weightedGroupExpr.valence() * d_emotion.valence() >= 0)
	{
		emotionalInfluence.setValence(amplification * PI.valence() + (1.0 - amplification) * NI.valence());
		if (d_emotion.valence() < 0 || weightedGroupExpr.valence() < 0)
			emotionalInfluence.setValence(-emotionalInfluence.valence());
		emotionalInfluence.setValence(emotionalInfluence.valence() - d_emotion.valence());
	}
	else
		emotionalInfluence.setValence(amplification * weightedGroupExpr.valence() - (1.0 - amplification) * d_emotion.valence());
	if (weightedGroupExpr.arousal() * d_emotion.arousal() >= 0)
	{
		emotionalInfluence.setArousal(amplification * PI.arousal() + (1.0 - amplification) * NI.arousal());
		if (d_emotion.arousal() < 0 || weightedGroupExpr.arousal() < 0)
			emotionalInfluence.setArousal(-emotionalInfluence.arousal());
		emotionalInfluence.setArousal(emotionalInfluence.arousal() - d_emotion.arousal());
	}
	else
		emotionalInfluence.setArousal(amplification * weightedGroupExpr.arousal() - (1.0 - amplification) * d_emotion.arousal());

	//Lastly the emotional stimulus as the result of contagion is determined
	d_contagion = totalchannelStrength * emotionalInfluence;
}

void DECADE::generate()
{
	//Apply bias based on the personality of the agent
	d_appraisal *= (d_appraisal.valence() > 0) ? d_posStimulusBias : d_negStimulusBias;
	d_contagion *= (d_contagion.valence() > 0) ? d_posStimulusBias : d_negStimulusBias;

	//Apply emotional stimuli to the internal emotion of the agent
	d_emotion += d_appraisal;
	d_emotion += d_contagion;
	d_emotion.limit(-1, 1);
	d_appraisal.set(0, 0); //reset for next timestep
	d_contagion.set(0, 0);
}

void DECADE::regulate()
{
	
	d_regThreshold = 0.1;
	if (dist(d_emotion) > d_regThreshold)
	{
		d_emotion -= d_emotion * (1.0 + tanh(d_regTimestep - d_maxTime)) / 2;
		d_regTimestep = d_regTimestep + 1;
	}
	else
		d_regTimestep -= d_regTimestep / 2;
}

void DECADE::express()
{
	//Determine expression from internal emotion considering the personality of the agent
	d_expression = d_emotion * (0.5 + d_expressivity);
}

//[ ACCESSORS ]
Emotion const& DECADE::emotion() const
{
	return d_emotion;
}
Emotion const& DECADE::expression() const
{
	return d_expression;
}
std::string const& DECADE::emotionLabel() const
{
	int nearestLbl = 0;
	if (d_emotion.valence() > 0.2 || d_emotion.arousal() > 0.2)  //if outside of neutral area, find closest emotion label
	{
		double dist = -1.f;
		for (int lbl = 1; lbl < d_labelLoc.size(); ++lbl)
		{
			double tmp = d_emotion.distTo(d_labelLoc[lbl]);
			if (tmp < dist || dist == -1.f)
			{
				dist = tmp;
				nearestLbl = lbl;
			}
		}
	}
	return d_labels[nearestLbl];
}
double const DECADE::susceptibility() const
{
	return d_susceptibility;
}
double const DECADE::expressivity() const
{
	return d_expressivity;
}
double const DECADE::regulationEfficiency() const
{
	return d_regulationEffectiveness;
}
double const DECADE::posStimulusBias() const
{
	return d_posStimulusBias;
}
double const DECADE::negStimulusBias() const
{
	return d_negStimulusBias;
}
bool const DECADE::attentionBias() const
{
	return d_attentionBiasStrength;
}
double const DECADE::attPrefVal() const
{
	return d_attentionPrefVal;
}
double const DECADE::attPrefAro() const
{
	return d_attentionPrefAro;
}
int const DECADE::idx() const
{
	return d_idx;
}

//[ MODIFIERS ]
void DECADE::setEmotion(Emotion& emotion)
{
	d_emotion.setArousal(emotion.arousal());
	d_emotion.setValence(emotion.valence());
}
void DECADE::setSusceptibility(OCEAN const& ocean)
{
	d_susceptibility = max(0.0, min(1.0, ocean.open() * 0.35 + ocean.conscientious() * 0.18 + ocean.extravert() * 0.14 + ocean.agreeable() * 0.31 + ocean.neurotic() * 0.29));
}
void DECADE::setExpressivity(OCEAN const& ocean)
{
	d_expressivity = max(0.0, min(1.0, ocean.open() * 0.14 - ocean.conscientious() * 0.02 + ocean.extravert() * 0.32 + ocean.agreeable() * 0.11 + ocean.neurotic() * 0.29));
}
void DECADE::setRegulationEfficiency(OCEAN const& ocean)
{
	d_regulationEffectiveness = max(0.0, min(1.0, ocean.open() * 0.17 + ocean.conscientious() * 0.22 + ocean.extravert() * 0.19 + ocean.agreeable() * 0.45 - ocean.neurotic() * 0.23));
	std::random_device dev;
	std::mt19937 rng(dev());
	std::normal_distribution<> regulationDist(250 - 250 * d_regulationEffectiveness, 20);
	d_maxTime = regulationDist(rng);
}
void DECADE::setPositiveStimulusBias(OCEAN const& ocean)
{
	d_posStimulusBias = (1.0 + 0.44 * ocean.extravert() + 0.12 * ocean.agreeable() + 0.26 * ocean.conscientious() + 0.20 * ocean.open() - 0.29 * ocean.neurotic()) / 2;
}void DECADE::setNegativeStimulusBias(OCEAN const& ocean)
{
	d_negStimulusBias = (1.0 + 0.54 * ocean.neurotic() - 0.19 * ocean.agreeable() - 0.21 * ocean.conscientious() - 0.03 * ocean.open() - 0.18 * ocean.extravert()) / 2;
}
void DECADE::setAttentionBias(bool bias, double prefVal, double prefAro)
{
	d_attentionBiasStrength = bias;
	d_attentionPrefVal = prefVal;
	d_attentionPrefAro = prefAro;
}
void DECADE::setIdx(int idx)
{
	d_idx = idx;
}

//[ OPERATORS ]
std::istream& operator>>(std::istream& is, DECADE& decade)
{
	is >> decade.d_emotion;
	is >> decade.d_attentionBiasStrength;
	is >> decade.d_attentionPrefVal;
	is >> decade.d_attentionPrefAro;
	decade.setSusceptibility(decade.d_model.agent(decade.d_idx).personalityMdl());
	decade.setExpressivity(decade.d_model.agent(decade.d_idx).personalityMdl());
	decade.setRegulationEfficiency(decade.d_model.agent(decade.d_idx).personalityMdl());
	decade.setPositiveStimulusBias(decade.d_model.agent(decade.d_idx).personalityMdl());
	decade.setNegativeStimulusBias(decade.d_model.agent(decade.d_idx).personalityMdl());
	return is;
}