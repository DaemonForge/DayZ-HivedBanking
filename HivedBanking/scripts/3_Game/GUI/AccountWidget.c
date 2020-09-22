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
	protected ref EditBoxWidget			m_Amount;
	
	protected ref ButtonWidget			m_DepositButton;
	protected ref ButtonWidget			m_WithdrawButton;
	
	
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
		m_Amount	   	 			= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "Amount" ) );
		
		m_DepositButton	   	 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "Deposit_Button" ) );
		m_WithdrawButton	   	 	= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "Withdraw_Button" ) );
		
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
		
		GetGame().GetPlayer().UpdateInventoryMenu();
	}
	
	void RPCReceivePlayerAmmount( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param2<string, float> data;  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		string GUID = data.param1;
		float PlayerAmmount = data.param2;
		Print("[HivedBanking] ReceivePlayerAmmount " + PlayerAmmount);
		m_PlayerBalance.SetText("On You: $" + MakeNiceString(PlayerAmmount));
		
		GetGame().GetPlayer().UpdateInventoryMenu();
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
		if (UApi().HasValidAuth()){
			HivedBankingLockControls();
			Print("BankingWidget Init");
			PlayerIdentity identity = PlayerIdentity.Cast(GetGame().GetPlayer().GetIdentity());
			if (identity){
				GetRPCManager().SendRPC("HBANK", "RPCReqPlayerBalance", new Param1<string>(identity.GetId()) , true);
				m_BankLimit.SetText("Limit: $" + MakeNiceString(GetHivedBankingModConfig().StartingLimit));
				m_Heading.SetText(GetHivedBankingModConfig().BankName);
				g_BankAccount = new ref HivedBankAccount;
				g_BankAccount.LoadAccount(identity);
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckForData, 200, false);
			}
		} else {
			m_Heading.SetText("BANK IS CURRENTLY OFFLINE");
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
	
	float SnapValue( float value){
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float min = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		float MinValue = value / min;
		float Value = Math.Round(MinValue);
		return Value * min;
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
		string StrSelection = OrginalString.Substring(StrLen,3);
		NiceString = StrSelection;
		while (StrLen > 3){
			StrLen = StrLen - 3;
			StrSelection = OrginalString.Substring(StrLen,3);
			NiceString = StrSelection + "," + NiceString;
		}
		StrSelection = OrginalString.Substring(StrLen,3);
		NiceString = StrSelection + "," + NiceString;
		
		return DollarAmount.ToString();
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if (!UApi().HasValidAuth()){
			DoWarning("Sorry Bank is currently Offline");
			return super.OnClick(w, x, y, button);
		}
		if (w == m_DepositButton){
			float DepositAmount = 0;
			DepositAmount = m_Amount.GetText().ToFloat();
			if (DepositAmount > 0){
				GetRPCManager().SendRPC("HBANK", "RPCBankingtransaction", new Param3<string, string, float>(g_BankAccount.GUID,"DEPOSIT", SnapValue(m_Amount.GetText().ToFloat())) , true);
			} else {
				DoWarning("Invalid Amount");
			}
			return true;
		}
		if (w == m_WithdrawButton){
			float WithdrawAmount = 0;
			WithdrawAmount = m_Amount.GetText().ToFloat();
			if (WithdrawAmount > 0){
				GetRPCManager().SendRPC("HBANK", "RPCBankingtransaction", new Param3<string, string, float>(g_BankAccount.GUID,"WITHDRAW", SnapValue(m_Amount.GetText().ToFloat())) , true);
			}else {
				DoWarning("Invalid Amount");
			}
			return true;
		}
		return super.OnClick(w, x, y, button);
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_Amount){
			string amount = CheckInput(m_Amount.GetText());
        	m_Amount.SetText(amount);
			return true;
        }
        return super.OnChange(w,x,y,finished);
	}
	
	string CheckInput(string input)
    {
		string amount = "";
		for (int i = 0; i<input.Length(); i++){
			if (ValidInput(input.Get(i))){
				amount = amount + input.Get(i);
			}
		}
		return amount;
    }
	
	bool ValidInput(string input)
    {
		if (input.Contains("0") || input.Contains("1") || input.Contains("2") || input.Contains("3") || input.Contains("4") || input.Contains("5") || input.Contains("6") || input.Contains("7") || input.Contains("8") || input.Contains("9") || input.Contains(".")){
			return true;
		}
		return false;
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