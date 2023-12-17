//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define MAX_PLAYER		100

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player{
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;  //누적학점 
	int flag_graduate; //졸업을 할 수 있는지 없는지  
}player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void printGrades(int player); //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}

void printPlayerStatus(void)
{
	int i;
	
	printf("\n\n===========player status===============\n"); 
	
	for (i=0; i<player_nr; i++)
	{
		printf("%s : credit %i, energy %i, position %i\n",
				cur_player[i].name,
				cur_player[i].accumCredit,
				cur_player[i].energy,
				cur_player[i].position);
	}
	printf("=========================================\n\n\n");
}


void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	for(i=0; i<n; i++)
	{
		//input name
	    printf("Input player %i's name:", i);
	    scanf("%s", cur_player[i].name);
	    fflush(stdin);
	
	    //set position
	    //player_position[i] = 0;
	    cur_player[i].position = 0;
	    
	    //set energy
	    //player_energy[i] = initEnergy;
	    cur_player[i].energy = initEnergy;
	    cur_player[i].accumCredit = 0;
	    cur_player[i].flag_graduate = 0;
	}
}


void clearInputBuffer()   //함수를 호출하여 입력버퍼를 비우기 위한 함수 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  //getchar로 한 번에 한 문자씩 입력을 읽어드림. \n이나 파일의 끝을 만날 때까지 입력 소비 
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    
    clearInputBuffer();  //입력 버퍼 비우기 위해 함수 호출 
    
    c = getchar();
    
    if (c == 'g')
        printGrades(player);
        
    return (rand() % MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;
    char response[20];
    
    int validInput = 0;  //입력이 올바른지 확인하기 위한 플래그 변수  
    
    
     switch (type)
    {   	
        case SMMNODE_TYPE_LECTURE:
    do 
	{
        printf("Lecture %s (credit:%i, energy:%i) starts! Are you going to join?: ",   
               smmObj_getNodeName(boardPtr), smmObj_getNodeCredit(boardPtr), smmObj_getNodeEnergy(boardPtr));

        scanf("%s", response);

        if (strcmp(response, "join") == 0) 
        {
        	if (0 <= cur_player[player].energy && cur_player[player].energy < smmObj_getNodeEnergy(boardPtr))
            {
            	printf("%s is too hungry to take the lecture %s. Not enough energy!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));

                // 에너지가 부족하면 다음 턴으로 넘어가도록 설정
                validInput = 1;
			}
			
			else
            {
            	cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

                printf("%s successfully takes the lecture %s\n", cur_player[player].name, smmObj_getNodeName(boardPtr));

                // 학점 생성
                gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, 0);
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

                validInput = 1;  // 플래그 변수가 1이면 do-while 루프를 종료한다.
			}
		}
		else if (strcmp(response, "drop") == 0)
		{
			printf("Player %s drops the lecture %s!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
            validInput = 1;
		}
        else
        {
        	printf("Invalid input! Please enter 'join' or 'drop'.\n");
		}
		
    } while (!validInput);
    	break;  
            
        case SMMNODE_TYPE_FOODCHANCE:
			printf("%s gets a food chance! press any key to pick a food card:", cur_player[player].name);
        	scanf(" %c", response);
        
        	srand(time(NULL));
        	int randomIndex = rand() % food_nr;     //랜덤한 음식 카드를 뽑기 위해서 ramdomIndex라는 변수를 새로 만들었다. 랜덤함수도 사용했다.  
			void *foodObj = smmdb_getData(LISTNO_FOODCARD, randomIndex);    //LISTNO_FOODCARD로 txt를 불러왔다.  
        
        	int foodEnergy = smmObj_getNodeEnergy(foodObj);   //에너지를 업데이트 하기 위해 foodEnergy라는 변수를 선언했다.  
    		cur_player[player].energy += foodEnergy;		//foodObj에서 에너지를 가져와서 현재 플레이어의 에너지를 업데이트하는 코드

    		printf("%s picks %s and charges %i (remained energy: %i)\n", cur_player[player].name, smmObj_getNodeName(foodObj), foodEnergy, cur_player[player].energy);
        
        	break;
        
        
        
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("Let's eat in %s and charge %i energies (remained energy : %i)\n", smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
		 
        
        default: 
            break;
    }
}


void goForward(int player, int step)
{
     void *boardPtr;
     cur_player[player].position += step;
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));                      
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn = 0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) //read a node parameter set
    {
    	//store the parameter set
       void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
       smmdb_addTail(LISTNO_NODE, boardObj); 
       
        if(type == SMMNODE_TYPE_HOME)
           initEnergy = energy; 
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for(i=0; i<board_nr; i++)
    {
    	void* boardObj = smmdb_getData(LISTNO_NODE, i);
		printf("node %i : %s, %i(%s), credit %i, energy %i\n",
		 i, smmObj_getNodeName(boardObj), 
		 smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
		 smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
	}
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy)==2)//read a food parameter set
    {
	    void *foodObj = smmObj_genObject(name, SMMNODE_TYPE_FOODCHANCE, 0, 0, energy, 0);  //foodObj라는 새로운 코드 작성 
		smmdb_addTail(LISTNO_FOODCARD, foodObj);  //위와 같이 코드 작성. LISTNO_NODE-> LISTNO_FOODCARD
		food_nr++; 
    }
    fclose(fp);
    
    for(i=0; i<food_nr; i++) //food_nr까지 반복 
    {
    	void* foodObj = smmdb_getData(LISTNO_FOODCARD, i);
		printf("node %i : %s, charge : %i\n",
		i, smmObj_getNodeName(foodObj), smmObj_getNodeEnergy(foodObj));
	}
	printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name)==1) //read a festival card string
    {
        void *festObj = smmObj_genObject(name, 0, 0, 0, 0, 0);  //festObj라는 새로운 코드 작성 
		smmdb_addTail(LISTNO_FESTCARD, festObj);  //위와 같이 코드 작성. LISTNO_NODE-> LISTNO_FESTCARD
		festival_nr++; 
    }
    fclose(fp);
    
    for(i=0; i<festival_nr; i++)  //festival_nr까지 반복 
    {
    	void* festObj = smmdb_getData(LISTNO_FESTCARD, i); 
		printf("node %i : %s\n", i, smmObj_getNodeName(festObj));
	}
    printf("Total number of festival cards : %i\n\n\n", festival_nr);
    
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
	    //input player number to player_nr
	    printf("input player no.:");
		scanf("%d", &player_nr);
		fflush(stdin);
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    
    generatePlayers(player_nr, initEnergy);
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn +1)%player_nr;
        
    }
    
    free(cur_player);
    return 0;
}
