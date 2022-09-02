#include "OCEAN.h"

OCEAN::OCEAN()
{}

OCEAN::OCEAN(float open, float conscientious, float extravert, float agreeable, float neurotic)
	: d_open(open), d_conscientious(conscientious), d_extravert(extravert), d_agreeable(agreeable), d_neurotic(neurotic)
{}

//[ ACCESSORS ]
float const OCEAN::open() const
{
	return d_open;
}

float const OCEAN::conscientious() const
{
	return d_conscientious;
}

float const OCEAN::extravert() const
{
	return d_extravert;
}

float const OCEAN::agreeable() const
{
	return d_agreeable;
}

float const OCEAN::neurotic() const
{
	return d_neurotic;
}

//[ MODIFIERS ]
void OCEAN::setOpen(float open)
{
	d_open = open;
}

void OCEAN::setConscientious(float conscientious)
{
	d_conscientious = conscientious;
}

void OCEAN::setExtravert(float extravert)
{
	d_extravert = extravert;
}

void OCEAN::setAgreeable(float agreeable)
{
	d_agreeable = agreeable;
}

void OCEAN::setNeurotic(float neurotic)
{
	d_neurotic = neurotic;
}

//[ OPERATORS ]
std::istream& operator>> (std::istream& is, OCEAN& ocean)
{
	is >> ocean.d_open;
	is >> ocean.d_conscientious;
	is >> ocean.d_extravert;
	is >> ocean.d_agreeable;
	is >> ocean.d_neurotic;
	return is;
}