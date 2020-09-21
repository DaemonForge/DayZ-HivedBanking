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
	
	
	override void InvokeOnDisconnect( PlayerBase player )
	{
		if ( player.GetIdentity() ){
			BankAccounts().OnDisConnect(player.GetIdentity());
		}
		
		super.InvokeOnDisconnect(player);
	}
	
	void RPCBankingtransaction( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		PlayerIdentity identity = PlayerIdentity.Cast(sender);
		Param3<string, string, float> data;  //Player ID, Icon
		if ( !ctx.Read( data ) && !identity ) return;
		string GUID = data.param1;
		string TransactionType = data.param2;
		float TransactionValue = data.param3;
		if (GUID == identity.GetId()){
			PlayerBase player = PlayerBase.Cast(UApi().FindPlayer(GUID));
			if (player && TransactionType == "WITHDRAW"){
				HBRequestWithdraw( player, TransactionValue );
			}
			if ( player && TransactionType == "DEPOSIT"){
				HBRequestDeposit( player, TransactionValue );
			}
		}
	}
	
	
	void RPCReqPlayerBalance( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ){
		PlayerIdentity identity;
		Param1<string> data;  //Player ID, Icon
		if ( !ctx.Read( data ) && !Class.CastTo(identity, sender) ) return;
		string PlayerGUID = data.param1;
		PlayerBase player = PlayerBase.Cast(UApi().FindPlayer(PlayerGUID));
		if (player && identity.GetId() == PlayerGUID){
			float PlayerBalance = player.HBGetPlayerBalance();
			GetRPCManager().SendRPC("HBANK", "RPCReceivePlayerAmmount", new Param2<string, float>(identity.GetId(), PlayerBalance) , true, identity);
		}
	}

	void HBRequestWithdraw( PlayerBase player, float amount ){
		PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
		if (identity){
			ref HivedBankAccount Account = BankAccounts().Get(identity.GetId());
			if (Account){
				string WarningMessage ="";
				if (Account.Balance < amount){
					WarningMessage = "Insufficient Funds";
					amount = Account.Balance;
				}
				Account.Balance = Account.Balance - amount;
				Account.Save();
				player.HBAddMoney(amount);
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, WarningMessage) , true, identity);
			}
		}
	}
	
	void HBRequestDeposit( PlayerBase player, float amount ){
		PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
		if (identity){
			ref HivedBankAccount Account = BankAccounts().Get(identity.GetId());
			if (Account){
				float PlayerBalance = player.HBGetPlayerBalance();
				string WarningMessage ="";
				if ( PlayerBalance < amount ){
					amount = PlayerBalance;
					WarningMessage = "Don't have that much on you";
				}
				Account.Balance = Account.Balance + amount;
				Account.Save();
				player.HBRemoveMoney(amount);
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, WarningMessage) , true, identity);
			}
		}
		
	}
}