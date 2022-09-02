#pragma once
#include <istream>

class OCEAN
{
	float d_open = 0.5;
	float d_conscientious = 0.5;
	float d_extravert = 0.5;
	float d_agreeable = 0.5;
	float d_neurotic = 0.5;

public:
	OCEAN();
	OCEAN(float open, float conscientious, float extravert, float agreeable, float neurotic);

	//[ ACCESSORS ]
	float const open() const;
	float const conscientious() const;
	float const extravert() const;
	float const agreeable() const;
	float const neurotic() const;

	//[ MODIFIERS ]
	void setOpen(float open);
	void setConscientious(float conscientious);
	void setExtravert(float extravert);
	void setAgreeable(float agreeable);
	void setNeurotic(float neurotic);

	//[ OPERATORS ]
	friend std::istream& operator>> (std::istream& is, OCEAN& ocean);
private:

};