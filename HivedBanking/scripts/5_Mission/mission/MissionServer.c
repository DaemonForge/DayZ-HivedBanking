modded class MissionServer extends MissionBase
{
	void MissionServer(){	
		GetRPCManager().AddRPC( "HBANK", "RPCBankingtransaction", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "HBANK", "RPCReqPlayerBalance", this, SingeplayerExecutionType.Both );
	}
	
	override void OnInit(){
		super.OnInit();
	}
	
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if ( identity ){ //Make sure the server creates the default Config for the player
			BankAccounts().OnConnect(identity);
		}
	}
	
	void RPCBankingtransaction( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param3<string, string, float> data;  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		string GUID = data.param1;
		string TransactionType = data.param2;
		float TransactionValue = data.param3;
		
	}
	
	
	void RPCReqPlayerBalance( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param1<string> data;  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		string PlayerGUID = data.param1;
		if (sender.GetId() == PlayerGUID){
			
		}
	}
	
}
