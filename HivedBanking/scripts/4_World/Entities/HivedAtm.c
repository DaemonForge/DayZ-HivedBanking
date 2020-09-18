class HivedAtm extends BuildingSuper
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAccessHivedAccount);
	}
	
}