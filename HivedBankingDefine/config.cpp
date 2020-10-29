class CfgPatches
{
	class HivedBankingDefine
	{
		requiredVersion=0.1;
		requiredAddons[]={
		};
	};
};

class CfgMods
{
	class HivedBankingDefine
	{
		dir="HivedBankingDefine";
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
					"HivedBankingDefine/scripts/Common"
					};
            };
			
			class worldScriptModule
            {
                value="";
                files[]={ 
					"HivedBankingDefine/scripts/Common"
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]={
					"HivedBankingDefine/scripts/Common"
				};
            };
        };
    };
};