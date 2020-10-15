class HivedAtm extends BuildingSuper {
	override void SetActions(){
		super.SetActions();
		AddAction( ActionAccessHivedAccount );
	}
}

class HivedAtmSmall extends HivedAtm {}

class HivedAtmMedium extends HivedAtm {}

class HivedAtmMedium_Red extends HivedAtmMedium {}

class HivedAtmMedium_Yellow extends HivedAtmMedium {}

class HivedAtmMedium_Purple extends HivedAtmMedium {}

class HivedAtmMedium_Blue extends HivedAtmMedium {}