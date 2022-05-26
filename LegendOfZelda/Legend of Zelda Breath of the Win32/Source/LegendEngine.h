#pragma once
class LegendEngine
{
	private:

		struct BG {
			int x;
			int y;
			int cX = 1792;					// ETT RUM:	y=168 px, x=256px
			int cY = 1176;
			int sizeX = 256;
			int sizeY = 168;
		};
		BG			background;
	
		int			player_Hei = 96;	// player width = 15 = 10 rutor h�g
		int			player_Wid = 90;	// player Height = 16 = 9 rutor bredd
		char		map[192][100];
		char		hittmp[4];
		bool		menuActive = true;
		bool		pause = false;
		int			a = 0;
		//bool		swordItem = true;
		bool		running = true;
		int			y;
	public:
		struct user {
			int x = 1920 / 2;
			int y = 1080 / 2;
			int posX = 1920 / 10;
			int posY = 1080 / 10;
			int cX = 0;					// en lin g�: y=16 px, x=15px
			int cY = 0;
			int sizeX = 15;
			int sizeY = 16;
			char face = 'D';
			bool idle = true;
			bool attack = false;
			int moners;
			int hp = 5;
		};
		user		player;
		// setters 
		void		setInput(int, bool);
		void		setIdle(bool);
		// getters
		char		getDir();
		user		getLink();

		void		bombs();
		void		swordItem();
		void		spinattack();
		void		bow();
		void		arrows();
		void		AI();
		void		enemie();
		void		shopKepper();
		void		runRight();				// spelaren springer h�ger
		void		runLeft();				// spelaren springer v�nster
		void		runUp();				// spelaren springer upp�t
		void		runDown();				// spelaren springer ner�t
		void		playerAnimation();		// animation f�r spelaren
		void		swordAnimation();
		// menu
		void        newGame();				// startar spel fr�n menyn
		void		exitGame();				// avsluta spelet 
		void		choice();				// vad f�r aval man v�ljer i menyn
		void        getActive();			// h�mtar aktivt menyval
		// game progression
		void		entrance(char);			// flytta bakgrunden
		void		calculateBorder();		// r�kna ut om man borde flyta backgrund
		void		playerPos();
		void		collision();
		void		createBox(int, int, int, int);
		// kill kill kill
		void		attackSword();

		// tmp
		void		createMap();
};

