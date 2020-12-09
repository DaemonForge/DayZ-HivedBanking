/*

This Class Handles the Banking Transactions and ensure that the data is saved to the bank before affecting the players inventory

*/

class HBTransactionHandler{
	
	void Init(){
		Print("[HivedBanking] Init TransactionHandler");
		GetRPCManager().AddRPC( "HBANK", "RPCBankingtransaction", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "HBANK", "RPCReqPlayerBalance", this, SingeplayerExecutionType.Both );
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
				RequestWithdraw( player, TransactionValue );
			}
			if ( player && TransactionType == "DEPOSIT"){
				RequestDeposit( player, TransactionValue );
			}
		}
	}
	
	
	void RPCReqPlayerBalance( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ){
		PlayerIdentity identity = PlayerIdentity.Cast(sender);
		Param1<string> data;  //Player ID, Icon
		if ( !ctx.Read( data ) && sender ) return;
		string PlayerGUID = data.param1;
		PlayerBase player = PlayerBase.Cast(UApi().FindPlayer(PlayerGUID));
		if (player && identity){
			if (identity.GetId() == PlayerGUID){
				float PlayerBalance = player.HBGetPlayerBalance();
				GetRPCManager().SendRPC("HBANK", "RPCReceivePlayerAmmount", new Param2<string, float>(PlayerGUID, PlayerBalance) , true, identity);
			}
		}
	}
	
	
	void RequestWithdraw( PlayerBase player, float amount ){
		PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
		if (identity){
			ref HivedBankAccount Account = BankAccounts().Get(identity.GetId());
			if (Account){
				//Print("[HivedBanking] TransactionHandler RequestWithdraw " + amount);
				string WarningMessage ="";
				if (!Account.DataReceived()){
					WarningMessage = "Something went wrong";
				}
				if (Account.DataReceived() && Account.Balance < amount){
					WarningMessage = "Insufficient Funds";
					amount = Account.Balance;
				}
				if (Account.DataReceived()){
					Transaction(Account, HBConstants.Withdraw, amount);
				}
				if (WarningMessage != ""){
					GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(NULL, WarningMessage) , true, identity);
				}
			}
		}
	}
	
	void RequestDeposit( PlayerBase player, float amount ){
		PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
		if (identity){
			ref HivedBankAccount Account = BankAccounts().Get(identity.GetId());
			if (Account){
				//Print("[HivedBanking] TransactionHandler RequestDeposit " + amount);
				float PlayerBalance = player.HBGetPlayerBalance();
				string WarningMessage ="";
				
				if (!Account.DataReceived()){
					WarningMessage = "Something went wrong";
				}
				
				if ( PlayerBalance < amount ){
					amount = PlayerBalance;
					WarningMessage = "Don't have that much on you";
				}
				float NewBalance = Account.Balance + amount;
				float AccountLimit = GetHivedBankingModConfig().StartingLimit + Account.LimitBonus;
				if (NewBalance > AccountLimit){
					float OverLimit = NewBalance - AccountLimit;
					amount = amount - OverLimit;
					WarningMessage = "Trying to deposit more than your limit";
				}
				if (Account.DataReceived() && amount > 0){
					Transaction(Account, HBConstants.Deposit, amount);
					GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(NULL, WarningMessage) , true, identity);
				} else if (WarningMessage != ""){
					GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, WarningMessage) , true, identity);
				}				
			}
		}
		
	}
	
	void Transaction(ref HivedBankAccount account, int transactionType, float amount){
		if (!account.DataReceived()){
			Print("[HivedBanking] There was an error with an transaction");
			return;
		}
		Print("[HivedBanking] TransactionHandler Transaction: " + account.GUID + " transactionType: " + transactionType + " Amount: " + amount);
		float orgBalance = account.Balance;
		ref HBTransactionCallBack transactionCallBack = new ref HBTransactionCallBack;
		transactionCallBack.SetTransaction(account.GUID, amount, transactionType, account);
		if (transactionType == HBConstants.Withdraw){
			account.Balance = orgBalance - amount;
		} else if (transactionType == HBConstants.Deposit) {
			account.Balance = orgBalance + amount;
		}
		UApi().Rest().PlayerSave("Banking", account.GUID, account.ToJson(), transactionCallBack);
		account.Balance = orgBalance;
	}
}


ref HBTransactionHandler m_TransactionHandler;
ref HBTransactionHandler GetTransactionHandler(){
	if (!m_TransactionHandler){
		 m_TransactionHandler = new HBTransactionHandler;
		 m_TransactionHandler.Init();
	}
	return m_TransactionHandler;
}