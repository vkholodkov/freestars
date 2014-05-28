<?xml version="1.0" standalone="yes"?>
<RaceDefinition>
<!-- FreeStars Race file -->
	<MetaInfo>
		<FileVersion>0.1</FileVersion>
		<FreeStarsVersion>0.1</FreeStarsVersion>
	<!-- Client should gracefully fail if version is greater than client, and hopefully import [if practical] if version is less. -->
	</MetaInfo>
    <SingularName>Gorgon</SingularName>
    <PluralName>Gorgons</PluralName>
    <Password>08sfsdDF490ad</Password>
	<Randomize>341</Randomize>
<!-- hash or whatever of the password -->
    <RaceEmblem>14</RaceEmblem>
<!-- Index of the picture to use. -->
    <PrimaryRacialTrait>Claim Adjuster</PrimaryRacialTrait>
    <LesserRacialTrait>No Ram Scoop Engines</LesserRacialTrait>
    <LesserRacialTrait>Only Basic Remote Mining</LesserRacialTrait>
    <LesserRacialTrait>No Advanced Scanners</LesserRacialTrait>
    <LesserRacialTrait>Total Terraforming</LesserRacialTrait>
    <HabSettings>
        <Hab Name="Gravity">
            <Center>10</Center>
            <Width>10</Width>
        </Hab>
        <Hab Name="Temperature">
            <Center>90</Center>
            <Width>10</Width>
        </Hab>
        <Hab Name="Radiation">
            <Center>90</Center>
            <Width>10</Width>
        </Hab>
    </HabSettings>
    <GrowthRate>0.18</GrowthRate>
    <PopEfficiency>1000</PopEfficiency>
    <FactoryRate>15</FactoryRate>
    <FactoryCost>
        <Resources>9</Resources>
        <Mineral Name="Ironium">0</Mineral>
        <Mineral Name="Boranium">0</Mineral>
        <Mineral Name="Germanium">4</Mineral>
        <Crew>0</Crew>
    </FactoryCost>
    <FactoriesRun>25</FactoriesRun>
    <MineRate>10</MineRate>
    <MineCost>
        <Resources>4</Resources>
        <Mineral Name="Ironium">0</Mineral>
        <Mineral Name="Boranium">0</Mineral>
        <Mineral Name="Germanium">0</Mineral>
        <Crew>0</Crew>
    </MineCost>
    <MinesRun>25</MinesRun>
    <TechFactor>
        <Tech Name="Energy">1.75</Tech>
        <Tech Name="Weapons">1.75</Tech>
        <Tech Name="Propulsion">1.75</Tech>
        <Tech Name="Construction">1.75</Tech>
        <Tech Name="Electronics">1.75</Tech>
        <Tech Name="Biotechnology">0.5</Tech>
    </TechFactor>
	<StartAt>true</StartAt>
    <StartMinerals>10</StartMinerals>
	<InitialSettings>
		<BattlePlan IDNumber="0">
			<Name>Default</Name>
			<PrimaryTarget>Armed</PrimaryTarget>
			<SecondaryTarget>Any</SecondaryTarget>
			<Tactic>MaximizeDamage</Tactic>
			<AttackWho>EnemiesNeutrals</AttackWho>
		</BattlePlan>
	</InitialSettings>
</RaceDefinition>
