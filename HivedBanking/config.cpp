class CfgPatches
{
	class HivedBanking
	{
		requiredVersion=0.1;
		requiredAddons[]={
			"UniversalApi"
		};
	};
};

class CfgMods
{
	class HivedBanking
	{
		dir="HivedBanking";
        name="HivedBanking";
        credits="DaemonForge";
        author="DaemonForge";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
	    dependencies[]={ "Game", "World", "Mission"};
	    class defs
	    {
			class gameScriptModule
            {
				value = "";
                files[]={
					"HivedBanking/scripts/Common",
					"HivedBanking/scripts/3_Game"
					};
            };
			
			class worldScriptModule
            {
                value="";
                files[]={ 
					"HivedBanking/scripts/Common",
					"HivedBanking/scripts/4_World" 
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]={
					"HivedBanking/scripts/Common",
					"HivedBanking/scripts/5_Mission" 
				};
            };
        };
    };
};
class CfgVehicles
{
	class HouseNoDestruct;
	class HivedAtm : HouseNoDestruct
	{
		scope = 1;
		displayName  = "Hived Atm";
		model = "HivedBanking\Data\HivedAtm.p3d";
		hiddenSelections[] = 
		{        
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"HivedBanking\Data\textures\HivedAtm_co.paa" 
		};  
		
	};
};

