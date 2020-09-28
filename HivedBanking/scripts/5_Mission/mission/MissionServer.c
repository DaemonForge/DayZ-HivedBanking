modded class MissionServer extends MissionBase
{
	void MissionServer(){	
		GetTransactionHandler();
	}	
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if ( identity ){ //Make sure the server creates the default Config for the player
			string guid = identity.GetId();
			string name = identity.GetName();
			string steamid = identity.GetPlainId();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(BankAccounts().OnConnect, 7 * 1000, false, guid, name, steamid );
		}
	}
	
	override void InvokeOnDisconnect( PlayerBase player )
	{
		if ( player.GetIdentity() ){
			string guid = player.GetIdentity().GetId();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(BankAccounts().OnDisConnect, 7 * 1000, false, guid);
		}
		
		super.InvokeOnDisconnect(player);
	}
	
}