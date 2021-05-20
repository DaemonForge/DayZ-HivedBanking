modded class MissionGameplay extends MissionBase
{
	
	override void OnUpdate (float timeslice) {
        super.OnUpdate(timeslice);

        Input input = GetGame().GetInput();
        if (input.LocalPress("UAUIBack", false)) {
            if ( GetGame().GetUIManager().IsMenuOpen(HIVEDBANKING_PANEL_MENU) || (m_HivedBankingPanel != NULL && GetGame().GetUIManager().GetMenu() == m_HivedBankingPanel)) {
                HivedBankingClosePanel();
            }
		}
    }
	
	void HivedBankingClosePanel()
	{
		if (m_HivedBankingPanel){
			m_HivedBankingPanel.SetOpen(false);
		}
		GetGame().GetUIManager().CloseMenu(HIVEDBANKING_PANEL_MENU);
		HivedBankingUnLockControls();
	}

    private void HivedBankingUnLockControls() {
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
    }
}