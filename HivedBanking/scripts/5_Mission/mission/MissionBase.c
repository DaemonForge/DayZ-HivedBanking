modded class MissionBase extends MissionBaseWorld
{
	void MissionBase()
	{
		UApi();
	}
	
	override void UniversalApiReady(){
		super.UniversalApiReady();
		GetHivedBankingModConfig( true );
	}
	
	override UIScriptedMenu CreateScriptedMenu (int id) {
        UIScriptedMenu menu = NULL;
        menu = super.CreateScriptedMenu (id);
        if (!menu) {
            switch (id) {
                case HIVEDBANKING_PANEL_MENU:
                    menu = new HivedBankingWidget;
                    break;
            }
            if (menu) {
                menu.SetID (id);
            }
        }
        return menu;
    }
}