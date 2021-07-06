#include "Map.h"
#include "PlayState.h"
#include <random>

// STERGE DE AICI
#include <iostream>

void Map::CreateTilesOnMap(const sf::Vector2u& tileSize)
{
	uint16_t tileNumber = 0;

	GenerateMapLayout();

	m_tileset.loadFromFile("../_external/sprites/sprites2.png");
	m_tileset.setSmooth(true);

	for (uint16_t heightIndex = 0; heightIndex < m_mapHeight; heightIndex++)
	{
		for (uint16_t widthIndex = 0; widthIndex < m_mapWidth; widthIndex++)
		{

			uint16_t tileNumberIndex = static_cast<uint16_t>(m_map[tileNumber]);

			uint16_t textureLineIndex = tileNumberIndex % (m_tileset.getSize().x / tileSize.x);
			uint16_t textureColumnIndex = tileNumberIndex / (m_tileset.getSize().x / tileSize.x);

			m_blocks[tileNumber].SetPosition(sf::Vector2f(widthIndex * tileSize.x, heightIndex * tileSize.y));
			m_blocks[tileNumber].SetBlockType(m_map[tileNumber]);
			m_blocks[tileNumber].SetBlockTexture(&m_tileset, sf::IntRect(textureLineIndex * tileSize.x, textureColumnIndex * tileSize.y, tileSize.x, tileSize.y));

			tileNumber++;
		}
	}

	GenerateRandomEnemy();

}

Block& Map::GetBlock(uint16_t position)
{
	return m_blocks[position];
}

const uint16_t Map::GetWidth()
{
	return m_mapWidth;
}

const uint16_t Map::GetHeight()
{
	return m_mapHeight;
}


bool Map::GenerateDestroyableBlock() const
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);

	return dis(gen);
}

void Map::GenerateBombs(const sf::Vector2f& playersPosition, const uint16_t& bombRadius, const float& timeElapsed, const uint16_t& maxNoBombs)
{
	if (m_bombsList.size() < maxNoBombs)
	{
		if (CheckBombsPositions(playersPosition) == true)
		{
			Bomb* newBomb = new Bomb(m_blocks[static_cast<uint16_t>(round(playersPosition.x / 48))].GetPosition().x,
				m_blocks[static_cast<uint16_t>(round(playersPosition.y / 48))].GetPosition().x,
				bombRadius, timeElapsed);
			m_bombsList.push_back(newBomb);
		}
	}
}

uint16_t Map::GetNoBombsDisplayed() const
{
	return m_bombsList.size();
}

void Map::GenerateRandomTeleport() {

	if (m_numberOfMonsters == 0) {

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, m_blocks.size() - 1);

		uint16_t randomIndex = 0;

		while (m_blocks[randomIndex].GetIsPortal() != true)
		{
			randomIndex = dis(gen);

			if (m_map[randomIndex] == EBlockType::StoneBlock) {

				m_blocks[randomIndex].SetIsPortal(true);
				m_portalGenerated = true;

			}
		}
	}
}

bool Map::IsPortalGenerate()
{
	return m_portalGenerated;
}

void Map::IsPortal(Block& block) {

	m_portal = new Portal(sf::Vector2f(block.GetBlockBody().getPosition()));
	block.SetBlockType(EBlockType::PortalBlock);
	m_portal->SetIsVisible(true);
}

void Map::GenerateRandomPowerUp(const sf::Vector2f& position)
{
	m_powerUp = new PowerUp(sf::Vector2f(position));
	m_powerUp->CreateRandomPowerUp();
}

void Map::GenerateRandomEnemy() {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, m_blocks.size() - 1);

	uint16_t randomIndex = 0;

	while (m_blocks[randomIndex].GetIsPortal() != true)
	{
		randomIndex = dis(gen);

		if (m_map[randomIndex] == EBlockType::EmptyBlock) {

			m_enemy = new Enemy();
			m_enemy->SetPosition(m_blocks[randomIndex].GetPosition());
			break;
		}

	}
}

bool Map::CheckBombsPositions(sf::Vector2f playerPosition)
{
	bool flag = false;
	if (m_bombsList.empty())
		flag = true;
	else
	{
		std::vector<uint16_t> bombsPositions;
		for (auto& bomb : m_bombsList)
		{
			uint16_t bombIndexColumn = (bomb->GetBombShape().getPosition().x + 24) / 48;
			uint16_t bombIndexLine = (bomb->GetBombShape().getPosition().y + 24) / 48;
			uint16_t bombsIndex = bombIndexLine * 17 + bombIndexColumn;
			bombsPositions.push_back(bombsIndex);
		}
		uint16_t playerIndexColumn = (playerPosition.x + 24) / 48;
		uint16_t playerIndexLine = (playerPosition.y + 24) / 48;
		uint16_t playersIndex = playerIndexLine * 17 + playerIndexColumn;
		if (std::find(bombsPositions.begin(), bombsPositions.end(), playersIndex) != bombsPositions.end())
			return false;
		else
			return true;
	}
	return flag;
}

void Map::EarlyExplode(Bomb* bomb)
{
	uint16_t bombIndexColumn = (bomb->GetBombShape().getPosition().x + 24) / 48;
	uint16_t bombIndexLine = (bomb->GetBombShape().getPosition().y + 24) / 48;
	uint16_t index = bombIndexLine * 17 + bombIndexColumn;
	if (m_blocks[index].GetBlockType() == EBlockType::FireBlock)
		bomb->SetBombStatus(true);
}

void Map::ClearFireBlocks()
{
	for (auto& fireBlock : fireBlocks)
	{
		m_blocks[fireBlock].SetBlockType(EBlockType::EmptyBlock);
	}
}

void Map::MoveEnemy()
{
	if (m_enemy != nullptr) {

		uint16_t enemyIndexColumn = (m_enemy->GetShape().getPosition().x + 24) / 48;
		uint16_t enemyIndexLine = (m_enemy->GetShape().getPosition().y + 24) / 48;

		uint16_t index = enemyIndexLine * 17 + enemyIndexColumn;

		if (m_elapsedTime - m_enemy->GetLastTimeMoving() >= 0.8f)
		{
			if (m_enemy->GetDirection() == Direction::DOWN && m_enemy->GetWallHitted() == false)
				if (m_blocks[index + 17].GetBlockType() == EBlockType::EmptyBlock)
				{
					m_enemy->SetPosition(m_blocks[index + 17].GetPosition());
					m_enemy->SetLastTimeMoving(m_elapsedTime);
					m_enemy->SetLastDirection(m_enemy->GetDirection());
				}
				else
				{
					m_enemy->SetWallHitted(true);
				}

			if (m_enemy->GetDirection() == Direction::UP && m_enemy->GetWallHitted() == false)
				if (m_blocks[index - 17].GetBlockType() == EBlockType::EmptyBlock)
				{
					m_enemy->SetPosition(m_blocks[index - 17].GetPosition());
					m_enemy->SetLastTimeMoving(m_elapsedTime);
					m_enemy->SetLastDirection(m_enemy->GetDirection());
				}
				else
				{
					m_enemy->SetWallHitted(true);
				}


			if (m_enemy->GetDirection() == Direction::LEFT && m_enemy->GetWallHitted() == false)
				if (m_blocks[index - 1].GetBlockType() == EBlockType::EmptyBlock)
				{
					m_enemy->SetPosition(m_blocks[index - 1].GetPosition());
					m_enemy->SetLastTimeMoving(m_elapsedTime);
					m_enemy->SetLastDirection(m_enemy->GetDirection());
				}
				else
				{
					m_enemy->SetWallHitted(true);
				}

			if (m_enemy->GetDirection() == Direction::RIGHT && m_enemy->GetWallHitted() == false)
				if (m_blocks[index + 1].GetBlockType() == EBlockType::EmptyBlock)
				{
					m_enemy->SetPosition(m_blocks[index + 1].GetPosition());
					m_enemy->SetLastTimeMoving(m_elapsedTime);
					m_enemy->SetLastDirection(m_enemy->GetDirection());
				}
				else
				{
					m_enemy->SetWallHitted(true);
				}

			if (m_enemy->GetWallHitted() == true)
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dis(0, 3);

				uint16_t randomIndex = 0;
				randomIndex = dis(gen);

				while (m_enemy->GetLastDirection() != static_cast<Direction>(randomIndex))
				{
					m_enemy->SetDirection(static_cast<Direction>(randomIndex));
					randomIndex = dis(gen);

				}
				m_enemy->SetWallHitted(false);
			}


		}

	}

}

bool Map::IsEnemyDead()
{
	if (m_enemy != nullptr) {

		uint16_t enemyIndexColumn = (m_enemy->GetPositionX() + 24) / 48;
		uint16_t enemyIndexLine = (m_enemy->GetPositionY() + 24) / 48;
		uint16_t index = enemyIndexLine * 17 + enemyIndexColumn;

		if (m_blocks[index].GetBlockType() == EBlockType::FireBlock)
		{
			m_enemy = nullptr;
			m_numberOfMonsters--;
			return true;
		}

		return false;
	}
}

uint16_t Map::GetNumberOfMonsters()
{
	return m_numberOfMonsters;
}

void Map::GenerateMapLayout()
{
	uint16_t indexBlockOnMap = 0;

	for (uint16_t lineIndex = 0; lineIndex < m_mapHeight; lineIndex++)
	{
		for (uint16_t columnIndex = 0; columnIndex < m_mapWidth; columnIndex++)
		{
			if (lineIndex == 0 || lineIndex == m_mapHeight - 1 || columnIndex == 0 || columnIndex == m_mapHeight - 1)
			{
				m_map[indexBlockOnMap] = EBlockType::BorderBlock;
				// std::cout << "3 ";
			}

			else if (lineIndex % 2 == 0 && columnIndex % 2 == 0)
			{
				m_map[indexBlockOnMap] = EBlockType::WallBlock;
				// std::cout << "2 ";
			}

			else
			{
				if (GenerateDestroyableBlock())
				{
					m_map[indexBlockOnMap] = EBlockType::StoneBlock;
					// std::cout << "1 ";
				}
				else
				{
					m_map[indexBlockOnMap] = EBlockType::EmptyBlock;
					// std::cout << "0 ";
				}
			}

			indexBlockOnMap++;
		}
	}

	m_map[18] = EBlockType::EmptyBlock;
	m_map[19] = EBlockType::EmptyBlock;
	m_map[35] = EBlockType::EmptyBlock;
}

PowerUp* Map::GetPower()
{
	return m_powerUp;
}

Enemy* Map::GetEnemy()
{
	return m_enemy;
}

void Map::ExplosionMarker(const sf::Vector2f& position)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 20);

	uint16_t randomNumber = dis(gen);

	uint16_t positionIndexColumn = (position.x + 24) / 48;
	uint16_t positionIndexLine = (position.y + 24) / 48;
	uint16_t index = positionIndexLine * 17 + positionIndexColumn;

	if (randomNumber == 20 && m_blocks[index].GetBlockType() == EBlockType::StoneBlock)
	{
		ClearBlock(position);
		if (m_powerUpPlaced == false && m_powerUp == nullptr) {
			GenerateRandomPowerUp(position);
			m_blocks[index].SetBlockType(EBlockType::PowerUpBlock);
			m_powerUpPlaced = true;
		}
	}
	else if (m_blocks[index].GetBlockType() == EBlockType::EmptyBlock)
	{
		m_blocks[index].SetBlockType(EBlockType::FireBlock);
		fireBlocks.push_back(index);
	}
	else if (m_blocks[index].GetBlockType() == EBlockType::PowerUpBlock)
		RemovePowerUp();
	else if (m_blocks[index].GetBlockType() == EBlockType::StoneBlock && m_blocks[index].GetIsPortal() != true)
		ClearBlock(position);
	else if (m_blocks[index].GetBlockType() == EBlockType::StoneBlock && m_blocks[index].GetIsPortal() == true)
	{
		IsPortal(m_blocks[index]);
	}
}

void Map::RemovePowerUp()
{
	m_powerUp = nullptr;
}

void Map::ClearBlock(const sf::Vector2f& position)
{
	uint16_t positionIndexColumn = (position.x + 24) / 48;
	uint16_t positionIndexLine = (position.y + 24) / 48;

	uint16_t index = positionIndexLine * 17 + positionIndexColumn;
	if (m_blocks[index].GetBlockType() != EBlockType::EmptyBlock)
	{
		m_blocks[index].SetBlockType(EBlockType::EmptyBlock);
		m_map[index] == EBlockType::EmptyBlock;
		m_blocks[index].SetBlockTexture(&m_tileset, sf::IntRect(0, 0, 48, 48));
	}
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = &m_tileset;

	for (const Block& block : m_blocks)
	{
		target.draw(block.GetBlockBody());
	}

	for (auto& bomb : m_bombsList)
	{
		target.draw(bomb->GetBombShape());
		bomb->Update(m_elapsedTime);
	}

	if (m_portal != nullptr && m_portal->GetIsVisible() == true)
	{
		target.draw(m_portal->GetShape());
		m_portal->Update(m_elapsedTime);
	}

	if (m_powerUp != nullptr)
	{
		target.draw(m_powerUp->GetShape());
	}

	if (m_enemy != nullptr)
	{
		target.draw(m_enemy->GetShape());
	}

}