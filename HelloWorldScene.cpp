/*
* ���ڣ�2019/5/20
* ���ߣ�����֥
* ���ܣ�ʵ�ַɻ���ս
*/
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
using namespace cocostudio::timeline;
#define BACKGROUND_HEIGHT 638
#define HERO_TAG 99
USING_NS_CC;

//��������
Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//�����µĲ�layerBG������ʾ����
	auto* layerBG = Layer::create();

	//������ͼ�㣬background[0]��background[1]ѭ����������ʾ������ͼ
	background[0] = Sprite::create("background.png");
	background[0]->setAnchorPoint(Vec2(0, 0));
	background[0]->setPosition(Vec2(0, 0));
	layerBG->addChild(background[0]);
	background[1] = Sprite::create("background.png");
	background[1]->setAnchorPoint(Vec2(0, 0));
	background[1]->setPosition(Vec2(0, BACKGROUND_HEIGHT));
	layerBG->addChild(background[1]);

	//�����ɻ��㣬background[2]��background[3]ѭ����������ʾ�ɻ��ɹ�����Ч��
	background[2] = Sprite::create("plane_background.png");
	background[2]->setAnchorPoint(Vec2(0, 0));
	background[2]->setPosition(Vec2(0, -BACKGROUND_HEIGHT));
	layerBG->addChild(background[2]);
	background[3] = Sprite::create("plane_background.png");
	background[3]->setAnchorPoint(Vec2(0, 0));
	background[3]->setPosition(Vec2(0, -BACKGROUND_HEIGHT));
	layerBG->addChild(background[3]);

	//��layerBG����뵽������
	addChild(layerBG);

	auto *hero = HeroPlane::createHeroPlane(Vec2(180, 100));
	hero->setTag(HERO_TAG);
	addChild(hero);

	//����������ӵ�����
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		bullet[i] = Bullet::createBullet(Vec2(180, 660));
		addChild(bullet[i]);
	}

	//��������ӵ��˶���
	auto* enemy1 = EnemyPlane::createEnemy();
	addChild(enemy1);
	enemy1->start(1, 30);
	auto* enemy2 = EnemyPlane::createEnemy();
	addChild(enemy2);
	enemy2->start(2, 150);
	auto* enemy3 = EnemyPlane::createEnemy();
	addChild(enemy3);
	enemy3->start(3, 280);

	preEnemy();
	preBullet();

	auto* dispatcher = Director::getInstance()->getEventDispatcher();
	auto* listener = EventListenerTouchOneByOne::create();

	// �󶨼����¼�
	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	scheduleUpdate();
	return true;
}

//��ʱ������
void HelloWorld::update(float dt)
{
	//������ͼ������
	background[0]->setPositionY(background[0]->getPositionY() - 1);
	background[1]->setPositionY(background[1]->getPositionY() - 1);

	//�ж�����е�ͼ�����߽���ı���λ��
	if (background[0]->getPositionY() < -BACKGROUND_HEIGHT)
	{
		background[0]->setPositionY(BACKGROUND_HEIGHT);
	}
	if (background[1]->getPositionY() < -BACKGROUND_HEIGHT)
	{
		background[1]->setPositionY(BACKGROUND_HEIGHT);
	}

	//�����ɻ�������
	background[2]->setPositionY(background[2]->getPositionY() + 2);
	background[3]->setPositionY(background[3]->getPositionY() + 2);
	if (background[2]->getPositionY()>BACKGROUND_HEIGHT)
	{
		background[2]->setPositionY(-BACKGROUND_HEIGHT);
	}
	if (background[3]->getPositionY()>BACKGROUND_HEIGHT)
	{
		background[3]->setPositionY(-BACKGROUND_HEIGHT);
	}

	hero = (HeroPlane*)getChildByTag(HERO_TAG);
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		if (bullet[i]->isActive == false)
		{
			bullet[i]->isActive = true;
			bullet[i]->shoot(hero->position);
			break;
		}
	}
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		if (bullet[i]->isActive&&bullet[i]->bullet->getPositionY() == 660)
		{
			bullet[i]->isActive = false;
		}
	}
	produceEnemy();
	produceBullet();
	checkEnemy();
	checkHero();
}

//��ʼ���㴥��
bool HelloWorld::onTouchBegan(Touch* touch, Event* event)
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	Point point = touch->getLocation();
	if (hero->plane->boundingBox().containsPoint(point))
	{
		relativePos = Vec2(point.x - hero->position.x, point.y - hero->position.y);
		return true;
	}
	return false;
}

//���㴥���ƶ�
void HelloWorld::onTouchMoved(Touch* touch, Event* event)
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	Point point = touch->getLocation();
	hero->planeMove(point, relativePos);
}

//���㴥������
void HelloWorld::onTouchEnded(Touch* touch, Event* event)
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	hero->plane->setRotation3D(Vec3(0, 0, 0));
}

//���л����볡����
void HelloWorld::preEnemy()
{
	for (int i = 0; i < ENEMY_NUMBER; i++)
	{
		enemy[i] = EnemyPlane::createEnemy();
		addChild(enemy[i]);
	}
}

//�������������������
void HelloWorld::produceEnemy()
{
	srand(number);
	number = rand();
	int num = number % 150;
	bool isProduce = false;
	int type = 0;
	if (num == 1)
	{
		type = 1;
		isProduce = true;
	}
	else if (num == 2)
	{
		type = 2;
		isProduce = true;
	}
	else if (num == 3)
	{
		type = 3;
		isProduce = true;
	}
	if (isProduce)
	{
		for (int i = 0; i < ENEMY_NUMBER; i++)
		{
			if (enemy[i]->type == 0)
			{
				int positionX = rand() % 360;
				enemy[i]->start(type, positionX);
				break;
			}
		}
	}

	for (int i = 0; i < ENEMY_NUMBER; i++)
	{
		if (enemy[i]->enemy->getPositionY() == END_HEIGHT)
		{
			enemy[i]->type = 0;
		}
	}
}

//���ӵ�Ԫ�ؼ��뵽������
void HelloWorld::preBullet()
{
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		enemyBullet[i] = EnemyBullet::createBullet();
		addChild(enemyBullet[i]);
	}
}
//ѭ�������ӵ�
void HelloWorld::produceBullet()
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		// �����ӵ�
		if (bullet[i]->isActive == false)
		{
			bullet[i]->isActive = true;
			bullet[i]->shoot(hero->position);
			break;
		}
	}
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		if (bullet[i]->isActive&&bullet[i]->bullet->getPositionY() == 660)
		{
			bullet[i]->isActive = false;
		}
	}
	// �Եл��ӵ����й���,���ж��Ƿ���Ҫ�����ӵ�
	if (enemyBulletLock % 100 == 0)
	{
		// �Եл����б�����������Щ�л��ܹ������ӵ�
		for (int i = 0; i < ENEMY_NUMBER; i++)
		{
			if (enemy[i]->type == 1)
			{
				// �Եл��ӵ����б���
				for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
				{
					if (enemyBullet[i]->isActive == false)
					{
						enemyBullet[i]->shoot(enemy[i]->enemy->getPosition());
						break;
					}
				}
			}
		}
	}
	enemyBulletLock++;

	// �Եл��ӵ��Ļ��գ�ֱ�ӶԵл��ӵ����б���
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		if (enemyBullet[i]->bullet->getPositionY() == -80)
		{
			enemyBullet[i]->isActive = false;
		}
	}
}

//����Ƿ���ел�
void HelloWorld::checkEnemy()
{
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		if (bullet[i]->isActive)
		{
			// ����ӵ�����Ч������ԶԵл�����ѭ��
			for (int j = 0; j < ENEMY_NUMBER; j++)
			{
				if (enemy[j]->type > 0)
				{
					float r = 0;

					//���type���Դ�������л���Ч�����Ƿ��뵱ǰ�ӵ�������ײ���м���
					if (enemy[j]->type == 1)
					{
						r = 64.0f;	  // ����Ϊ1���뾶Ϊ64
					}
					else if (enemy[j]->type == 2)
					{
						r = 32.0f;    // ����Ϊ2���뾶Ϊ32
					}
					else
					{
						r = 10.0f;    // ����Ϊ3���뾶Ϊ10
					}

					// �����ӵ���л��е�ľ���
					float dx = bullet[i]->bullet->getPositionX() - enemy[j]->enemy->getPositionX();
					float dy = bullet[i]->bullet->getPositionY() - enemy[j]->enemy->getPositionY();
					float d = sqrt(dx*dx + dy*dy);
					if (d < r)
					{
						enemy[j]->enemyDestory();
						bullet[i]->bullet->stopAllActions();
						bullet[i]->bullet->setPosition(0, 660);
						bullet[i]->isActive = false;
					}
				}
			}
		}
	}
}

//���Ӣ���Ƿ񱻻���
void HelloWorld::checkHero()
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	for (int i = 0; i < ENEMY_NUMBER; i++)
	{
		if (enemy[i]->type > 0)
		{
			// �л���Ч����λ�ý����ж�
			auto point = enemy[i]->enemy->getPosition();
			if (hero->plane->boundingBox().containsPoint(point))
			{
				// Ƥ���𱻵л�ײ��
				CCLOG("Pikachu is die!!!");
			}
		}
	}
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		if (enemyBullet[i]->isActive)
		{
			// �����ӵ���Ч����λ�ý����ж�
			auto point = enemyBullet[i]->bullet->getPosition();
			if (hero->plane->boundingBox().containsPoint(point))
			{
				// Ƥ���𱻵л�ײ��
				CCLOG("Pikachu is die!!!");
			}
		}
	}
}
