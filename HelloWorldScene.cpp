/*
* 日期：2019/5/20
* 作者：杜盼芝
* 功能：实现飞机大战
*/
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
using namespace cocostudio::timeline;
#define BACKGROUND_HEIGHT 638
#define HERO_TAG 99
USING_NS_CC;

//创建场景
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
	//创建新的层layerBG用于显示背景
	auto* layerBG = Layer::create();

	//背景地图层，background[0]和background[1]循环滚动，显示背景地图
	background[0] = Sprite::create("background.png");
	background[0]->setAnchorPoint(Vec2(0, 0));
	background[0]->setPosition(Vec2(0, 0));
	layerBG->addChild(background[0]);
	background[1] = Sprite::create("background.png");
	background[1]->setAnchorPoint(Vec2(0, 0));
	background[1]->setPosition(Vec2(0, BACKGROUND_HEIGHT));
	layerBG->addChild(background[1]);

	//背景飞机层，background[2]和background[3]循环滚动，显示飞机飞过地面效果
	background[2] = Sprite::create("plane_background.png");
	background[2]->setAnchorPoint(Vec2(0, 0));
	background[2]->setPosition(Vec2(0, -BACKGROUND_HEIGHT));
	layerBG->addChild(background[2]);
	background[3] = Sprite::create("plane_background.png");
	background[3]->setAnchorPoint(Vec2(0, 0));
	background[3]->setPosition(Vec2(0, -BACKGROUND_HEIGHT));
	layerBG->addChild(background[3]);

	//将layerBG层加入到场景中
	addChild(layerBG);

	auto *hero = HeroPlane::createHeroPlane(Vec2(180, 100));
	hero->setTag(HERO_TAG);
	addChild(hero);

	//场景中添加子弹对象
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		bullet[i] = Bullet::createBullet(Vec2(180, 660));
		addChild(bullet[i]);
	}

	//场景中添加敌人对象
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

	// 绑定监听事件
	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	scheduleUpdate();
	return true;
}

//定时器更新
void HelloWorld::update(float dt)
{
	//背景地图层下移
	background[0]->setPositionY(background[0]->getPositionY() - 1);
	background[1]->setPositionY(background[1]->getPositionY() - 1);

	//判断如果有地图超出边界则改变其位置
	if (background[0]->getPositionY() < -BACKGROUND_HEIGHT)
	{
		background[0]->setPositionY(BACKGROUND_HEIGHT);
	}
	if (background[1]->getPositionY() < -BACKGROUND_HEIGHT)
	{
		background[1]->setPositionY(BACKGROUND_HEIGHT);
	}

	//背景飞机层上升
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

//开始单点触摸
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

//单点触摸移动
void HelloWorld::onTouchMoved(Touch* touch, Event* event)
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	Point point = touch->getLocation();
	hero->planeMove(point, relativePos);
}

//单点触摸结束
void HelloWorld::onTouchEnded(Touch* touch, Event* event)
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	hero->plane->setRotation3D(Vec3(0, 0, 0));
}

//将敌机加入场景中
void HelloWorld::preEnemy()
{
	for (int i = 0; i < ENEMY_NUMBER; i++)
	{
		enemy[i] = EnemyPlane::createEnemy();
		addChild(enemy[i]);
	}
}

//按随机数方法产生敌人
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

//将子弹元素加入到场景中
void HelloWorld::preBullet()
{
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		enemyBullet[i] = EnemyBullet::createBullet();
		addChild(enemyBullet[i]);
	}
}
//循环产生子弹
void HelloWorld::produceBullet()
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		// 发射子弹
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
	// 对敌机子弹进行管理,先判断是否需要发射子弹
	if (enemyBulletLock % 100 == 0)
	{
		// 对敌机进行遍历，看有哪些敌机能够发射子弹
		for (int i = 0; i < ENEMY_NUMBER; i++)
		{
			if (enemy[i]->type == 1)
			{
				// 对敌机子弹进行遍历
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

	// 对敌机子弹的回收，直接对敌机子弹进行遍历
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		if (enemyBullet[i]->bullet->getPositionY() == -80)
		{
			enemyBullet[i]->isActive = false;
		}
	}
}

//检测是否击中敌机
void HelloWorld::checkEnemy()
{
	for (int i = 0; i < BULLET_NUMBER; i++)
	{
		if (bullet[i]->isActive)
		{
			// 如果子弹是有效的则可以对敌机进行循环
			for (int j = 0; j < ENEMY_NUMBER; j++)
			{
				if (enemy[j]->type > 0)
				{
					float r = 0;

					//如果type属性大于零则敌机有效，对是否与当前子弹发生碰撞进行计算
					if (enemy[j]->type == 1)
					{
						r = 64.0f;	  // 类型为1，半径为64
					}
					else if (enemy[j]->type == 2)
					{
						r = 32.0f;    // 类型为2，半径为32
					}
					else
					{
						r = 10.0f;    // 类型为3，半径为10
					}

					// 计算子弹与敌机中点的距离
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

//检测英雄是否被击中
void HelloWorld::checkHero()
{
	auto* hero = (HeroPlane*)getChildByTag(HERO_TAG);
	for (int i = 0; i < ENEMY_NUMBER; i++)
	{
		if (enemy[i]->type > 0)
		{
			// 敌机有效，对位置进行判断
			auto point = enemy[i]->enemy->getPosition();
			if (hero->plane->boundingBox().containsPoint(point))
			{
				// 皮卡丘被敌机撞上
				CCLOG("Pikachu is die!!!");
			}
		}
	}
	for (int i = 0; i < ENEMYBULLET_NUMBER; i++)
	{
		if (enemyBullet[i]->isActive)
		{
			// 敌人子弹有效，对位置进行判断
			auto point = enemyBullet[i]->bullet->getPosition();
			if (hero->plane->boundingBox().containsPoint(point))
			{
				// 皮卡丘被敌机撞上
				CCLOG("Pikachu is die!!!");
			}
		}
	}
}
