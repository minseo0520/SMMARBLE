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
	int accumCredit;  //�������� 
	int flag_graduate; //������ �� �� �ִ��� ������
	int experimentFailed; //���� ���� ���� ���� 
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

     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)  //�÷��̾��� ���� ��� �ݺ� 
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         smmObjGrade_e grade = ((smmObject_t*)gradePtr)->grade;   //���� ��ü�� ����� �����´�. 
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


void clearInputBuffer()   //�Լ��� ȣ���Ͽ� �Է¹��۸� ���� ���� �Լ� 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  //getchar�� �� ���� �� ���ھ� �Է��� �о�帰��. \n�̳� ������ ���� ���� ������ �Է� �Һ� 
}


int rolldie(int player)
{
    char c;
    printf("This is %s's turn:::::Press any key to roll a die (press g to see grade): ", cur_player[player].name);
    
    clearInputBuffer();  //�Է� ���� ���� ���� �Լ� ȣ�� 
    
    c = getchar();
    
    if (c == 'g')
        printGrades(player);
        
    return (rand() % MAX_DIE + 1);
}



float calcAverageGrade(int player) {
    int i;
    float totalGrade = 0.0;  //���� ����� ����ϱ� ���� �� �����̶�� ���ο� ������ �����ߴ�.  

    int numCourses = smmdb_len(LISTNO_OFFSET_GRADE + player);  // ������ ���� ����� ���ο� ������ �����ߴ�.  

    for (i = 0; i < numCourses; i++)   //������ �����ִ� for���̴�.
	{        
        smmObject_t *gradePtr = (smmObject_t *)smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        totalGrade += gradePtr->grade;
    }

    if (numCourses > 0)    //����� ����ϴ� �ڵ��̴�.
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

    int validInput = 0;  //�Է��� �ùٸ��� Ȯ���ϱ� ���� �÷��� ����  
    
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
        		if (strcmp(response, "join") == 0) //�÷��̾ join�� �Է��ϸ� 
        		{
        			if (0 <= cur_player[player].energy && cur_player[player].energy < smmObj_getNodeEnergy(boardPtr))
            		{
            			printf("%s is too hungry to take the lecture %s. Not enough energy!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
                		// �������� �����ϸ� ���� ������ �Ѿ���� ����
                		validInput = 1;
					}
					
					else
            		{
                		// ������ �̹� �����ߴ��� Ȯ��
                		int alreadyTaken = 0;  //�̹� �����ߴ����� ���� �÷��� ���� 
                		for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
                		{
                    		void *takenGradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);  //�÷��̾ ������ ���ǿ� ���� 
                    		if(strcmp(smmObj_getNodeName(takenGradePtr), smmObj_getNodeName(boardPtr)) == 0)
                    		{
                        		alreadyTaken = 1;
                        		break;
                    		}
                		}

                		if (alreadyTaken)
                		{
                    		printf("%s has already taken %s!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
                    		validInput = 1; //���� ������ �ѱ�� 
                		}
                		
                	
                	
					else
           	 		{
           	 			smmObjGrade_e getRandomGrade()		//�������� ����� ��ȯ�ϴ� �Լ��� �����ߴ�. 
						{ 
    						srand((unsigned int)time(NULL));    // ���� �߻��� �ʱ�ȭ

    						int randomIndex = rand() % 9;       // ���� ���� (0���� 8����)

    						return (smmObjGrade_e)randomIndex;  // ������ �ش��ϴ� ��� ��ȯ
    					}

            			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr); 

                		smmObjGrade_e randomGrade = getRandomGrade();   //�������� ������ ���� 
                		grade = ((smmObject_t*)gradePtr)->grade; //���� ���� ��ü�� ��� 

                		printf("%s successfully takes the lecture %s with grade %s(average : %f, remained energy : %i)\n", cur_player[player].name, 
						smmObj_getNodeName(boardPtr), smmObj_getNodeGrade(randomGrade), calcAverageGrade(player), cur_player[player].energy);

                		// ���� ����
                		gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, randomGrade);  //������ ������ �̸��� credit, ������ �����Ѵ�.  
                		smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

                		validInput = 1;  // �÷��� ������ 1�̸� do-while ������ �����Ѵ�.
				    	}
					}
				
				}
				
				else if (strcmp(response, "drop") == 0)  //�÷��̾ drop�� �Է��ϸ� 
				{
					printf("Player %s drops the lecture %s!\n", cur_player[player].name, smmObj_getNodeName(boardPtr));
            		validInput = 1;
				}
				
        		else
        		{
        			printf("Invalid input! Please enter 'join' or 'drop'.\n");  //�߸��� ���ڸ� �Է������� 
				}
    		}while (!validInput);
    		break;  
           
		   
		    
        case SMMNODE_TYPE_FOODCHANCE:
			printf("%s gets a food chance! press any key to pick a food card:", cur_player[player].name);
        	scanf(" %c", response);
        
        	srand(time(NULL));
        	int randomfood = rand() % food_nr;     //������ ���� ī�带 �̱� ���ؼ� ramdomIndex��� ������ ���� �������. �����Լ��� ����ߴ�.  
			void *foodObj = smmdb_getData(LISTNO_FOODCARD, randomfood);    //LISTNO_FOODCARD�� txt�� �ҷ��Դ�.  
        
        	int foodEnergy = smmObj_getNodeEnergy(foodObj);   //�������� ������Ʈ �ϱ� ���� foodEnergy��� ������ �����ߴ�.  
    		cur_player[player].energy += foodEnergy;   //foodObj���� �������� �����ͼ� ���� �÷��̾��� �������� ������Ʈ�ϴ� �ڵ�
    		
    		
    		printf("%s picks %s and charges %i (remained energy: %i)\n", cur_player[player].name, smmObj_getNodeName(foodObj), foodEnergy, cur_player[player].energy);
        
        	break;
        
        
        
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("Let's eat in %s and charge %i energies (remained energy : %i)\n", smmObj_getNodeName(boardPtr), 
			smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);  //�Ĵ翡 �����ϸ� �������� ����.  
        	break;
		 
        
        
        case SMMNODE_TYPE_GOTOLAB:
    			cur_player[player].experimentThreshold = rand() % MAX_DIE + 1;  //�����ϰ� �ֻ����� ���� Ż���������� �̴´�.  
    
    			printf("Experiment time! Let's see if you satisfy professor (threshold : %i)\n", cur_player[player].experimentThreshold);

    			int experimentResult = rolldie(turn);  //��������� ������ ���� �����ߴ�.  

    			if (experimentResult >= cur_player[player].experimentThreshold)   //�������� Ż�������� �������� ���� �����̴�.  
				{
        			printf("Experiment result : %i, success! %s can exit this lab!\n", experimentResult, cur_player[player].name);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			cur_player[player].experimentFailed = 1;   //���輺���̸� �÷��� ���� 1 
    			} 
				
				else 
				{
        			printf("Experiment result : %i, fail T_T. %s needs more experiment....\n", experimentResult, cur_player[player].name);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			cur_player[player].experimentFailed = 0;  //���� ���и� �÷��� ���� 0 
    			}
    			break;

		
		
		
		case SMMNODE_TYPE_FESTIVAL:
			printf("%s paticipates to Snow Festival! press any key to pick a festival card:", cur_player[player].name);
			scanf(" %c", response);
			
			srand(time(NULL));
        	int randomfest = rand() % festival_nr;     //���� �Լ��� ����ϱ� ���� ramdomfest��� ���ο� ������ �����ߴ�.  
			void *festObj = smmdb_getData(LISTNO_FESTCARD, randomfest);      //LISTNO_FESTCARD�� txt�� �ҷ��Դ�.
        	
			printf("MISSION : %s\n", smmObj_getNodeName(festObj));  //������ �̼��� �������� �ڵ��ߴ�.  
			printf("(Press any key when mission is ended.)");
			scanf(" %c", response);    //���ڸ� �Է¹����� ������ �������� �ڵ��ߴ�.  
			
			break;
			
		case SMMNODE_TYPE_HOME:
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);  //���� �����ϸ� �������� ����.  
			printf("returned to HOME! energy charged by %i (total : %i)", smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
			
			break;
		
		case SMMNODE_TYPE_LABORATORY:
    		if (cur_player[player].position == SMMNODE_TYPE_GOTOLAB) //���� ������ �����ϸ� 
			{
        		printf("OMG! This is experiment time!! Player %s goes to the lab.\n", cur_player[player].name);
        		cur_player[player].position = SMMNODE_TYPE_LABORATORY; //����Ƿ� �̵��Ѵ�.  
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

void stayOnFailedExperimentNode(int player, int nodeIndex)  //���迡 �������� �� �÷��̾ �� ��忡 �ӹ��� �ϱ� ���� �Լ� 
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, nodeIndex);
    cur_player[player].experimentFailed = 1;
}

void goForward(int player, int step)
{
    int gameEnd = 0;  // ������ ���������� �����ϱ� ���� �÷��� ���� 
    int totalNodes = 16;  // �� ��� ��
    int i;
    
    stayOnFailedExperimentNode(player, cur_player[player].position);

    int *positions = (int *)malloc((totalNodes + 1) * sizeof(int));  // �迭�� �����Ͽ� �÷��̾��� ��ġ�� ����

    while (!gameEnd)  // while�� �̿��� �ݺ�
    {
        void *boardPtr;
        cur_player[player].position += step;

        if (cur_player[player].position >= totalNodes)  // �÷��̾��� ��ġ�� �� ��带 �Ѿ�� ó�� ���� ���ư��� �Ѵ�.
        {
            cur_player[player].position %= totalNodes;
        }

        for (i = cur_player[player].position - step; i <= cur_player[player].position; i++)
        {
            if (i >= 0)
            {
                positions[i] = i;  // ��ġ�� ����
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

        if (isGraduated())  // �÷��̾ �����ϸ� while���� �������´�.
        {
            gameEnd = 1;
            break;
        }
    }
    free(positions);  // �迭 ���� 
}



int isGraduated(void)
{
    int i;

    for (i = 0; i < player_nr; i++)
    {
        // ���� ���� Ȯ��: ���������� ���� �̻��̰� ���� ������ ���
        if (cur_player[i].flag_graduate != 0 && cur_player[i].accumCredit >= GRADUATE_CREDIT && cur_player[i].position >= 0)
        {
            // ������ �÷��̾��� �ε����� ��ȯ�Ѵ�. 
            return i;
        }
    }

    // ������ �÷��̾ ������ -1�� ��ȯ�Ѵ�. 
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
	    void *foodObj = smmObj_genObject(name, SMMNODE_TYPE_FOODCHANCE, 0, 0, energy, 0);  //foodObj��� ���ο� �ڵ� �ۼ� 
		smmdb_addTail(LISTNO_FOODCARD, foodObj);  //���� ���� �ڵ� �ۼ�. LISTNO_NODE -> LISTNO_FOODCARD
		food_nr++; 
    }
    fclose(fp);
    
    for(i=0; i<food_nr; i++) //food_nr���� �ݺ� 
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
        void *festObj = smmObj_genObject(name, 0, 0, 0, 0, 0);  //festObj��� ���ο� �ڵ� �ۼ� 
		smmdb_addTail(LISTNO_FESTCARD, festObj);  //���� ���� �ڵ� �ۼ�. LISTNO_NODE-> LISTNO_FESTCARD
		festival_nr++; 
    }
    fclose(fp);
    
    for(i=0; i<festival_nr; i++)  //festival_nr���� �ݺ� 
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
        
        if(isGraduated()!=-1)   //���� ���� ���� 
		{
        	printf("GAME OVER! Player %s has graduated!\n", cur_player[isGraduated()].name);
        	printf("Courses taken:\n");  //�̶����� �÷��̾ ������ ���ǿ� ������ ����Ѵ�.  
        	printGrades(isGraduated());
		}
		
        //4-5. next turn
        turn = (turn +1)%player_nr;
        
	}
	free(cur_player);
    return 0;
}
