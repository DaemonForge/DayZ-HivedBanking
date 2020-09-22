class HivedBankingModConfig extends UApiConfigBase
{ 
	string ConfigVersion = "1";
	
	string BankName = "Multi World Bank";
	
	float StartingBalance = 0;
	float StartingLimit = 1000000;
	bool CanDepositRuinedBills = false;
	
	ref array<ref HBMoneyValue> MoneyValues = new ref array<ref HBMoneyValue>;
	
	override void Load(){
		SetDataReceived(false);
		SetDefaults();
		UApi().Rest().GlobalsLoad("Banking", this, this.ToJson());
	}
	
	override void Save(){
		if (GetGame().IsServer()){	
			UApi().Rest().GlobalsSave("Banking", this.ToJson());
		}
	}
	
	override void OnDataReceive(){
		SetDataReceived();
		Print("Banking Data Received: " + this.ToJson());
		SortMoney();
	}
	
	
	override string ToJson(){
		string jsonString = JsonFileLoader<HivedBankingModConfig>.JsonMakeData(this);;
		return jsonString;
	}
	
	override void SetDefaults(){
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble100", 100));
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble50", 50));
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble25", 25));
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble10", 10));
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble5", 5));
		MoneyValues.Insert(new ref HBMoneyValue("MoneyRuble1", 1));
	}
	
	override void OnError(int errorCode) {
		Print("[BankingMod] CallBack Failed errorCode: " + errorCode);		
		SetDataReceived(true);
		BankName = "Bank Offline - " + errorCode;
	};
	
	override void OnTimeout() {
		Print("[BankingMod] CallBack Failed errorCode: Timeout");
		SetDataReceived(true);
		BankName = "Bank Offline - T";
		
	};
	
	override void OnSuccess(string data, int dataSize) {
		JsonFileLoader<HivedBankingModConfig>.JsonLoadData(data, this);
		if (this){
			OnDataReceive();
		} else {
			Print("[BankingMod] HivedBankingModConfig Failed errorCode: Invalid Data");
			BankName = "Bank Offline";
		}
		
	};
	
	void SortMoney(){
		array<HBMoneyValue> StartingValues =  new array<HBMoneyValue>;
		for (int h = 0; h < MoneyValues.Count(); h++){
			StartingValues.Insert(MoneyValues.Get(h));
		}
		ref array<ref HBMoneyValue> SortedMoneyValues = new ref array<ref HBMoneyValue>;
		while (StartingValues.Count() > 0){
			ref HBMoneyValue HighestValue = StartingValues.Get(0);
			for (int i = 1; i < StartingValues.Count(); i++){
				if (StartingValues.Get(i).Value > HighestValue.Value){
					HighestValue = StartingValues.Get(i);
				}
			}
			StartingValues.RemoveItem(HighestValue);
			SortedMoneyValues.Insert(HighestValue);
		}
		if (StartingValues.Count() == 1){
			SortedMoneyValues.Insert(StartingValues.Get(0));
		}
		MoneyValues = SortedMoneyValues;
	}
	
	HBMoneyValue GetHighestDenomination(float amount){
		int LastIndex = MoneyValues.Count() - 1;
		for (int i = 0; i < MoneyValues.Count(); i++){
			if (GetAmount(MoneyValues.Get(i), amount) > 0){
				return MoneyValues.Get(i);
			}
		}
		return NULL;
	}
	
	int GetAmount(HBMoneyValue MoneyObj, float amount){
		if (MoneyObj){
			return Math.Floor(amount / MoneyObj.Value);
		} 
		return 0;
	}
	
};
ref HivedBankingModConfig m_HivedBankingModConfig;
ref HivedBankingModConfig GetHivedBankingModConfig(bool RdyToLoad = false){
	if (!m_HivedBankingModConfig && RdyToLoad){
		 m_HivedBankingModConfig = new HivedBankingModConfig;
		 m_HivedBankingModConfig.Load();
	}
	return m_HivedBankingModConfig;
}



class HBMoneyValue{
	
	string Item;
	float Value
	
	void HBMoneyValue(string item, float value ){
		Item = item;
		Value = value;
	}
}

class HivedBankAccount extends UApiConfigBase {
	
	string ConfigVersion = "1";
	
	string GUID = "";
	string Name = "";
	string SteamID = "";
	float LimitBonus = 0;
	float Balance = 0;
	
	void LoadAccount(PlayerIdentity identity){
		GUID = identity.GetId();
		Name = identity.GetName();
		SteamID = identity.GetPlainId();
		
		SetDefaults();
		Load(GUID);
	}
	
	override void Load(string ID){
		SetDataReceived(false);
		UApi().Rest().PlayerLoad("Banking", ID, this, this.ToJson());
	}
	
	override void Save(){
		if (GetGame().IsServer()){
			UApi().Rest().PlayerSave("Banking", GUID, this.ToJson());
		}
	}
	
	override string ToJson(){
		string jsonString = JsonFileLoader<HivedBankAccount>.JsonMakeData(this);;
		bool ok = false;
		//Print("[UAPI] Debug HivedBankAccount:" + jsonString);
		return jsonString;
	}
	
	
	override void SetDefaults(){
		Balance = GetHivedBankingModConfig().StartingBalance;
	}
	
	
	override void OnError(int errorCode) {
		Print("[BankingMod] CallBack Failed errorCode: " + errorCode);		
	}
	
	override void OnTimeout() {
		Print("[BankingMod]CallBack Failed errorCode: Timeout");
		
	}
	
	override void OnSuccess(string data, int dataSize) {
		Print("[UAPI] Debug HivedBankAccount OnSuccess:" + data);
		JsonFileLoader<HivedBankAccount>.JsonLoadData(data, this);
		if (this.GUID != ""){
			OnDataReceive();
		} else {
			Print("[BankingMod] HivedBankAccount Failed errorCode: Invalid Data");
		}
	}
};