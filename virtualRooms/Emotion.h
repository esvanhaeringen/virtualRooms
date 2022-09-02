#pragma once
#include <string>
#include <vector>
#include <istream>

class Emotion
{
	double d_valence = 0.0;
	double d_arousal = 0.0;

public:
	Emotion();
	Emotion(double valence, double arousal);
	double dist() const;
	double distTo(Emotion const& emo) const;
	void limit(double lower, double upper);

	//[ ACCESSORS ]
	double const valence() const;
	double const arousal() const;

	//[ MODIFIERS ]
	void set(double valence, double arousal);
	void setValence(double valence);
	void setArousal(double arousal);

	//[ OPERATORS ]
	void operator=(Emotion const& emotion);
	void operator+=(Emotion const& emotion);
	void operator-=(Emotion const& emotion);
	void operator*=(Emotion const& emotion);
	void operator/=(Emotion const& emotion);
	void operator=(double const value);
	void operator+=(double const value);
	void operator-=(double const value);
	void operator*=(double const value);
	void operator/=(double const value);
	Emotion operator+(double value);
	Emotion operator+(Emotion const& emotion);
	friend Emotion operator+(double value, Emotion const& emotion);
	Emotion operator-(double value);
	Emotion operator-(Emotion const& emotion);
	friend Emotion operator-(double value, Emotion const& emotion);
	Emotion operator*(double value);
	Emotion operator*(Emotion const& emotion);
	friend Emotion operator*(double value, Emotion const& emotion);
	Emotion operator/(double value);
	Emotion operator/(Emotion const& emotion);
	friend Emotion operator/(double value, Emotion const& emotion);
	friend std::istream& operator>>(std::istream& is, Emotion& emotion);
	friend Emotion log(Emotion const& emotion);
	friend Emotion abs(Emotion const& emotion);
	friend double dist(Emotion const& emotion);
	friend double dist(Emotion const& emotion1, Emotion const& emotion2);
};