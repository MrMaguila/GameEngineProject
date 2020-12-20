//#############################################################################################
/*
Copyright[2020][Gabriel G. Fernandes]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.

by Gabriel G. Fernandes 19/12/2020
gabrielgf6000@gmail.com
*/
//#############################################################################################

#include "Game.h"




void Game::handleMultiplayer()
{
	char ch;
	std::string port;
	while (true)
	{
		std::cout << "->Do you want to play multiplayer?(enter 'y' or 'n') ";
		std::cin >> ch;
	
		if (ch == 'n')  //play offline
			return;
		else if (ch == 'y') //the player wants to play multiplayer
		{
			networkHandler.initializeWinsock();
			break;
		}
		else 
		{ 
			std::cout << "Invalid response.\n\n"; 
			std::cin.clear(); 
			std::cin.ignore(INT_MAX, '\n'); 
		}
	}

	while (true)
	{
		std::cout << "Do you want to open to lan or connect to another player's world?(enter '1' or '2') ";
		std::cin >> ch;

		if (ch == '1')
		{
			std::cout << "Enter the port number you want to use(it's recomended to choose a number higher than 7000): ";
			std::cin >> port;
			//networkHandler.initializeSocket(port); //initialize the udp socket
			networkHandler.host(port); //initiaze the tcp socket and listen to connection requests
			
			std::cout << "Oppened to lan on port " << port << ". Players can now connect to this game.\n";
			return;
		}
		else if (ch == '2')
		{
			std::cout << "Enter the ipv4 address of the Host(don't forget the points): ";
			std::string ipAddress;
			std::cin >> ipAddress;
			
			std::cout << "Now enter the port number of the Host: ";
			std::cin >> port;

			//networkHandler.initializeSocket(port); //initialize the udp socket
			networkHandler.connectTo(ipAddress, port);
			std::cout << "Successfully connected to Host.\n";
			return;
		}
		else 
		{ 
			std::cout << "Invalid response.\n"; 
			std::cin.clear(); 
			std::cin.ignore(INT_MAX, '\n'); 
		}
	}

}




//######################################################################################################





void Game::initializeWindow()
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!glfwInit())
	{
		std::cerr << "GLFW ERROR: GLFW failed to initialize!!!\n";
		myAssert(false);
	}

	std::cout << "!GLFW SUCCESSFULLY INITIALIZED;\n";

	glfwSetErrorCallback(error_callback);
	window = glfwCreateWindow(STARTING_WINDOW_WIDTH, STARTING_WINDOW_HEIGHT, "Game", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		myAssert(false);
	}
	std::cout << "!WINDOW SUCCESSFULLY CREATED;\n";
	glfwSetFramebufferSizeCallback(window, resize_callback);
	initializeGraphicsAPI(window);
}




//######################################################################################################




void Game::initializeGame()
{
	//Initialization:
	TextureHandler::instance().addTexture("Assets/images/MagePngTest2.png"); //0
	TextureHandler::instance().addTexture("Assets/images/grass_15.png"); //1
	TextureHandler::instance().addTexture("Assets/images/Image1.png"); //2
	//TextureHandler::instance().addTexture("Assets/Models/boxModel/Sprite-0001.png"); //2
	TextureHandler::instance().addTexture("Assets/images/MagePngTestNormals.png"); //3
	TextureHandler::instance().addTexture("Assets/images/lampPost.png"); //4
	TextureHandler::instance().addTexture("Assets/images/lampPostNormals.png"); //5
	TextureHandler::instance().addTexture("Assets/images/lampPostEmissionMap.png"); //6
	TextureHandler::instance().addTexture("Assets/images/lampPostEmissionMapRed.png"); //7
	TextureHandler::instance().addTexture("Assets/images/MagePngTestEmissionMap.png"); //8
	TextureHandler::instance().addTexture("Assets/images/Huds/Hud0.png"); //9
	TextureHandler::instance().addTexture("Assets/images/Huds/Hud1.png"); //10
	TextureHandler::instance().addTexture("Assets/images/Huds/Hud2.png"); //11
	TextureHandler::instance().addTexture("Assets/images/Huds/Hud3.png"); //12
	TextureHandler::instance().addTexture("Assets/images/Huds/cursor.png"); //13
	TextureHandler::instance().addTexture("Assets/images/grass_17.png"); //14
	TextureHandler::instance().addTexture("Assets/images/wandDiffuse.png"); //15
	TextureHandler::instance().addTexture("Assets/images/wandEmissionMap.png"); //16
	TextureHandler::instance().addTexture("Assets/images/wandNormalMap.png"); //17

	//Hide mouse cursor:
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//initialize the world:
	world.initalize();
	world.setCurrentScene(0);

	//Systems initialization:
	//------------------------------------------------
	aiEngine.intialize(&world);
	aiEngine.addObserver(physicsEngine);

	graphicsEngine.initialize(&world, window); 
	graphicsEngine.bloom = true;

	physicsEngine.initialize(&world);
	physicsEngine.addObserver(gameplayHandler);

	InputHandler::instance().Initialize(&world, window);  //create and initialize the InputHandler
	InputHandler::instance().addObserver(gameplayHandler);

	gameplayHandler.initialize(&world, window);
	gameplayHandler.addObserver(physicsEngine);
	gameplayHandler.addObserver(graphicsEngine);

	//networkHandler.initializeSocket("14000");
	
	ModelHandler::instance().loadModel("Assets/Models/animModel/", "tree.glb", true, 1, 1, true); //0
	ModelHandler::instance().loadModel("Assets/Models/lampModel/", "lampPost.fbx"); //1
	ModelHandler::instance().loadModel("Assets/Models/boxModel/", "box2.fbx"); //2
	ModelHandler::instance().loadModel("Assets/Models/gryphonModel/", "gryphon.dae"); //3
	ModelHandler::instance().loadModel("Assets/Models/terrain/", "terrain.glb", true, 1, 1, true); //4
	ModelHandler::instance().loadModel("Assets/Models/sword/", "sword.glb", true, 1, 1, true); //5
	ModelHandler::instance().loadModel("Assets/Models/wand/", "wand_Back.glb", true, 1, 1, true); //6
	ModelHandler::instance().loadModel("Assets/Models/mage/", "player.glb", true, 4, 4, true); //7
	ModelHandler::instance().loadModel("Assets/Models/campfire/", "campfire.glb", true, 1, 1, true); //8

	//Scenes initialization:
	//-------------------------------------------------
	int id = world.currentScene->createEntity();
	//TransformComponent* 
	TransformComponent* tComp = world.currentScene->getTransformComponent(id);
	myAssert(tComp);

	//physicsEngine.addSpherePhysicalComponent(id, 5.0f, glm::vec3(0.0f, 0.0f, 160.0f));
	//world.currentScene->getSphereRigidBodyComponent(id)->setMass(10.0f);
	physicsEngine.addBoxPhysicalComponent(id, 16, 44, 12, glm::vec3(0.0f, 20.0f, 0.0f));
	world.currentScene->getBoxRigidBodyComponent(id)->setMass(10.0f);

	tComp->setPosition(glm::vec3(0.0f, 0.0f, 160.0f));
	tComp->setScale(glm::vec3(32.0f, 32.0f, 32.0f));
	tComp->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//tComp->rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	//graphicsEngine.addImageComponent(id, TextureHandler::instance().get(0), 12, 4);
	//graphicsEngine.addNormalMap(id, TextureHandler::instance().get(3));
	//graphicsEngine.addEmissionMap(id, TextureHandler::instance().get(8));

	gameplayHandler.createCharacterComponent(id, int(CharacterType::Player));
	world.currentScene->getCharacterComponent(id)->setModelPos(glm::vec3(0.0f, -28.0f, 0.0f));
	world.currentScene->getCharacterComponent(id)->setModel(
		ModelHandler::instance().getModel(7));
	gameplayHandler.setPlayer(id);
	
	//--------------------------------------------------------------------------
	int id11 = world.currentScene->createEntity();


	tComp = world.currentScene->getTransformComponent(id11);
	myAssert(tComp);

	tComp->setScale(glm::vec3(12.0, 12.0f, 12.0f));
	tComp->setPosition(glm::vec3(0.0f, -6.0f, 0.0f));
	tComp->rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//graphicsEngine.addImageComponent(id11, TextureHandler::instance().get(15), 1, 1);
	//graphicsEngine.addNormalMap(id11, TextureHandler::instance().get(17));
	//graphicsEngine.addEmissionMap(id11, TextureHandler::instance().get(16));

	
	gameplayHandler.createInteractableObjComp(id11, glm::vec3(6, 32, 2), EffectType::swordSwing00, 
		ModelHandler::instance().getModel(5));
	gameplayHandler.setObjectHolder(id11, id);
	
	//---------------------------------------------------
	
	int id111 = world.currentScene->createEntity();

	tComp = world.currentScene->getTransformComponent(id111);
	myAssert(tComp);

	tComp->setScale(glm::vec3(28.0, 28.0f, 28.0f));
	tComp->setPosition(glm::vec3(0.0f, 0.0f, -10.0f));

	physicsEngine.addBoxPhysicalComponent(id111, 24, 12, 24, glm::vec3(0.0f, -14.0f, 0.0f));
	world.currentScene->getBoxRigidBodyComponent(id111)->setMass(-1); //infinite mass
	graphicsEngine.addModelComponent(id111, ModelHandler::instance().getModel(8));
	gameplayHandler.createInteractableObjComp(id111, glm::vec3(16, 32, 16), EffectType::fireEffect01, nullptr);
	

	gameplayHandler.startEffect(id111, EffectType::fireEffect01, nullptr);
	
	//---------------------------------------------------
	int id2 = world.currentScene->createEntity();
	//graphicsEngine.addImageComponent(id2, TextureHandler::instance().get(1), 1, 1);
	graphicsEngine.addModelComponent(id2, ModelHandler::instance().getModel(4));

	physicsEngine.addBoxPhysicalComponent(id2, 640, 2, 640, glm::vec3(0.0f, -20.0f, 0.0f));
	world.currentScene->getBoxRigidBodyComponent(id2)->setMass(-1); //infinite mass

	//ImageComponent* imagComp = world.currentScene->getImageComponent(id2);
	//myAssert(imagComp);
	tComp = world.currentScene->getTransformComponent(id2);
	myAssert(tComp);
	//tComp->transform = glm::translate(tComp->transform, glm::vec3(0.0f, -20.0f, 0.0f));
	//tComp->transform = glm::rotate(tComp->transform, FLOAT_TYPE(glm::radians(-90.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
	//tComp->transform = glm::rotate(tComp->transform, FLOAT_TYPE(glm::radians(180.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
	//tComp->setPosition(glm::vec3(0.0f, -20.0f, 0.0f));
	//tComp->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//tComp->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	tComp->setScale(glm::vec3(50.0f, 50.0f, 50.0f));
	//set transforms hierarchy:
	//tComp = &(world.currentScene->getTransformComponent(id));
	//tComp->setParent(id2);

	
	//-------------------------------------------
	int id3 = world.currentScene->createEntity();
	TransformComponent* tComp2 = world.currentScene->getTransformComponent(id3);
	myAssert(tComp2);
	//tComp->setScale
	//tComp->transform = glm::translate(tComp->transform, glm::vec3(0.0f, 0.0f, -50.0f));

	//physicsEngine.addSpherePhysicalComponent(id3, 5.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//world.currentScene->getSphereRigidBodyComponent(id3)->setMass(10.0f);
	physicsEngine.addBoxPhysicalComponent(id3, 24, 150, 24, glm::vec3(46.0f, 32.0f, 150.0f));
	world.currentScene->getBoxRigidBodyComponent(id3)->setMass(-1.0f);


	//tComp->setPosition(glm::vec3(9.0f, -5.0f, 0.0f));


	//tComp->transform = glm::scale(tComp->transform, glm::vec3(16.0f, 32.0f, 32.0f));


	//tComp2->rotate(-90.0f, glm::normalize(glm::vec3(.0f, 1.0f, 0.0f)));
	//tComp2->rotate(-90.0f, glm::vec3(1.0f, .0f, 0.0f));
	tComp2->rotate(180.0f, glm::vec3(.0f, 1.0f, 0.0f));
	tComp2->setScale(glm::vec3(24.0f, 24.0f, 24.0f));
	//tComp2->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	//tComp->setParent(id);
	//graphicsEngine.addImageComponent(id3, TextureHandler::instance().get(2), 1, 1);
	graphicsEngine.addModelComponent(id3, ModelHandler::instance().getModel(0));
	world.currentScene->getModelComponent(id3)->cullFace = false;
	world.currentScene->getModelComponent(id3)->setPosition(glm::vec3(0.0f, -56.0f, 0.0f));

	//-----------------------------------------------------

	int id33 = world.currentScene->createEntity();
	tComp2 = world.currentScene->getTransformComponent(id33);
	myAssert(tComp2);
	//tComp->setScale
	//tComp->transform = glm::translate(tComp->transform, glm::vec3(0.0f, 0.0f, -50.0f));

	//physicsEngine.addSpherePhysicalComponent(id3, 5.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//world.currentScene->getSphereRigidBodyComponent(id3)->setMass(10.0f);
	physicsEngine.addBoxPhysicalComponent(id33, 16, 32, 32, glm::vec3(46.0f, 32.0f, 150.0f));
	world.currentScene->getBoxRigidBodyComponent(id33)->setMass(10.0f);


	//tComp->setPosition(glm::vec3(9.0f, -5.0f, 0.0f));


	//tComp->transform = glm::scale(tComp->transform, glm::vec3(16.0f, 32.0f, 32.0f));


	//tComp2->rotate(-90.0f, glm::normalize(glm::vec3(.0f, 1.0f, 0.0f)));
	tComp2->rotate(-90.0f, glm::vec3(1.0f, .0f, 0.0f));
	tComp2->rotate(135.0f, glm::vec3(.0f, 1.0f, 0.0f));
	tComp2->setScale(glm::vec3(16.0f, 16.0f, 16.0f));
	//tComp2->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	//tComp->setParent(id);
	//graphicsEngine.addImageComponent(id3, TextureHandler::instance().get(2), 1, 1);
	//graphicsEngine.addModelComponent(id33, ModelHandler::instance().getModel(3));
	//world.currentScene->getModelComponent(id33)->cullFace = false;
	//world.currentScene->getModelComponent(id33)->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//---------------------------------------------
	int id4 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id4);
	myAssert(tComp);
	//tComp->transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 100.0f, -100.0f));
	tComp->setPosition(glm::vec3(0.0f, 100.0f, -100.0f));

	graphicsEngine.addDirLightComponent(id4, glm::vec3(238.0f, 139.0f, 61.0f) / 6.0f, glm::vec3(-0.5f, -1.0f, 1.0f), true);
	//graphicsEngine.addDirLightComponent(id4, glm::vec3(68.0f, 40.0f, 100.0f) / 15.0f, glm::vec3(-0.5f, -1.0f, 1.0f), true);


	//---------------------------------------------
	int id44 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id44);
	myAssert(tComp);
	//tComp->transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 100.0f, -100.0f));
	tComp->setPosition(glm::vec3(0.0f, 100.0f, -100.0f));

	//graphicsEngine.addDirLightComponent(id44, glm::vec3(425.0f, 240.0f, 120.0f) / 200.0f, glm::vec3(-0.5f, -1.0f, -1.0f), false);
	//graphicsEngine.addDirLightComponent(id4, glm::vec3(35.0f, 30.0f, 80.0f) / 50.0f, glm::vec3(-0.5f, -1.0f, 1.0f));

	//---------------------------------------------
	int id5 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id5);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(-100.0f, -10.0f, -82.0f));
	//tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	//tComp->setParent(id3);

	//graphicsEngine.addImageComponent(id5, TextureHandler::instance().get(4), 1, 1, true);

	//graphicsEngine.addImageComponent(id5, TextureHandler::instance().get(4), 1, 1, true);
	//graphicsEngine.addNormalMap(id5, TextureHandler::instance().get(5));
	//graphicsEngine.addPointLightComponent(id5, glm::vec3(50.0f, 1.0f, 1.0f), 0.04f, 0.017f); //!

	//---------------------------------------------
	int id55 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id55);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(-100.0f, 20.0f, -80.0f));
	tComp->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
	tComp->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	tComp->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//tComp->rotate(135.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//tComp->setOrientation();
	//tComp->setParent(id3);

	//graphicsEngine.addImageComponent(id5, TextureHandler::instance().get(4), 1, 1, true);
	physicsEngine.addBoxPhysicalComponent(id55, 4, 4, 60, glm::vec3(-50.0f, 8.0f, 30.0f));
	world.currentScene->getBoxRigidBodyComponent(id55)->setMass(4.0f);
	//graphicsEngine.addImageComponent(id55, TextureHandler::instance().get(4), 1, 1);
	//graphicsEngine.addNormalMap(id55, TextureHandler::instance().get(5));
	//graphicsEngine.addEmissionMap(id55, TextureHandler::instance().get(7));
	graphicsEngine.addModelComponent(id55, ModelHandler::instance().getModel(1));
	ModelComponent* mComp = world.currentScene->getModelComponent(id55);
	myAssert(mComp);
	mComp->setPosition(glm::vec3(0.0f, -32.0f, 0.0f));
	//graphicsEngine.addPointLightComponent(id5, glm::vec3(15.0f, 5.0f, 5.0f), 0.022f, 0.0019f);

	//--------------------------------------------
	/*
	int id555 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id555);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(-100.0f, 20.0f, -80.0f));
	tComp->setScale(glm::vec3(16.0f, 16.0f, 16.0f));
	tComp->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	tComp->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//tComp->rotate(135.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//tComp->setOrientation();
	//tComp->setParent(id3);

	//graphicsEngine.addImageComponent(id5, TextureHandler::instance().get(4), 1, 1, true);
	physicsEngine.addBoxPhysicalComponent(id555, 4, 4, 60, glm::vec3(-10.0f, 8.0f, 100.0f));
	world.currentScene->getBoxRigidBodyComponent(id555)->setMass(4.0f);
	//graphicsEngine.addImageComponent(id55, TextureHandler::instance().get(4), 1, 1);
	//graphicsEngine.addNormalMap(id55, TextureHandler::instance().get(5));
	//graphicsEngine.addEmissionMap(id55, TextureHandler::instance().get(7));
	graphicsEngine.addModelComponent(id555, ModelHandler::instance().getModel(5));
	mComp = world.currentScene->getModelComponent(id555);
	myAssert(mComp);
	mComp->setPosition(glm::vec3(0.0f, -24.0f, 0.0f));
	//graphicsEngine.addPointLightComponent(id5, glm::vec3(15.0f, 5.0f, 5.0f), 0.022f, 0.0019f);
	*/
	//--------------------------------------------

	/*
	int id6 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id6);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(0.0f, 20.0f, 28.0f));
	//tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	//tComp->setParent(id3);

	graphicsEngine.addImageComponent(id6, TextureHandler::instance().get(4), 1, 1);
	graphicsEngine.addNormalMap(id6, TextureHandler::instance().get(5));
	graphicsEngine.addEmissionMap(id6, TextureHandler::instance().get(6));
	//graphicsEngine.addPointLightComponent(id6, glm::vec3(15.0f, 15.0f, 15.0f), 0.022f, 0.0019f);
	*/
	//--------------------------------------------

	int id7 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id7);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(0.0f, -14.0f, 0.0f));
	//tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	tComp->setParent(id55);

	//graphicsEngine.addImageComponent(id6, TextureHandler::instance().get(4), 1, 1, true);
	//graphicsEngine.addNormalMap(id7, TextureHandler::instance().get(5));
	graphicsEngine.addPointLightComponent(id7, glm::vec3(30.0f, 20.0f, 10.0f), 0.04f, 0.002f);

	//--------------------------------------------

	int id777 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id777);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	//tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	tComp->setParent(id111);

	//graphicsEngine.addImageComponent(id6, TextureHandler::instance().get(4), 1, 1, true);
	//graphicsEngine.addNormalMap(id7, TextureHandler::instance().get(5));
	graphicsEngine.addPointLightComponent(id777, glm::vec3(30.0f, 20.0f, 0.0f), 0.04f, 0.002f);


	//----------------------------------------------------------------

	int id77 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id77);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(40.0f, -10.0f, -106.0f));
	tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	//tComp->setParent(id3);

	//graphicsEngine.addImageComponent(id6, TextureHandler::instance().get(4), 1, 1, true);
	//graphicsEngine.addNormalMap(id7, TextureHandler::instance().get(5));

	//------------------------------------------------------------------

	int id78 = world.currentScene->createEntity();
	tComp = world.currentScene->getTransformComponent(id78);
	myAssert(tComp);
	//tComp->move(glm::vec3(00.0f, 20.0f, 2.0f)); //glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, -1.0f));
	tComp->setPosition(glm::vec3(40.0f, 0.0f, 100.0f));
	//tComp->setScale(glm::vec3(64.0f, 96.0f, 16.0f));
	//tComp->setParent(id3);

	//graphicsEngine.addImageComponent(id6, TextureHandler::instance().get(4), 1, 1, true);
	//graphicsEngine.addNormalMap(id7, TextureHandler::instance().get(5));
	//graphicsEngine.addPointLightComponent(id78, glm::vec3(50.0f, 10.0f, 5.0f), 0.07f, 0.01f);

	//------------------------------------------------------------------

	int id8 = world.currentScene->createEntity();
	//TransformComponent* 
	tComp = world.currentScene->getTransformComponent(id8);
	myAssert(tComp);

	//physicsEngine.addSpherePhysicalComponent(id, 5.0f, glm::vec3(0.0f, 0.0f, 160.0f));
	//world.currentScene->getSphereRigidBodyComponent(id)->setMass(10.0f);
	physicsEngine.addBoxPhysicalComponent(id8, 16, 40, 12, glm::vec3(0.0f, 20.0f, 0.0f));
	world.currentScene->getBoxRigidBodyComponent(id8)->setMass(10.0f);

	tComp->setPosition(glm::vec3(0.0f, 0.0f, 160.0f));
	tComp->setScale(1.2f * glm::vec3(26.0f, 26.0f, 26.0f));
	tComp->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	//graphicsEngine.addImageComponent(id8, TextureHandler::instance().get(0), 12, 4);
	//graphicsEngine.addNormalMap(id8, TextureHandler::instance().get(3));
	//graphicsEngine.addEmissionMap(id, TextureHandler::instance().get(8));

	gameplayHandler.createCharacterComponent(id8, int(CharacterType::Enemy));
	world.currentScene->getCharacterComponent(id8)->movingSpeed = 1.0f;

	world.currentScene->getCharacterComponent(id8)->setModelPos(glm::vec3(0.0f, -28.0f, 0.0f));
	world.currentScene->getCharacterComponent(id8)->setModel(
		ModelHandler::instance().getModel(7));
	//gameplayHandler.setPlayer(id8);

	//=======================================

	std::vector<int> objects;
	for (int i = 0; i < 200; ++i)
	{
		int id9 = world.currentScene->createEntity();
		objects.push_back(id9);
		tComp2 = world.currentScene->getTransformComponent(id9);
		myAssert(tComp2);
		
		tComp2->setPosition(glm::vec3(std::rand() % 400 - 200, -10.0f, std::rand() % 400 - 200));

	}


	for (int i = 0; i < objects.size(); ++i)
		world.currentScene->deleteEntity(objects[i]);
	/*
	for (int i = 0; i < 60; ++i)
	{
		int id9 = world.currentScene->createEntity();
		objects.push_back(id9);
		tComp2 = world.currentScene->getTransformComponent(id9);
		myAssert(tComp2);

		tComp2->setPosition(glm::vec3(std::rand() % 400 - 200, -12.0f, std::rand() % 400 - 200));

		//tComp2->rotate(-90.0f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		//tComp2->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//tComp2->rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		//tComp->setParent(id);
		graphicsEngine.addImageComponent(id9, TextureHandler::instance().get(14), 1, 8);
		world.currentScene->getImageComponent(id9)->play(15, 0);
		world.currentScene->getImageComponent(id9)->instanced = true;
	}
	*/
}





//######################################################################################################





void Game::gameLoop()
{
	//CharacterComponent* cComp = world.currentScene->getCharacterComponent(0);

	//fps counting variables:
	double currentTime = glfwGetTime();
	int frameCount = 0;
	int simulationsCount = 0;

	
	double previous = glfwGetTime();
	double lag = 0.0f;

	std::chrono::time_point<std::chrono::steady_clock> physicalTime;
	auto currentTime3 = std::chrono::high_resolution_clock::now();

	glfwSwapInterval(0);
	
	std::cout << "\n========================================================================\n"
		<< "Starting Game Loop: \n\n";

	//the game loop:
	while (!glfwWindowShouldClose(window) && running)
	{
		
		
		//frameStart:
		double frameBegin = glfwGetTime();
		double timeElapsed = frameBegin - previous; //time elapsed since frame start, in seconds
		previous = frameBegin;
		
		//===================================================================================
		//Input, physics and AI:
		
		auto time3 = std::chrono::high_resolution_clock::now();
		auto time2 = std::chrono::high_resolution_clock::now();

		lag += timeElapsed;
		while (physicsEngine.getTimeStep() <= lag)
		{

		
			//Send and clear all messages:
			InputHandler::instance().notify();
			gameplayHandler.notify();
			physicsEngine.notify();
			aiEngine.notify();
			graphicsEngine.update();

			simulationsCount += 1;
			lag -= physicsEngine.getTimeStep();

			//handle Input:
			InputHandler::instance().update();

			//physics update:
			time3 = std::chrono::high_resolution_clock::now();
			physicsEngine.update();
			time2 = std::chrono::high_resolution_clock::now();
			//Game logic:
			gameplayHandler.update();
			aiEngine.update(physicsEngine.getTimeStep());

			//update camera pos:
			glm::vec3 newPos = gameplayHandler.getCameraPos(); //the camera position is handled by the gameplay
																//handler, to handle things like cut scenes
																//and switching the character controlled by the
																//player.

			//if (glm::length(graphicsEngine.camPosition - newPos) >= 1.0f)
			graphicsEngine.camPosition = newPos;

			
		}

		
		glfwPollEvents();
	

		//====================================================================================
		//Rendering
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		int offset = (-16.0f * FLOAT_TYPE(height) / 18) + (FLOAT_TYPE(width) / 2);

		
		
		graphicsEngine.render(offset, width, height);
		
		auto time1 = std::chrono::high_resolution_clock::now();
		
		
		//auto time1 = glfwGetTime();//std::chrono::high_resolution_clock::now();
	
		++frameCount;
		
		if (glfwGetTime() - currentTime >= 1.0) //if one second has been elapsed
		{
			std::cout << "FPS: " << frameCount << ", "<< "Simulations: " << simulationsCount << ' ' 
				<< world.currentScene->transformComponents.getSize() <<'\n';
			frameCount = 0;
			simulationsCount = 0;
			currentTime = glfwGetTime();
		}

		//==========================================================================
		
		
		if (frameCount == 12)
		{
			std::cout << "FrameDur: " << std::chrono::duration_cast<std::chrono::microseconds>(time1 - time2).count()
				<< ", " << std::chrono::duration_cast<std::chrono::microseconds>(time2 - time3).count() << '\n';

			networkHandler.update();
		}
	}

	

	glfwDestroyWindow(window);
	glfwTerminate();
}