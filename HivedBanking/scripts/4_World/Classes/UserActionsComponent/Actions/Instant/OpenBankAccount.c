class ActionAccessHivedAccount extends ActionInteractBase
{

	void ExpansionActionOpenTraderMenu(){
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
	}
	
	override string GetText(){
		return "Access Account";
	}

	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ){	
		return true;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		super.OnExecuteServer( action_data );
		PlayerBase player = PlayerBase.Cast( action_data.m_Player);
		if (player && GetGame().IsServer()){
			PlayerIdentity identity = PlayerIdentity.Cast(player.GetIdentity());
			if (identity){ //Make sure server has most recent copy of the Banking Data
				BankAccounts().Get(identity.GetId()).Load(identity.GetId());
			}
		}
	}

	override void OnExecuteClient( ActionData action_data ){
		super.OnExecuteClient( action_data );
		if (m_HivedBankingPanel) {
            if (m_HivedBankingPanel.IsOpen()){
            } else if (GetGame().GetUIManager().GetMenu() == NULL) {
				if (!m_HivedBankingPanel_Opening){
					GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(this.OpenPanel);
				}
				m_HivedBankingPanel_Opening = true;
            }
        } else if (GetGame().GetUIManager().GetMenu() == NULL && m_HivedBankingPanel == NULL) {
			//Wait a bit before opening so that way player data is received
			if (!m_HivedBankingPanel_Opening){
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(this.CreatePanel);
			}
			m_HivedBankingPanel_Opening = true;
       	}
	}
	
	void OpenPanel(){
		GetGame().GetUIManager().ShowScriptedMenu(m_HivedBankingPanel, NULL);
	    m_HivedBankingPanel.SetOpen(true);
	    m_HivedBankingPanel.HivedBankingLockControls();
		m_HivedBankingPanel_Opening = false;
	}
	
	void CreatePanel(){
	    m_HivedBankingPanel = HivedBankingWidget.Cast(GetGame().GetUIManager().EnterScriptedMenu(HIVEDBANKING_PANEL_MENU, null));
	    m_HivedBankingPanel.SetOpen(true);
	    m_HivedBankingPanel.HivedBankingLockControls();
		m_HivedBankingPanel_Opening = false;
	}
};