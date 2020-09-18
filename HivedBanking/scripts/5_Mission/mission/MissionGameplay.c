modded class MissionGameplay extends MissionBase
{
	
	override void OnUpdate (float timeslice) {
        super.OnUpdate(timeslice);

        Input input = GetGame().GetInput();
        if (input.LocalPress("UAUIBack", false)) {
            if (m_HivedBankingPanel != NULL && GetGame().GetUIManager().GetMenu() == m_HivedBankingPanel) {
                HivedBankingClosePanel();
            }
		}
    }
	
	void HivedBankingClosePanel()
	{
		m_HivedBankingPanel.SetOpen(false);
        GetGame().GetUIManager().HideScriptedMenu(m_HivedBankingPanel);
		HivedBankingUnLockControls();
	}

    private void HivedBankingUnLockControls() {
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
    }
}