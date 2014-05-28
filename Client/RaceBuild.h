#pragma once

class RaceBuild :
	public FreeStars::Player
{
public:
	RaceBuild(void);
	virtual ~RaceBuild(void);

	//these  should be part of rules
	wxString GetHabUnitString(int hab) const;
	wxString GetHabValueString(int hab, int value) const;
	wxColour GetHabColour(int hab) const;
	wxString GetFullHabString(int hab) const;
	long GetLeftoverStringCount() const	{ return 6; } //:)
	const wxString *GetLeftoverStrings() const;
	long GetTechCostStringsCount() const {return 3;}
	const wxString *GetTechCostStrings() const;

	bool IsAR() const	{return ARTechType() >= 0;}
	
	// would be easier if these TechCost levels were integers 0, 1, 2 etc.
	long GetIntTechCost(FreeStars::TechType tt) const; 
	void SetIntTechCost(FreeStars::TechType tt, long value);

	//more Setters/Getters missing 
	long GetRaceEmblem() {return mRaceEmblem;}
	void SetRaceEmblem(long value) {mRaceEmblem = value;}

	void SetSingleName(const char *name) {mSingularName = name;}
	void SetPluralName(const char *name) {mPluralName = name;}
	void SetStartAt(bool val) {mStartAt = val;}

	long GetLeftoverBuys() const	{ return mLeftoverBuys; }

	void AddLRT(const FreeStars::RacialTrait * lrt);
	void RemoveLRT(const FreeStars::RacialTrait * lrt);

	void SetCenter(int hab, int val){mHabCenter[hab]=val;}
	void SetWidth(int hab, int val) {mHabWidth[hab]=val;}

	// again ... integers would be easier
	long GetIntGrowthRate() const	{ return mGrowthRate*100; }
	void SetGrowth(int growth) {mGrowthRate = growth/100.0;}

	void SetPRT(const FreeStars::RacialTrait * Prt) {mPRT = Prt;}

	void SetIntPopEfficiency(int eff) {mPopEfficiency = eff*100;}
	void SetFactoryRate(int rate) {mFactoryRate = rate;}
	void SetFactoryCost(FreeStars::CargoType ct, int cost) {mFactoryCost[ct]=cost;}
	void SetFactoriesRun(int operated) {mFactoriesRun = operated;}
	void SetMineRate(int rate) {mMineRate = rate;}
	void SetMineCost(FreeStars::CargoType ct, int cost) {mMineCost[ct]=cost;}
	void SetMinesRun(int operated) {mMinesRun = operated;}

	void WriteRaceFile(const char*location);

};
