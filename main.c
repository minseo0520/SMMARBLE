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
	int experimentFailed; //실험 성공 실패 여부 
	int experimentThreshold;
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
     smmObjGrade_e grade;

     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)  //플레이어의 학점 목록 반복 
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         smmObjGrade_e grade = ((smmObject_t*)gradePtr)->grade;   //학점 개체의 등급을 가져온다. 
         printf("%s(credit : %i) : %s\n", smmObj_getNodeName(gradePtr), smmObj_getNodeCredit(gradePtr), smmObj_getNodeGrade(grade)); 
     }
}


void printPlayerStatus(void)
{
	int i;
	
	printf("\n\n===========player status=================\n"); 
	
	for (i=0; i<player_nr; i++)
	{
		printf("%s : credit %i, energy %i, position %i\n",
				cur_player[i].name,
				cur_player[i].accumCredit,
				cur_player[i].energy,
				cur_player[i].position);
	}
	printf("==========================================\n\n\n");
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
	    cur_player[i].experimentThreshold = 0;
	}
}


void clearInputBuffer()   //함수를 호출하여 입력버퍼를 비우기 위한 함수 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  //getchar로 한 번에 한 문자씩 입력을 읽어드린다. \n이나 파일의 끝을 만날 때까지 입력 소비 
}


int rolldie(int player)
{
    char c;
    printf("This is %s's turn:::::Press any key to roll a die (press g to see grade): ", cur_player[player].name);
    
    clearInputBuffer();  //입력 버퍼 비우기 위해 함수 호출 
    
    c = getchar();
    
    if (c == 'g')
        printGrades(player);
        
    return (rand() % MAX_DIE + 1);
}



float calcAverageGrade(int player) {
    int i;
    float totalGrade = 0.0;  //학점 평균을 계산하기 위해 총 학점이라는 새로운 변수를 정의했다.  

    int numCourses = smmdb_len(LISTNO_OFFSET_GRADE + player);  // 수강한 강의 수라는 새로운 변수를 정의했다.  

    for (i = 0; i < numCourses; i++)   //학점을 더해주는 for문이다.
	{        
        smmObject_t *gradePtr = (smmObject_t *)smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        totalGrade += gradePtr->grade;
    }

    if (numCourses > 0)    //평균을 계산하는 코드이다.
	{       
        return totalGrade / numCourses;
    } 

    else 
	{
        return 0.0;
    }
}


#include <stdbool.h>


//action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);

    void *gradePtr;
    char response[20];

    smmObjGrade_e grade;

    int turn = 0;

    int validInput = 0;  //입력이 올바른지 확인하기 위한 플래그 변수  
    
    float calcAverageGrade(int player);
    int i;
    
     switch (type)
    {   	
        case SMMNODE_TYPE_LECTURE:
    		do 
			{
        		printf("Lecture %s (credit:%i, energy:%i) starts! Are you going to join?: ",   
               	smmObj_getNodeName(boardPtr), smmObj_getNodeCredit(boardPtr), smmObj_getNodeEnergy(boardPtr));
        		scanf("%s", response);
        		if (strcmp(response, "join") == 0) //플레이어가 join을 입력하면 
        		{
        			if (0 <= cur_player[player].energy && cur_player[player].energy < smmObj_getNodeEnergy(boardPtr))
            		{
            			printf("%s is too hungry to take the lecture %s. Not enough energy!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
                		// 에너지가 부족하면 다음 턴으로 넘어가도록 설정
                		validInput = 1;
					}
					
					else
            		{
                		// 학점을 이미 수강했는지 확인
                		int alreadyTaken = 0;  //이미 수강했는지에 대한 플래그 변수 
                		for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
                		{
                    		void *takenGradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);  //플레이어가 수강한 강의에 접근 
                    		if(strcmp(smmObj_getNodeName(takenGradePtr), smmObj_getNodeName(boardPtr)) == 0)
                    		{
                        		alreadyTaken = 1;
                        		break;
                    		}
                		}

                		if (alreadyTaken)
                		{
                    		printf("%s has already taken %s!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
                    		validInput = 1; //다음 턴으로 넘기기 
                		}
                		
                	
                	
					else
           	 		{
           	 			smmObjGrade_e getRandomGrade()		//랜덤으로 등급을 반환하는 함수를 정의했다. 
						{ 
    						srand((unsigned int)time(NULL));    // 난수 발생기 초기화

    						int randomIndex = rand() % 9;       // 난수 생성 (0부터 8까지)

    						return (smmObjGrade_e)randomIndex;  // 난수에 해당하는 등급 반환
    					}

            			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr); 

                		smmObjGrade_e randomGrade = getRandomGrade();   //랜덤으로 학점이 생성 
                		grade = ((smmObject_t*)gradePtr)->grade; //현재 학점 개체의 등급 

                		printf("%s successfully takes the lecture %s with grade %s(average : %f, remained energy : %i)\n", cur_player[player].name, 
						smmObj_getNodeName(boardPtr), smmObj_getNodeGrade(randomGrade), calcAverageGrade(player), cur_player[player].energy);

                		// 학점 생성
                		gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, randomGrade);  //수강한 강의의 이름과 credit, 학점을 저장한다.  
                		smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

                		validInput = 1;  // 플래그 변수가 1이면 do-while 루프를 종료한다.
				    	}
					}
				
				}
				
				else if (strcmp(response, "drop") == 0)  //플레이어가 drop를 입력하면 
				{
					printf("Player %s drops the lecture %s!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
            		validInput = 1;
				}
				
        		else
        		{
        			printf("Invalid input! Please enter 'join' or 'drop'.\n");  //잘못된 문자를 입력했을때 
				}
    		}while (!validInput);
    		break;  
           
		   
		    
        case SMMNODE_TYPE_FOODCHANCE:
			printf("%s gets a food chance! press any key to pick a food card:", cur_player[player].name);
        	scanf(" %c", response);
        
        	srand(time(NULL));
        	int randomfood = rand() % food_nr;     //랜덤한 음식 카드를 뽑기 위해서 ramdomIndex라는 변수를 새로 만들었다. 랜덤함수도 사용했다.  
			void *foodObj = smmdb_getData(LISTNO_FOODCARD, randomfood);    //LISTNO_FOODCARD로 txt를 불러왔다.  
        
        	int foodEnergy = smmObj_getNodeEnergy(foodObj);   //에너지를 업데이트 하기 위해 foodEnergy라는 변수를 선언했다.  
    		cur_player[player].energy += foodEnergy;   //foodObj에서 에너지를 가져와서 현재 플레이어의 에너지를 업데이트하는 코드
    		
    		
    		printf("%s picks %s and charges %i (remained energy: %i)\n", cur_player[player].name, smmObj_getNodeName(foodObj), foodEnergy, cur_player[player].energy);
        
        	break;
        
        
        
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("Let's eat in %s and charge %i energies (remained energy : %i)\n", smmObj_getNodeName(boardPtr), 
			smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);  //식당에 도착하면 에너지가 찬다.  
        	break;
		 
        
        
        case SMMNODE_TYPE_GOTOLAB:
    			cur_player[player].experimentThreshold = rand() % MAX_DIE + 1;  //랜덤하게 주사위의 수를 탈출조건으로 뽑는다.  
    
    			printf("Experiment time! Let's see if you satisfy professor (threshold : %i)\n", cur_player[player].experimentThreshold);

    			int experimentResult = rolldie(turn);  //실험결과라는 변수를 새로 정의했다.  

    			if (experimentResult >= cur_player[player].experimentThreshold)   //실험결과가 탈출조건을 만족함을 묻는 조건이다.  
				{
        			printf("Experiment result : %i, success! %s can exit this lab!\n", experimentResult, cur_player[player].name);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			cur_player[player].experimentFailed = 1;   //실험성공이면 플래그 변수 1 
    			} 
				
				else 
				{
        			printf("Experiment result : %i, fail T_T. %s needs more experiment....\n", experimentResult, cur_player[player].name);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			cur_player[player].experimentFailed = 0;  //실험 실패면 플래그 변수 0 
    			}
    			break;

		
		
		
		case SMMNODE_TYPE_FESTIVAL:
			printf("%s paticipates to Snow Festival! press any key to pick a festival card:", cur_player[player].name);
			scanf(" %c", response);
			
			srand(time(NULL));
        	int randomfest = rand() % festival_nr;     //랜덤 함수를 사용하기 위해 ramdomfest라는 새로운 변수를 선언했다.  
			void *festObj = smmdb_getData(LISTNO_FESTCARD, randomfest);      //LISTNO_FESTCARD로 txt를 불러왔다.
        	
			printf("MISSION : %s\n", smmObj_getNodeName(festObj));  //랜덤한 미션이 나오도록 코딩했다.  
			printf("(Press any key when mission is ended.)");
			scanf(" %c", response);    //문자를 입력받으면 루프가 끝나도록 코딩했다.  
			
			break;
			
		case SMMNODE_TYPE_HOME:
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);  //집에 도착하면 에너지가 찬다.  
			printf("returned to HOME! energy charged by %i (total : %i)", smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
			
			break;
		
		case SMMNODE_TYPE_LABORATORY:
    		if (cur_player[player].position == SMMNODE_TYPE_GOTOLAB) //실험 조건을 만족하면 
			{
        		printf("OMG! This is experiment time!! Player %s goes to the lab.\n", cur_player[player].name);
        		cur_player[player].position = SMMNODE_TYPE_LABORATORY; //실험실로 이동한다.  
			}
			else 
			{
        		printf("This is not experiment time. You can go through this lab.\n");
    		}
    		break;
         
         
        default: 
            break;
    }
}

void stayOnFailedExperimentNode(int player, int nodeIndex)  //실험에 실패했을 때 플레이어를 그 노드에 머물게 하기 위한 함수 
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, nodeIndex);
    cur_player[player].experimentFailed = 1;
}

void goForward(int player, int step)
{
    int gameEnd = 0;  // 게임이 끝나는지를 구분하기 위한 플래그 변수 
    int totalNodes = 16;  // 총 노드 수
    int i;
    
    stayOnFailedExperimentNode(player, cur_player[player].position);

    int *positions = (int *)malloc((totalNodes + 1) * sizeof(int));  // 배열을 선언하여 플레이어의 위치를 저장

    while (!gameEnd)  // while문 이용해 반복
    {
        void *boardPtr;
        cur_player[player].position += step;

        if (cur_player[player].position >= totalNodes)  // 플레이어의 위치가 총 노드를 넘어서면 처음 노드로 돌아가게 한다.
        {
            cur_player[player].position %= totalNodes;
        }

        for (i = cur_player[player].position - step; i <= cur_player[player].position; i++)
        {
            if (i >= 0)
            {
                positions[i] = i;  // 위치를 저장
                boardPtr = smmdb_getData(LISTNO_NODE, positions[i]);
                printf("Jump to %s\n", smmObj_getNodeName(boardPtr));
            }
        }

        printf("%s go to node %i (name: %s)\n",
               cur_player[player].name, cur_player[player].position,
               smmObj_getNodeName(boardPtr));
               
        if (cur_player[player].experimentFailed == 0)
        {
            stayOnFailedExperimentNode(player, cur_player[player].position);
        }

        if (isGraduated())  // 플레이어가 졸업하면 while문을 빠져나온다.
        {
            gameEnd = 1;
            break;
        }
    }
    free(positions);  // 배열 해제 
}



int isGraduated(void)
{
    int i;

    for (i = 0; i < player_nr; i++)
    {
        // 졸업 조건 확인: 누적학점이 일정 이상이고 집에 도착한 경우
        if (cur_player[i].flag_graduate != 0 && cur_player[i].accumCredit >= GRADUATE_CREDIT && cur_player[i].position >= 0)
        {
            // 졸업한 플레이어의 인덱스를 반환한다. 
            return i;
        }
    }

    // 졸업한 플레이어가 없으면 -1을 반환한다. 
    return -1;
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
		smmdb_addTail(LISTNO_FOODCARD, foodObj);  //위와 같이 코드 작성. LISTNO_NODE -> LISTNO_FOODCARD
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
    while (isGraduated()==-1) //is anybody graduated?
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
        
        if(isGraduated()!=-1)   //게임 종료 조건 
		{
        	printf("GAME OVER! Player %s has graduated!\n", cur_player[isGraduated()].name);
        	printf("Courses taken:\n");  //이때까지 플레이어가 수강한 강의와 학점을 출력한다.  
        	printGrades(isGraduated());
		}
		
        //4-5. next turn
        turn = (turn +1)%player_nr;
        
	}
	free(cur_player);
    return 0;
}
