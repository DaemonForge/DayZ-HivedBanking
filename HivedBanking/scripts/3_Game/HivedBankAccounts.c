ref HivedBankAccounts m_HivedBankAccounts;
static HivedBankAccounts BankAccounts(){
	if (GetGame().IsServer() && !m_HivedBankAccounts){
		 m_HivedBankAccounts = new HivedBankAccounts;
		 m_HivedBankAccounts.Init();
	}
	return m_HivedBankAccounts;
}


class HivedBankAccounts{
	
	ref map<string, ref HivedBankAccount> m_BankAccounts = new map<string, ref HivedBankAccount>;	
	
	HivedBankAccount Get(string guid){
		return m_BankAccounts.Get(guid);
	}
	
	void Init(){
	
	}
		
	void OnConnect(string guid, string name, string steamid){
		if (!Get(guid)){
			ref HivedBankAccount tempAccount = new HivedBankAccount;
			tempAccount.LoadAccount(guid, name, steamid);
			Add(tempAccount);
		} else {
			Get(guid).LoadAccount(guid);
		}
	}
	
	void OnDisConnect(string guid){
		Save(guid);
	}
	
	void Add(HivedBankAccount account_data){
		if ( !Get(account_data.GUID) ){
			m_BankAccounts.Insert(account_data.GUID, account_data);
		} else { //Just make sure the data is reloaded from the server if trying to readd someone
			Get(account_data.GUID).Load(account_data.GUID);
		}
	}
	
	void Remove(string guid){
		if ( Get(guid) ){
			Save(guid);
		}
	}
	
	void Save(string guid){
		if ( !Get(guid) ){
		} else {
			Get(guid).Save();
		}
	}
}