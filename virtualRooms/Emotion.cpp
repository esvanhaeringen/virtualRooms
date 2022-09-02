#include "Emotion.h"
#include <numeric>

using namespace std;

Emotion::Emotion()
{}

Emotion::Emotion(double valence, double arousal)
	: d_valence(valence), d_arousal(arousal)
{}

void Emotion::limit(double lower, double upper)
{
	d_valence = max(lower, min(upper, d_valence));
	d_arousal = max(lower, min(upper, d_arousal));
}

double Emotion::dist() const
{
	return sqrt(pow(d_valence, 2) + pow(d_arousal, 2));
}

double Emotion::distTo(Emotion const& emo) const
{
	return sqrt(pow(d_valence - emo.valence(), 2) + pow(d_arousal - emo.arousal(), 2));
}

//[ ACCESSORS ]
double const Emotion::valence() const
{
	return d_valence;
}

double const Emotion::arousal() const
{
	return d_arousal;
}

//[ MODIFIERS ]
void Emotion::set(double valence, double arousal)
{
	d_valence = valence;
	d_arousal = arousal;
}

void Emotion::setValence(double valence)
{
	d_valence = valence;
}

void Emotion::setArousal(double arousal)
{
	d_arousal = arousal;
}

//[ OPERATORS ]
void Emotion::operator=(Emotion const& emotion)
{
	setValence(emotion.valence());
	setArousal(emotion.arousal());
}

void Emotion::operator+=(Emotion const& emotion)
{
	d_arousal = std::min(std::max(d_arousal + emotion.arousal(), -1.0), 1.0);
	d_valence = std::min(std::max(d_valence + emotion.valence(), -1.0), 1.0);
}

void Emotion::operator-=(Emotion const& emotion)
{
	d_arousal = std::min(std::max(d_arousal - emotion.arousal(), -1.0), 1.0);
	d_valence = std::min(std::max(d_valence - emotion.valence(), -1.0), 1.0);
}

void Emotion::operator*=(Emotion const& emotion)
{
	d_arousal = std::min(std::max(d_arousal * emotion.arousal(), -1.0), 1.0);
	d_valence = std::min(std::max(d_valence * emotion.valence(), -1.0), 1.0);
}

void Emotion::operator/=(Emotion const& emotion)
{
	d_arousal = std::min(std::max(d_arousal / emotion.arousal(), -1.0), 1.0);
	d_valence = std::min(std::max(d_valence / emotion.valence(), -1.0), 1.0);
}

void Emotion::operator=(double const value)
{
	setValence(value);
	setArousal(value);
}

void Emotion::operator+=(double const value)
{
	d_arousal = std::min(std::max(d_arousal + value, -1.0), 1.0);
	d_valence = std::min(std::max(d_valence + value, -1.0), 1.0);
}

void Emotion::operator-=(double const value)
{
	d_arousal = std::min(std::max(d_arousal - value, -1.0), 1.0);
	d_valence = std::min(std::max(d_valence - value, -1.0), 1.0);
}

void Emotion::operator*=(double const value)
{
	d_arousal = std::min(std::max(d_arousal * value, -1.0), 1.0);
	d_valence = std::min(std::max(d_valence * value, -1.0), 1.0);
}

void Emotion::operator/=(double const value)
{
	d_arousal = std::min(std::max(d_arousal / value, -1.0), 1.0);
	d_valence = std::min(std::max(d_valence / value, -1.0), 1.0);
}

Emotion Emotion::operator+(double value)
{
	return Emotion(d_valence + value, d_arousal + value);
}

Emotion Emotion::operator+(Emotion const& emotion)
{
	return Emotion(d_valence + emotion.valence(), d_arousal + emotion.arousal());
}

Emotion operator+(double value, Emotion const& emotion)
{
	return Emotion(value + emotion.valence(), value + emotion.arousal());
}

Emotion Emotion::operator-(double value)
{
	return Emotion(d_valence - value, d_arousal - value);
}

Emotion Emotion::operator-(Emotion const& emotion)
{
	return Emotion(d_valence - emotion.valence(), d_arousal - emotion.arousal());
}

Emotion operator-(double value, Emotion const& emotion)
{
	return Emotion(value - emotion.valence(), value - emotion.arousal());
}

Emotion Emotion::operator*(double value)
{
	return Emotion(d_valence * value, d_arousal * value);
}

Emotion Emotion::operator*(Emotion const& emotion)
{
	return Emotion(d_valence * emotion.valence(), d_arousal * emotion.arousal());
}

Emotion operator*(double value, Emotion const& emotion)
{
	return Emotion(value * emotion.valence(), value * emotion.arousal());
}

Emotion Emotion::operator/(double value)
{
	return Emotion(d_valence / value, d_arousal / value);
}

Emotion Emotion::operator/(Emotion const& emotion)
{
	return Emotion(d_valence / emotion.valence(), d_arousal / emotion.arousal());
}

Emotion operator/(double value, Emotion const& emotion)
{
	return Emotion(value / emotion.valence(), value / emotion.arousal());
}

Emotion log(Emotion const& emotion)
{
	return Emotion(log(emotion.valence()), log(emotion.arousal()));
}

Emotion abs(Emotion const& emotion)
{
	return Emotion(abs(emotion.valence()), abs(emotion.arousal()));
}

double dist(Emotion const& emotion)
{
	return sqrt(pow(emotion.valence(), 2) + pow(emotion.arousal(), 2));
}

double dist(Emotion const& emotion1, Emotion const& emotion2)
{
	return sqrt(pow(emotion1.valence() - emotion2.valence(), 2) + pow(emotion1.arousal() - emotion2.arousal(), 2));
}

istream& operator>>(istream& is, Emotion& emotion)
{
	is >> emotion.d_valence;
	is >> emotion.d_arousal;
	return is;
}