////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#ifndef __HOUSE__
#define __HOUSE__
#include "otsystem.h"

#include <boost/regex.hpp>

#include "position.h"
#include "housetile.h"

#include "player.h"

class House;
class BedItem;

enum AccessList_t
{
	GUEST_LIST = 0x100,
	SUBOWNER_LIST = 0x101
};

enum AccessHouseLevel_t
{
	HOUSE_NO_INVITED = 0,
	HOUSE_GUEST = 1,
	HOUSE_SUBOWNER = 2,
	HOUSE_OWNER = 3
};

enum RentPeriod_t
{
	RENTPERIOD_DAILY,
	RENTPERIOD_WEEKLY,
	RENTPERIOD_MONTHLY,
	RENTPERIOD_YEARLY,
	RENTPERIOD_NEVER
};

typedef std::list<HouseTile*> HouseTileList;
typedef std::list<Door*> HouseDoorList;
typedef std::list<BedItem*> HouseBedItemList;
typedef std::map<uint32_t, House*> HouseMap;

class AccessList
{
	public:
		AccessList() {}
		virtual ~AccessList() {}

		bool parseList(const std::string& _list);
		bool addPlayer(std::string& name);
		bool addGuild(const std::string& guildName, const std::string& rankName);
		bool addExpression(const std::string& expression);

		bool isInList(const Player* player);

		void getList(std::string& _list) const;

	private:
		typedef OTSERV_HASH_SET<uint32_t> PlayerList;
		typedef std::list<std::pair<uint32_t, int32_t> > GuildList;
		typedef std::list<std::string> ExpressionList;
		typedef std::list<std::pair<boost::regex, bool> > RegExList;

		std::string list;
		PlayerList playerList;
		GuildList guildList;
		ExpressionList expressionList;
		RegExList regExList;
};

class Door : public Item
{
	public:
		Door(uint16_t _type);
		virtual ~Door();

		virtual Door* getDoor() {return this;}
		virtual const Door* getDoor() const {return this;}

		House* getHouse() {return house;}

		//serialization
		virtual Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream);

		void setDoorId(uint32_t _doorId){ setIntAttr(ATTR_ITEM_DOORID, (uint32_t)_doorId); }
		uint32_t getDoorId() const{ return getIntAttr(ATTR_ITEM_DOORID); }

		bool canUse(const Player* player);

		void setAccessList(const std::string& textlist);
		bool getAccessList(std::string& list) const;

		//overrides
		virtual void onRemoved();
		void copyAttributes(Item* item);

	protected:
		void setHouse(House* _house);

	private:
		House* house;
		AccessList* accessList;
		friend class House;
};

class HouseTransferItem : public Item
{
	public:
		static HouseTransferItem* createHouseTransferItem(House* house);

		HouseTransferItem(House* _house): Item(0) {house = _house;}
		virtual ~HouseTransferItem() {}

		virtual bool onTradeEvent(TradeEvents_t event, Player* owner, Player* seller);
		virtual bool canTransform() const {return false;}

		House* getHouse() {return house;}

	protected:
		House* house;
};

class House
{
	public:
		virtual ~House() {}

		House(uint32_t _houseId);
		uint32_t getHouseId() const {return houseId;}

		void setEntryPos(const Position& pos) {posEntry = pos;}
		const Position& getEntryPosition() const {return posEntry;}

		void setName(const std::string& _houseName) {houseName = _houseName;}
		const std::string& getName() const {return houseName;}

		void setHouseOwner(uint32_t guid);
		bool setHouseOwnerEx(uint32_t guid, bool transfer);
		uint32_t getHouseOwner() const {return houseOwner;}

		void setPaidUntil(time_t paid) {paidUntil = paid;}
		time_t getPaidUntil() const {return paidUntil;}

		void setRent(uint32_t _rent) {rent = _rent;}
		uint32_t getRent() const {return rent;}

		void setPrice(uint32_t _price, bool update = false);
		uint32_t getPrice() const {return price;}

		void setLastWarning(time_t _lastWarning) {lastWarning = _lastWarning;}
		time_t getLastWarning() {return lastWarning;}

		void setRentWarnings(uint32_t warnings) {rentWarnings = warnings;}
		uint32_t getRentWarnings() const {return rentWarnings;}

		void setTownId(uint32_t _town) {townId = _town;}
		uint32_t getTownId() const {return townId;}

		void setSize(uint32_t _size) {size = _size;}
		uint32_t getSize() const {return size;}

		void setPendingTransfer(bool transfer) {pendingTransfer = transfer;}
		bool hasPendingTransfer() const {return pendingTransfer;}

		void setGuild(bool _guild) {guild = _guild;}
		bool isGuild() const;

		uint32_t getDoorsCount() const {return doorList.size();}
		uint32_t getBedsCount() const {return bedsList.size();}
		uint32_t getTilesCount() const {return houseTiles.size();}

		bool canEditAccessList(uint32_t listId, const Player* player);
		void setAccessList(uint32_t listId, const std::string& textlist, bool teleport = true);
		bool getAccessList(uint32_t listId, std::string& list) const;

		bool isInvited(const Player* player);
		AccessHouseLevel_t getHouseAccessLevel(const Player* player);

		bool kickPlayer(Player* player, Player* target);
		void updateDoorDescription(std::string name = "");
		void clean();

		void addDoor(Door* door);
		void removeDoor(Door* door);
		HouseDoorList::iterator getHouseDoorBegin() {return doorList.begin();}
		HouseDoorList::iterator getHouseDoorEnd() {return doorList.end();}

		void addBed(BedItem* bed);
		HouseBedItemList::iterator getHouseBedsBegin() {return bedsList.begin();}
		HouseBedItemList::iterator getHouseBedsEnd() {return bedsList.end();}

		void addTile(HouseTile* tile);
		HouseTileList::iterator getHouseTileBegin() {return houseTiles.begin();}
		HouseTileList::iterator getHouseTileEnd() {return houseTiles.end();}

		Door* getDoorByNumber(uint32_t doorId) const;
		Door* getDoorByPosition(const Position& pos);

	private:
		bool transferToDepot();
		void removePlayer(Player* player, bool ignoreRights);
		void removePlayers(bool ignoreInvites);

		bool guild, pendingTransfer;
		time_t paidUntil, lastWarning;
		uint32_t houseId, houseOwner, rentWarnings, rent, price, townId, size;
		std::string houseName;
		Position posEntry;

		AccessList guestList, subOwnerList;
		HouseTileList houseTiles;
		HouseDoorList doorList;
		HouseBedItemList bedsList;
};

class Houses
{
	public:
		virtual ~Houses() {}
		static Houses& getInstance()
		{
			static Houses instance;
			return instance;
		}

		bool loadFromXml(std::string filename);
		bool reloadPrices();

		void payHouses();
		bool payRent(Player* player, House* house, time_t _time = 0);

		HouseMap::iterator getHouseBegin() {return houseMap.begin();}
		HouseMap::iterator getHouseEnd() {return houseMap.end();}

		House* getHouse(uint32_t houseId, bool add = false);
		House* getHouseByPlayer(Player* player);

		House* getHouseByPlayerId(uint32_t playerId);
		House* getHouseByGuildId(uint32_t guildId);

		uint32_t getHousesCount(uint32_t accId);

	private:
		Houses();
		bool payHouse(House* house, time_t _time);

		HouseMap houseMap;
		RentPeriod_t rentPeriod;
};
#endif
