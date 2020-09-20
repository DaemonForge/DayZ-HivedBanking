ref HivedBankAccounts m_HivedBankAccounts;
ref HivedBankAccounts BankAccounts(){
	if (GetGame().IsServer() && !m_HivedBankAccounts){
		 m_HivedBankAccounts = new ref HivedBankAccounts;
		 m_HivedBankAccounts.Init();
	}
	return m_HivedBankAccounts;
}


class HivedBankAccounts{
	
	ref map<string, ref HivedBankAccount> m_BankAccounts = new ref map<string, ref HivedBankAccount>;	
	
	ref HivedBankAccount Get(string guid){
		return m_BankAccounts.Get(guid);
	}
	
	void Init(){
	
	}
	
	void SaveAll(){
		if (m_BankAccounts.Count() > 0){
			SaveNext(0);
		}
	}
	
	void SaveNext(int i){
		if (i < m_BankAccounts.Count()){
			m_BankAccounts.GetElement(i).Save();
			i++;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SaveNext, 50, false, i); //Making a Delayed Save List
		}
	}
	
	void OnConnect(PlayerIdentity identity){
		if (PlayerIdentity.Cast(identity) && !Get(identity.GetId())){
			ref HivedBankAccount tempAccount = new ref HivedBankAccount;
			tempAccount.LoadAccount(identity);
			Add(tempAccount);
		}
	}
	
	void OnDisConnect(PlayerIdentity identity){
		if (PlayerIdentity.Cast(identity)){
			Remove(identity.GetId());
		}
	}
	
	void Add(ref HivedBankAccount account_data){
		if ( !Get(account_data.GUID) ){
			m_BankAccounts.Insert(account_data.GUID, account_data);
		}
	}
	
	void Remove(string guid){
		if ( Get(guid) ){
			Get(guid).Save();
			m_BankAccounts.Remove(guid);
		}
	}
	
	void Save(string guid){
		if ( !Get(guid) ){
		} else {
			Get(guid).Save();
		}
	}
}