modded class PlayerBase extends ManBase
{
	bool HBCanAccept(ItemBase item){
		return !item.IsRuined() || GetHivedBankingModConfig().CanDepositRuinedBills;
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
			return 2;
		}
		int Return = 0;
		float AmountToAdd = Amount;
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float SmallestCurrency = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		float PlayerBalance = HBGetPlayerBalance();
		float OptimalPlayerBalance = PlayerBalance + AmountToAdd;
		
		HBMoneyValue MoneyValue = GetHivedBankingModConfig().GetHighestDenomination(AmountToAdd);
		int MaxLoop = 5000;
		while (MoneyValue && AmountToAdd >= SmallestCurrency && NoError && MaxLoop > 0){
			MaxLoop--;
			int AmountToSpawn = GetHivedBankingModConfig().GetAmount(MoneyValue,AmountToAdd);
			if (AmountToSpawn == 0){
				NoError = false;
			}
			
			int AmountLeft = HBCreateMoneyInventory(MoneyValue.Item, AmountToSpawn);
			if (AmountLeft > 0){
				Return = 1;
				HBCreateMoneyGround(MoneyValue.Item, AmountLeft);
			}
			
			float AmmountAdded = MoneyValue.Value * AmountToSpawn;
			
			AmountToAdd = AmountToAdd - AmmountAdded;
			
			HBMoneyValue NewMoneyValue = GetHivedBankingModConfig().GetHighestDenomination(AmountToAdd);
			if (NewMoneyValue && NewMoneyValue != MoneyValue){
				MoneyValue = NewMoneyValue;
			} else {
				NoError = false;
			}
		}
		return Return;
	}
	
	
	int HBRemoveMoney(float Amount){
		if (Amount <= 0){
			return 2;
		}
		int Return = 0;
		float AmountToRemove = Amount;
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float SmallestCurrency = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		for (int i = 0; i < GetHivedBankingModConfig().MoneyValues.Count(); i++){
			AmountToRemove =  HBRemoveMoneyInventory(GetHivedBankingModConfig().MoneyValues.Get(i), AmountToRemove);
		}
		if (AmountToRemove >= SmallestCurrency){ // Now to delete a larger bill and make change
			for (int j = LastIndex; j >= 0; j--){
				//Print("[HivedBanking] Trying to remove " + GetHivedBankingModConfig().MoneyValues.Get(j).Item);
				float NewAmountToRemove =  HBRemoveMoneyInventory(GetHivedBankingModConfig().MoneyValues.Get(j), GetHivedBankingModConfig().MoneyValues.Get(j).Value);
				if (NewAmountToRemove == 0){
					float AmountToAddBack = GetHivedBankingModConfig().MoneyValues.Get(j).Value - AmountToRemove;
					//Print("[HivedBanking] A " + GetHivedBankingModConfig().MoneyValues.Get(j).Item + " removed trying to add back " + AmountToAddBack );
					Return = HBAddMoney(AmountToAddBack);
				}
			}
		}
		return Return;
	}
	
	//Return how much left still to remove
	float HBRemoveMoneyInventory(HBMoneyValue MoneyValue, float Amount ){
		int AmountToRemove = GetHivedBankingModConfig().GetAmount(MoneyValue, Amount);
		int LastIndex = GetHivedBankingModConfig().MoneyValues.Count() - 1;
		float SmallestCurrency = GetHivedBankingModConfig().MoneyValues.Get(LastIndex).Value;
		if (AmountToRemove > 0){
			array<EntityAI> itemsArray = new array<EntityAI>;
			this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
			for (int i = 0; i < itemsArray.Count(); i++){
				ItemBase item = ItemBase.Cast(itemsArray.Get(i));
				if (item && HBCanAccept(item)){
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
	int HBCreateMoneyInventory(string itemType, int amount)
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
					return 0;
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
		bool stoploop = false;
		int MaxLoop = 5000;
		//any leftover or new stacks
		while (currentAmount > 0 && !stoploop && MaxLoop > 0)
		{
			MaxLoop--;
			ItemBase newItem = ItemBase.Cast(this.GetInventory().CreateInInventory(itemType));
			if (!newItem){
				stoploop = true; //To stop the loop from running away since it couldn't create an item
				for (int j = 0; j < itemsArray.Count(); j++){
					Class.CastTo(item, itemsArray.Get(j));
					if (item){ 
						newItem = ItemBase.Cast(item.GetInventory().CreateInInventory(itemType)); //CreateEntityInCargo	
						if (newItem){
							//Print("[HivedBanking] NewItem Created " + newItem.GetType() + " in " + item.GetType());
							stoploop = false; //Item was created so we can don't need to stop the loop anymore
							break;
						}
					}
				}
			}
			
			Magazine newMagItem = Magazine.Cast(newItem);
			Ammunition_Base newammoItem = Ammunition_Base.Cast(newItem);
			if (newMagItem && !newammoItem)	{	
				int SetAmount = currentAmount;
				if (newMagItem.GetQuantityMax() <= currentAmount){
					SetAmount = currentAmount;
					currentAmount = 0;
				} else {
					SetAmount = newMagItem.GetQuantityMax();
					currentAmount = currentAmount - SetAmount;
				}
				newMagItem.ServerSetAmmoCount(SetAmount);
			} else if (hasSomeQuant){
				if (newammoItem){
					currentAmount = newammoItem.HBSetQuantity(currentAmount);
	
				}	
				ItemBase newItemBase;
				if (Class.CastTo(newItemBase, newItem)){
					currentAmount = newItemBase.HBSetQuantity(currentAmount);
				}
			} else { //It created just one of the item
				currentAmount--;
			}
		}
		return currentAmount;
		this.UpdateInventoryMenu();
	}
	
	//Return How many Items it faild to create in on the ground
	void HBCreateMoneyGround(string Type, int Amount){
		int AmountToSpawn = Amount;
		bool hasSomeQuant = ((HBMaxQuantity(Type) > 0) || HBHasQuantity(Type));
		int MaxQuanity = HBMaxQuantity(Type);
		int StacksRequired = AmountToSpawn;
		if (MaxQuanity != 0){
			StacksRequired = Math.Ceil( AmountToSpawn /  MaxQuanity);
		}
		for (int i = 0; i <= StacksRequired; i++){
			if (AmountToSpawn > 0){
				ItemBase newItem = ItemBase.Cast(GetGame().CreateObjectEx(Type, GetPosition(), ECE_PLACE_ON_SURFACE));
				if (newItem && hasSomeQuant){
					AmountToSpawn = newItem.HBSetQuantity(AmountToSpawn);
				}
			}
		}
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