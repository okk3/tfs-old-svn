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

#ifndef __PARTY__
#define __PARTY__
#include "player.h"

typedef std::vector<Player*> PlayerVector;
typedef std::vector<Item*> ItemVector;

class Player;
class Party;

class Party
{
	public:
		Party(Player* _leader);
		virtual ~Party() {}

		Player* getLeader() const {return leader;}
		void setLeader(Player* _leader) {leader = _leader;}
		PlayerVector getMembers() {return memberList;}

		bool passPartyLeadership(Player* player);
		void disband();
		bool disbandParty() {return (memberList.empty() && inviteList.empty());}

		bool invitePlayer(Player* player);
		void revokeInvitation(Player* player);
		bool removeInvite(Player* player);
		bool joinParty(Player* player);
		bool leaveParty(Player* player);

		void updateAllPartyIcons();
		void updatePartyIcons(Player* player);
		void broadcastPartyMessage(MessageClasses messageClass, const std::string& text, bool sendToInvitations = false);
		void broadcastPartyLoot(const std::string& monster, const ItemVector& items);

		void shareExperience(uint64_t experience);
		bool setSharedExperience(Player* player, bool _sharedExpActive);
		bool isSharedExperienceActive() const {return sharedExpActive;}
		bool isSharedExperienceEnabled() const {return sharedExpEnabled;}
		bool canUseSharedExperience(const Player* player, uint32_t highestLevel) const;
		void updateSharedExperience();

		void addPlayerHealedMember(Player* player, uint32_t points);
		void addPlayerDamageMonster(Player* player, uint32_t points);
		void clearPlayerPoints(Player* player);

		bool isPlayerMember(const Player* player) const;
		bool isPlayerInvited(const Player* player) const;
		bool canOpenCorpse(uint32_t ownerId);

	protected:
		bool canEnableSharedExperience();

		PlayerVector memberList;
		PlayerVector inviteList;

		Player* leader;
		bool sharedExpActive, sharedExpEnabled;
		struct CountBlock_t
		{
			int32_t totalHeal;
			int32_t totalDamage;
			int64_t ticks;
		};

		typedef std::map<uint32_t, CountBlock_t> CountMap;
		CountMap pointMap;
};
#endif

