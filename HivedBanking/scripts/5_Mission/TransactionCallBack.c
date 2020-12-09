class HBTransactionCallBack : RestCallback {
	
	string GUID = "";
	float Amount = 0;
	
	int TransactionType = 0;
	ref HivedBankAccount Account;
	
	void SetTransaction (string guid, float amount, int transactionType, ref HivedBankAccount account){
		GUID = guid;
		Amount = amount;
		TransactionType = transactionType;
		Account = account;
	}
	
	override void OnError(int errorCode) {
		DayZPlayer player = DayZPlayer.Cast(UApi().FindPlayer(GUID));
		Print("[HivedBanking] TransactionHandler OnError  GUID: " + GUID + " Error: " + errorCode);
		if (player) {
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity) {
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, "Something went wrong") , true, identity);
			}
		}
	};
	
	override void OnTimeout() {
		Print("[HivedBanking] TransactionHandler OnError  GUID: " + GUID + " Error: Timeout");
		DayZPlayer player = DayZPlayer.Cast(UApi().FindPlayer(GUID));
		if (player){
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity){
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, "Something went wrong") , true, identity);
			}
		}
	};
	
	override void OnSuccess(string data, int dataSize) {
		int MoneyError = 0;
		string ReturnError = "";
		PlayerBase player = PlayerBase.Cast(UApi().FindPlayer(GUID)); 
		if (player){
			Print("[HivedBanking] TransactionHandler OnSuccess " + GUID + " TransactionType " + TransactionType + " Amount " + Amount);
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity){
				JsonFileLoader<HivedBankAccount>.JsonLoadData(data, Account);
				if (TransactionType == HBConstants.Withdraw){
					MoneyError = player.HBAddMoney(Amount);
					
				} else if (TransactionType == HBConstants.Deposit){
					MoneyError = player.HBRemoveMoney(Amount);
				}
				if (MoneyError == 1){
					ReturnError = "Money spawned on ground";
				}
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, ReturnError) , true, identity );
			}
		}
	};
	
};
