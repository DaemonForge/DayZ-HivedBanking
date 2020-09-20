ref HivedBankingWidget m_HivedBankingPanel;
bool m_HivedBankingPanel_Opening = false;

class HivedBankingWidget extends UIScriptedMenu
{	
	protected ref HivedBankAccount		g_BankAccount;
	protected bool						m_PanelIsOpen = false; 
	protected bool						m_IsInitialized = false;
	
	protected ref Widget				m_BankingBoarder;
	protected ref Widget				m_BankingPanel;
	protected ref TextWidget			m_Heading;
	protected ref TextWidget			m_BankBalance;
	protected ref TextWidget			m_BankLimit;
	protected ref TextWidget			m_PlayerBalance;
	protected ref TextWidget			m_WarningMessage;
	
	
	override Widget Init()
    {
		if (!m_IsInitialized){
			Initialize();
		}
		
		layoutRoot 					= GetGame().GetWorkspace().CreateWidgets("HivedBanking/GUI/Layouts/Banking.layout");
		m_BankingBoarder			= Widget.Cast( layoutRoot.FindAnyWidget( "BankingBoarder" ) );
		m_BankingPanel	        	= Widget.Cast( layoutRoot.FindAnyWidget( "BankingPanel" ) );
		m_Heading	    			= TextWidget.Cast( layoutRoot.FindAnyWidget( "Heading" ) );
		m_BankBalance	   	 		= TextWidget.Cast( layoutRoot.FindAnyWidget( "BankBalance" ) );
		m_BankLimit	   	 			= TextWidget.Cast( layoutRoot.FindAnyWidget( "Limit" ) );
		m_PlayerBalance	   	 		= TextWidget.Cast( layoutRoot.FindAnyWidget( "PlayerBalance" ) );
		m_WarningMessage	   	 	= TextWidget.Cast( layoutRoot.FindAnyWidget( "Warning_Message" ) );
		ClearWarning();
		m_BankingBoarder.Show(false);
		m_BankingPanel.Show(false);
		return layoutRoot;
    }

	void Initialize(){
		m_IsInitialized = true;
		GetRPCManager().AddRPC( "HBANK", "RPCUpdateFromServer", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "HBANK", "RPCReceivePlayerAmmount", this, SingeplayerExecutionType.Both );
	}
	
	void RPCUpdateFromServer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param2<HivedBankAccount, string> data;  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		g_BankAccount = data.param1;
		string WarningMessage = data.param2;
		if (WarningMessage != ""){
			DoWarning(WarningMessage);
		} else {
			ClearWarning();
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(g_BankAccount.LoadAccount, 200, false, sender);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckForData, 400, false);
		GetRPCManager().SendRPC("HBANK", "RPCReqPlayerBalance", new Param1<string>(g_BankAccount.GUID) , true);
	}
	
	void RPCReceivePlayerAmmount( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param2<string, float> data;  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		string GUID = data.param1;
		float PlayerAmmount = data.param2;
		Print("[HivedBanking] ReceivePlayerAmmount " + PlayerAmmount);
		m_PlayerBalance.SetText("On You: $" + MakeNiceString(PlayerAmmount));
	}
	
	void DoWarning(string warningMessage){
		m_WarningMessage.SetText(warningMessage);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ClearWarning, 15 * 1000, false);
	}
	
	void ClearWarning(){
		m_WarningMessage.SetText("");
	}
	
	void ~HivedBankingWidget()
    {
		layoutRoot.Show(false);
		m_BankingBoarder.Show(false);
		m_BankingPanel.Show(false);
		m_BankingBoarder.SetAlpha(0);
		m_BankingPanel.SetAlpha(0);
		CloseBanking();
    }
	

	void BankingInit()	
	{
		GetRPCManager().SendRPC("HBANK", "RPCReqPlayerBalance", new Param1<string>(g_BankAccount.GUID) , true);
		HivedBankingLockControls();
		Print("BankingWidget Init");
		PlayerIdentity identity = PlayerIdentity.Cast(GetGame().GetPlayer().GetIdentity());
		if (identity){
			m_BankLimit.SetText("Limit: $" + MakeNiceString(GetHivedBankingModConfig().StartingLimit));
			m_Heading.SetText(GetHivedBankingModConfig().BankName);
			g_BankAccount = new ref HivedBankAccount;
			g_BankAccount.LoadAccount(identity);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckForData, 200, false);
		}
	}
	
	void CheckForData(){
		if (m_PanelIsOpen && g_BankAccount.DataReceived()){
			m_BankBalance.SetText("$" + MakeNiceString(g_BankAccount.Balance));
			m_BankLimit.SetText("Limit: $" + MakeNiceString(GetHivedBankingModConfig().StartingLimit + g_BankAccount.LimitBonus));
		} else if (m_PanelIsOpen){
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckForData, 200, false);
		}
	}
	
	bool IsOpen() {
		return m_PanelIsOpen;
	}

	void SetOpen(bool open) {
		m_BankingBoarder.Show(open);
		m_BankingPanel.Show(open);
		m_PanelIsOpen = open;
		if (m_PanelIsOpen){
			BankingInit();
		} else {
			CloseBanking();
		}
	}
	
	void UpdateData(){
	
	}
	
	void CloseBanking(){
		HivedBankingUnLockControls();
	}
	
	string MakeNiceString(int DollarAmount){
		string NiceString = "";
		string OrginalString = DollarAmount.ToString();
		if (OrginalString.Length() <= 3){
			return OrginalString;
		} 
		Print("MakeNiceString ORG: "  + DollarAmount);
		int StrLen = OrginalString.Length() - 4;
		NiceString = OrginalString.Substring(StrLen,3);
		Print("MakeNiceString NiceString: "  + NiceString);
		while (StrLen > 3){
			StrLen = StrLen - 3;
			NiceString = OrginalString.Substring(StrLen,3) + "," + NiceString;
			Print("MakeNiceString NiceString: "  + NiceString);
		}
		NiceString = OrginalString.Substring(0,StrLen) + "," + NiceString;
		Print("MakeNiceString NiceString: "  + NiceString);
		
		return NiceString;
	}
	
	void HivedBankingLockControls() {
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_MOUSE_ALL);
        GetGame().GetUIManager().ShowUICursor(true);
    }

    void HivedBankingUnLockControls() {
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
    }
}