class HBTransactionCallBack : RestCallback {
	
	string GUID = "";
	float Amount = 0;
	
	int TransactionType = 0;
	ref HivedBankAccount Account;
	
	void SetTransaction (string guid, float amount, int transactionType, ref HivedBankAccount account){
		Print("Setting Transaction guid" + guid + " amount" + amount + " transactionType" + transactionType);
		GUID = guid;
		Amount = amount;
		TransactionType = transactionType;
		Account = account;
	}
	
	override void OnError(int errorCode) {
		DayZPlayer player = DayZPlayer.Cast(UApi().FindPlayer(GUID));
		Print("[HivedBanking] TransactionHandler OnError " + errorCode);
		if (player) {
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity) {
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, "Something went wrong") , true, identity);
			}
		}
	};
	
	override void OnTimeout() {
		Print("[HivedBanking] TransactionHandler OnError Timeout");
		DayZPlayer player = DayZPlayer.Cast(UApi().FindPlayer(GUID));
		if (player){
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity){
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, "Something went wrong") , true, identity);
			}
		}
	};
	
	override void OnSuccess(string data, int dataSize) {
		PlayerBase player = PlayerBase.Cast(UApi().FindPlayer(GUID)); 
		if (player){
			Print("[HivedBanking] TransactionHandler OnSuccess " + data + " TransactionType "+ TransactionType + " Amount " + Amount);
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity){
				JsonFileLoader<HivedBankAccount>.JsonLoadData(data, Account);
				if (TransactionType == HBConstants.Withdraw){
					player.HBAddMoney(Amount);
				} else if (TransactionType == HBConstants.Deposit){
					player.HBRemoveMoney(Amount);
				}
				GetRPCManager().SendRPC("HBANK", "RPCUpdateFromServer", new Param2<HivedBankAccount, string>(Account, "") , true, identity );
			}
		}
	};
	
};
