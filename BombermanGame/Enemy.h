#pragma once
#include <SFML/Graphics.hpp>
#include "Collider.h"
#include "EnemyDirection.h"
#include "Block.h"
#include "EnemyDirection.h"

class Enemy
{
public:

	Enemy();
	~Enemy() = default;

	Collider GetCollider();
	sf::RectangleShape GetShape() const;
	float GetPositionX() const;
	float GetPositionY() const;
	float GetLastTimeMoving() const;

	void SetLastTimeMoving(float elapsedTime);

	void SetPosition(const sf::Vector2f& position);

	Direction GetDirection();
	void SetDirection(Direction direction);

	Direction GetLastDirection();
	void SetLastDirection(Direction direction);

	bool GetWallHitted();
	void SetWallHitted(bool status);

private:

	sf::RectangleShape m_enemyShape;
	sf::Texture m_texture;
	float m_lastTimeMoving;

	Direction m_direction = Direction::UP;
	Direction m_lastDirection = Direction::UP;
	bool m_wallHitted = true;


private:
	float m_speed;
};

