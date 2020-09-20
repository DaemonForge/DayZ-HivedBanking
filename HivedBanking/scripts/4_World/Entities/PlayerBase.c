modded class PlayerBase extends ManBase
{
	bool HBCanAccept(ItemBase item){
		return !item.IsRuined();
	}
	
	
	float HBGetPlayerBalance(){
		float PlayerBalance = 0;
		
		array<EntityAI> inventory = new array<EntityAI>;
		this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventory);
		
		ItemBase item;
		for (int i = 0; i < inventory.Count(); i++){
			if (Class.CastTo(item, inventory.Get(i))){
				for (int j = 0; j < GetHivedBankingModConfig().MoneyValues.Count(); j++){
					if (item.GetType() == GetHivedBankingModConfig().MoneyValues.Get(j).Item && HBCanAccept(item)){
						PlayerBalance += HBCurrentQuantity(item) * GetHivedBankingModConfig().MoneyValues.Get(j).Value;
					}
				}
			}
		}
		return PlayerBalance;
	}
	
	
	
	
	int HBAddMoney(float Amount){
		if (Amount <= 0){
			return 0;
		}
		int Return = 0;
		float AmountToAdd = Amount;
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float SmallestCurrency = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		float PlayerBalance = HBGetPlayerBalance();
		float OptimalPlayerBalance = PlayerBalance + AmountToAdd;
		
		HBMoneyValue MoneyValue = GetHivedBankingModConfig().GetHighestDenomination(AmountToAdd);
		while (MoneyValue && PlayerBalance < OptimalPlayerBalance && NoError){
			int AmountToSpawn = GetHivedBankingModConfig().GetAmount(MoneyValue,AmountToAdd);
			if (AmountToSpawn == 0){
				NoError = false;
			}
			HBCreateMoneyInventory(MoneyValue.Item, AmountToSpawn);
			PlayerBalance = HBGetPlayerBalance();
			AmountToAdd = OptimalPlayerBalance - PlayerBalance;
			HBMoneyValue NewMoneyValue = GetHivedBankingModConfig().GetHighestDenomination(AmountToAdd);
			if (NewMoneyValue && NewMoneyValue != MoneyValue){
				MoneyValue = NewMoneyValue;
			} else if (NewMoneyValue){
				int AmountToSpawnGround = GetHivedBankingModConfig().GetAmount(MoneyValue,AmountToAdd);
				if (AmountToSpawnGround < 1){
					NoError = false;
				} else {
					HBCreateMoneyGround(MoneyValue.Item, AmountToSpawnGround);
					float BalanceDiff = MoneyValue.Value * AmountToSpawnGround;
					PlayerBalance = PlayerBalance + BalanceDiff;
					Return = 1;
					MoneyValue = GetHivedBankingModConfig().GetHighestDenomination(AmountToAdd);
				}
			}else{
				NoError = false;
			}
		}
		return Return;
	}
	
	
	void HBRemoveMoney(float Amount){
		if (Amount <= 0){
			return;
		}
		float AmountToRemove = Amount;
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float SmallestCurrency = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		for (int i = 0; i < GetHivedBankingModConfig().MoneyValues.Count(); i++){
			AmountToRemove =  HBRemoveMoneyInventory(GetHivedBankingModConfig().MoneyValues.Get(i), AmountToRemove);
		}
		if (AmountToRemove > 0){ // Now to delete a larger bill and make change
			int MaxIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
			for (int j = MaxIndex; j > 0; j--){
				int NewAmountToRemove =  HBRemoveMoneyInventory(GetHivedBankingModConfig().MoneyValues.Get(j), GetHivedBankingModConfig().MoneyValues.Get(j).Value);
				if (NewAmountToRemove == 0){
					int AmountToAddBack = GetHivedBankingModConfig().MoneyValues.Get(j).Value - AmountToRemove;
					HBAddMoney(AmountToAddBack);
				}
			}
		}
	}
	
	//Return how much left still to remove
	float HBRemoveMoneyInventory(HBMoneyValue MoneyValue, float Amount ){
		int AmountToRemove = GetHivedBankingModConfig().GetAmount(MoneyValue, Amount);
		if (AmountToRemove > 0){
			array<EntityAI> itemsArray = new array<EntityAI>;
			this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
			for (int i = 0; i < itemsArray.Count(); i++){
				ItemBase item = ItemBase.Cast(itemsArray.Get(i));
				if (item){
					string ItemType = item.GetType();
					ItemType.ToLower();
					string MoneyType = MoneyValue.Item;
					MoneyType.ToLower();
					if (ItemType == MoneyType){
						int CurQuantity = item.GetQuantity();
						float AmountRemoved = 0;
						if (AmountToRemove < CurQuantity){
							AmountRemoved = MoneyValue.Value * AmountToRemove;
							item.HBSetQuantity(CurQuantity - AmountToRemove);
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount - AmountRemoved;
						} else if (AmountToRemove == CurQuantity){
							AmountRemoved = MoneyValue.Value * AmountToRemove;
							GetGame().ObjectDelete(item);
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount - AmountRemoved;
						} else {
							AmountRemoved = MoneyValue.Value * CurQuantity;
							AmountToRemove = AmountToRemove - CurQuantity;
							GetGame().ObjectDelete(item);
							Amount = Amount - AmountRemoved;
						}
						if (AmountToRemove <= 0){
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount;
						}
					}
				}
			}
		}
		this.UpdateInventoryMenu(); // RPC-Call needed?
		return Amount;
	}
	
	//Return How many Items it faild to create in the Inventory
	void HBCreateMoneyInventory(string itemType, int amount)
	{
		array<EntityAI> itemsArray = new array<EntityAI>;
		this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		string itemLower = itemType;
		itemLower.ToLower();

		ItemBase item;
		Ammunition_Base ammoItem;
		int currentAmount = amount;
		//if item doesn't have count, quantity or it has quantitybar then we should spawn one item instead of trying to stack it
		//if item has quantitybar we should spawn one with full bar? maybe make it an option for stuff like gasoline canister
		bool hasSomeQuant = ((HBMaxQuantity(itemType) > 0) || HBHasQuantity(itemType));		
		int itemHasSpawnedOrStacked = 0;
		//autostacking
		//check if we have any stackable items of the type itemType
		//if we do, then add to each stack until no more stacks found or out of amount
		//we should keep count of how many items we spawned
		if (hasSomeQuant)
		{
			for (int i = 0; i < itemsArray.Count(); i++)
			{
				if (currentAmount <= 0){
					this.UpdateInventoryMenu(); // RPC-Call needed?
					return;
				}
				Class.CastTo(item, itemsArray.Get(i));
				string itemPlayerClassname = "";
				if (item){
					if (item.IsRuined()){
						continue;
					}
					itemPlayerClassname = item.GetType();
					itemPlayerClassname.ToLower();
					if (itemLower == itemPlayerClassname && !item.IsFullQuantity() && !item.IsMagazine()){
						currentAmount = item.HBAddQuantity(currentAmount);
					}
				}

				Class.CastTo(ammoItem, itemsArray.Get(i));
				if (ammoItem){
					if (ammoItem.IsRuined()){	
						continue;
					}
					itemPlayerClassname = ammoItem.GetType();
					itemPlayerClassname.ToLower();
					if (itemLower == itemPlayerClassname && ammoItem.IsAmmoPile()){
						currentAmount = ammoItem.HBAddQuantity(currentAmount);
					}
				}
			}
		}
		
		//any leftover or new stacks
		while (currentAmount > 0)
		{
			EntityAI newItem = EntityAI.Cast(this.GetInventory().CreateInInventory(itemType));
			if (!newItem)
			{
				for (int j = 0; j < itemsArray.Count(); j++)
				{
					Class.CastTo(item, itemsArray.Get(j));
					if (!item){
						continue;
					}
					newItem = EntityAI.Cast(item.GetInventory().CreateInInventory(itemType)); //CreateEntityInCargo	
					if (newItem){
						break;
					}
				}
			}
			
			Magazine newMagItem = Magazine.Cast(newItem);
			Ammunition_Base newammoItem = Ammunition_Base.Cast(newItem);
			if (newMagItem && !newammoItem)					
			{	
				int SetAmount = currentAmount;
				if (newMagItem.GetQuantityMax() <= currentAmount){
					SetAmount = currentAmount;
				} else {
					SetAmount = newMagItem.GetQuantityMax();
					currentAmount = currentAmount - SetAmount;
				}
				newMagItem.ServerSetAmmoCount(SetAmount);
			}
			if (hasSomeQuant)
			{
				if (newammoItem){
					currentAmount = newammoItem.HBSetQuantity(currentAmount);
	
				}	
				ItemBase newItemBase;
				if (Class.CastTo(newItemBase, newItem)){
					currentAmount = newItemBase.HBSetQuantity(currentAmount);
				}
			}
		}
		this.UpdateInventoryMenu(); // RPC-Call needed?
	}
	
	//Return How many Items it faild to create in on the ground
	int HBCreateMoneyGround(string Type, int Amount){
		
		return 0;
	}
	
	int HBCurrentQuantity(ItemBase money){
		ItemBase moneyItem = ItemBase.Cast(money);
		if (!moneyItem){
			return false;
		}	
		if (HBMaxQuantity(moneyItem.GetType()) == 0){
			return 1;
		}
		if ( moneyItem.IsMagazine() ){
			Magazine mag = Magazine.Cast(moneyItem);
			if (mag){
				return mag.GetAmmoCount();
			}
		}
		return moneyItem.GetQuantity();
	}
	
	int HBMaxQuantity(string Type)
	{
		if ( GetGame().ConfigIsExisting(  CFG_MAGAZINESPATH  + " " + Type + " count" ) ){
			return g_Game.ConfigGetInt(  CFG_MAGAZINESPATH  + " " + Type + " count" );
		}
		if ( GetGame().ConfigIsExisting(  CFG_VEHICLESPATH + " " + Type + " varQuantityMax" ) ){
			return g_Game.ConfigGetInt( CFG_VEHICLESPATH + " " + Type + " varQuantityMax" ) );
		}
		return 0;
	}
	
	bool HBSetMoneyAmount(ItemBase item, int amount)
	{
		ItemBase money = ItemBase.Cast(item);
		if (!money){
			return false;
		}
		if ( money.IsMagazine() ){
			Magazine mag = Magazine.Cast(money);
			if (mag){
				return true;
				mag.ServerSetAmmoCount(amount);
			}
		}
		else{
			money.SetQuantity(amount);
			return true;
		}
		return false;
	}
	
	bool HBHasQuantity(string Type)
	{   
		
		string path = CFG_MAGAZINESPATH  + " " + Type + " count";
	    if (GetGame().ConfigIsExisting(path)){
	     	if (GetGame().ConfigGetInt(path) > 0){
				return true;
			}
		}
	    path = CFG_VEHICLESPATH  + " " + Type + " quantityBar";
	    if (GetGame().ConfigIsExisting(path))   {
	        return GetGame().ConfigGetInt(path) == 1;
		}
	
	    return false;
	}
}