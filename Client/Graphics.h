#pragma once

#include "TX_templates.h"

//basically ... singleton graphics currently
class FSC_Graphics;
extern FSC_Graphics TheGraphics;


DECLARE_TX_LOADER(ComponentPicture,TX_Structure)

class ComponentPicture : public ComponentPictureLoader
{
public:
	TX_String Component;
	TX_String FileName;
	//FreeStars::Component *m_pComponent;
	//wxBitmap *m_pBmp;
};


DECLARE_TX_LOADER(RaceEmbleme,TX_Structure)

class RaceEmbleme : public RaceEmblemeLoader
{
public:
	TX_Long ID;
	TX_String FileNameBig;
	//wxBitmap *m_pBmpBig;
};


extern const char ComponentPictureVectorName[];// = "ComponentPictures";
extern const char RaceEmblemeVectorName[];// = "RaceEmblemes";

class FSC_Graphics
{
	bool loaded;
	TX_Vector<ComponentPicture,ComponentPictureVectorName> ComponentPictures;
	TX_Vector<RaceEmbleme,RaceEmblemeVectorName> RaceEmblemes;

public:
	FSC_Graphics(void)
		:loaded(false)
		{}

	virtual ~FSC_Graphics(void)
		{}
	
	bool IsLoaded() const 
		{return loaded;}

	void Load(void);

	void EnsureLoaded(void) 
		{if (!IsLoaded()) Load();}

	const char *GetBigEmblemeName(int ID) const;

	const char *GetComponentPictureName(const FreeStars::Component *comp) const;

}; 




