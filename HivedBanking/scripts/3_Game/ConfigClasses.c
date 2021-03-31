class HivedBankingModConfig extends UApiConfigBase
{ 
	string ConfigVersion = "2";
	
	string BankName = "Multi World Bank";
	
	float StartingBalance = 0;
	float StartingLimit = 1000000;
	bool CanDepositRuinedBills = false;
	
	ref array<int> MenuThemeColour = {2, 136, 209};
	
	ref array<ref HBMoneyValue> MoneyValues = new array<ref HBMoneyValue>;
	
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
	
	void SaveTransaction(){
	
	}
	
	override void OnDataReceive(){
		SetDataReceived();
		if (GetGame().IsServer() && ConfigVersion != "2"){
			ConfigVersion = "2";
			MenuThemeColour = {2, 136, 209};
			Save();
		}
		SortMoney();
	}
	
	
	override string ToJson(){
		string jsonString = JsonFileLoader<HivedBankingModConfig>.JsonMakeData(this);;
		return jsonString;
	}
	
	override void SetDefaults(){
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble100", 100));
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble50", 50));
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble25", 25));
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble10", 10));
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble5", 5));
		MoneyValues.Insert(new HBMoneyValue("MoneyRuble1", 1));
	}
	
	override void OnError(int errorCode) {
		Print("[BankingMod] CallBack Failed errorCode: " + errorCode);		
		SetDataReceived(true);
		BankName = "Bank Offline - " + errorCode;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load, 300 * 1000, false);
	};
	
	override void OnTimeout() {
		Print("[BankingMod] CallBack Failed errorCode: Timeout");
		SetDataReceived(true);
		BankName = "Bank Offline - T";
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load, 300 * 1000, false);
	};
	
	override void OnSuccess(string data, int dataSize) {
		Print("[BankingMod] CallBack OnSuccess data" + data);
		JsonFileLoader<HivedBankingModConfig>.JsonLoadData(data, this);
		Print("[BankingMod] CallBack OnSuccess data Loaded");
		if (this){
			OnDataReceive();
		} else {
			Print("[BankingMod] HivedBankingModConfig Failed errorCode: Invalid Data");
			BankName = "Bank Offline";
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load, 300 * 1000, false);
		}
		
	};
	
	void SortMoney(){
		array<HBMoneyValue> StartingValues =  new array<HBMoneyValue>;
		for (int h = 0; h < MoneyValues.Count(); h++){
			StartingValues.Insert(MoneyValues.Get(h));
		}
		ref array<ref HBMoneyValue> SortedMoneyValues = new array<ref HBMoneyValue>;
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
	
	int GetThemeColour(){
        float r = MenuThemeColour[0];
        float g = MenuThemeColour[1];
        float b = MenuThemeColour[2];
		return ARGB(255, r, g, b);
	}
	
};

ref HivedBankingModConfig m_HivedBankingModConfig;
ref HivedBankingModConfig GetHivedBankingModConfig(){
	if (!m_HivedBankingModConfig){
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
	
	[NonSerialized()]
	bool RetryCount = 0;
	[NonSerialized()]
	int RetryType = 0;
	
	void LoadAccount(string guid, string name = "", string steamid = ""){
		SetDataReceived(false);
		GUID = guid;
		Name = name;
		SteamID = steamid;
		
		Balance = GetHivedBankingModConfig().StartingBalance;
		SetDefaults();
		Load(GUID);
	}
	
	override void Load(string ID){
		SetDataReceived(false);
		RetryType = 1;
		UApi().Rest().PlayerLoad("Banking", ID, this, this.ToJson());
	}
	
	override void Save(){
		if (GetGame().IsServer() && DataReceived() && RetryType != 1){ // Only if the data has been received should and its not currently trying to 
			RetryType = 2;
			UApi().Rest().PlayerSave("Banking", GUID, this.ToJson());
		}
	}
	
	override string ToJson(){
		string jsonString = JsonFileLoader<HivedBankAccount>.JsonMakeData(this);;
		bool ok = false;
		return jsonString;
	}
	
	
	override void SetDefaults(){
		Balance = GetHivedBankingModConfig().StartingBalance;
	}
	
	
	override void OnError(int errorCode) {
		Print("[BankingMod] CallBack Failed errorCode: " + errorCode);
		
		RetryCount++;
		
		if (RetryCount <= 5){
			if (RetryType == 1){
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load,  RetryCount * 90 * 1000, false, GUID);
			}
			if (RetryType == 2){
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Save, RetryCount * 90 * 1000, false, GUID);
			}
		}
		
	}
	
	override void OnTimeout() {
		Print("[BankingMod] CallBack Failed errorCode: Timeout");
		
		RetryCount++;
		
		if (RetryCount <= 5){
			if (RetryType == 1){
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load, RetryCount * 90 * 1000, false, GUID);
			}
			if (RetryType == 2){
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Save, RetryCount * 90 * 1000, false, GUID);
			}
		}
	}
	
	override void OnSuccess(string data, int dataSize) {
		JsonFileLoader<HivedBankAccount>.JsonLoadData(data, this);
		if (this.GUID != ""){
			OnDataReceive();
			RetryType = 0;
			RetryCount = 0;
		} else {
			Print("[BankingMod] HivedBankAccount Failed errorCode: Invalid Data");
			
			RetryCount++;
		
			if (RetryCount <= 5){
				if (RetryType == 1){
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Load, RetryCount * 90 * 1000, false, GUID);
				}
				if (RetryType == 2){
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Save, RetryCount * 90 * 1000, false, GUID);
				}
			}
		}
	}
	
	
};