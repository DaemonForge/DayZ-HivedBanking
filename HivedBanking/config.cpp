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
	class HivedAtm : HouseNoDestruct{
		scope = 1;
		displayName  = "Hived Atm";
		model = "HivedBanking\Data\HivedAtm.p3d";
		hiddenSelections[] = {        
			"zbytek"
		};
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\HivedAtm_co.paa" 
		};  
		
	};
	class HivedAtmSmall : HivedAtm{
		scope = 1;
		displayName  = "Hived Atm Small";
		model = "HivedBanking\Data\HivedAtm_Small.p3d";
		hiddenSelections[] = {        
			"zbytek", "full_atm"
		};
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_small_grey_co.paa", 
			"HivedBanking\Data\textures\atm_small_grey_co.paa" 
		};  
		
	};
	class HivedAtmSmall_Black : HivedAtmSmall{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_small_black_co.paa" , 
			"HivedBanking\Data\textures\atm_small_black_co.paa" 
		};  
		
	};
	class HivedAtmSmall_Blue : HivedAtmSmall{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_small_blue_co.paa" , 
			"HivedBanking\Data\textures\atm_small_blue_co.paa" 
		};  
		
	};
	class HivedAtmSmall_Green : HivedAtmSmall{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_small_green_co.paa" , 
			"HivedBanking\Data\textures\atm_small_green_co.paa" 
		};  
		
	};
	class HivedAtmSmall_Red : HivedAtmSmall{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_small_red_co.paa" , 
			"HivedBanking\Data\textures\atm_small_red_co.paa" 
		};  
		
	};
	class HivedAtmMedium : HivedAtm{
		scope = 1;
		displayName  = "Hived Atm Medium";
		model = "HivedBanking\Data\HivedAtm_Medium.p3d";
		hiddenSelections[] = {        
			"zbytek", "full_atm"
		};
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_medium_default_co.paa",
			"HivedBanking\Data\textures\atm_medium_default_co.paa" 
		};  
		
	};
	class HivedAtmMedium_Red : HivedAtmMedium{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_medium_red_co.paa",
			"HivedBanking\Data\textures\atm_medium_red_co.paa"
		};  
		
	};
	class HivedAtmMedium_Yellow : HivedAtmMedium{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_medium_yellow_co.paa",
			"HivedBanking\Data\textures\atm_medium_yellow_co.paa" 
		};  
		
	};
	class HivedAtmMedium_Purple : HivedAtmMedium{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_medium_purple_co.paa",
			"HivedBanking\Data\textures\atm_medium_purple_co.paa" 
		};  
		
	};
	class HivedAtmMedium_Blue : HivedAtmMedium{
		scope = 1;
		hiddenSelectionsTextures[] = {
			"HivedBanking\Data\textures\atm_medium_blue_co.paa",
			"HivedBanking\Data\textures\atm_medium_blue_co.paa" 
		};  
		
	};
};

