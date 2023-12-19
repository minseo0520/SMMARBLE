//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
       "����",
       "�Ĵ�",
       "�����",
       "��",
       "����Ƿ��̵�",
       "��������",
       "�����ð�"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}




//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;

//3. ���� �Լ� ���� 
//object generation
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjType_e objgrade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;     //Type -> objType�� ����. �׷��� node�� Type�� �����۵� 
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = objgrade;
    
    return ptr;
}


//3. ���� �Լ� ���� 
//member retrieving
char* smmObj_getNodeName(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->name;
}



//3. ���� �Լ� ���� 
int smmObj_getNodeType(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->type;
}

int smmObj_getNodeCredit(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->credit;
}

int smmObj_getNodeEnergy(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->energy;
}


//element to string
char* smmObj_getNodeNameType(smmNode_e type)   //�ش���Ÿ���� �̸��� �����ͼ� ��ȯ, �̸��� �ߺ��Ǿ getNodeNameByType�� ���� 
{
    return smmObj_getTypeName(type);
}

char* smmObj_getNodeGrade(smmObjGrade_e grade)
{
	switch (grade) {             //grade�� ����ϱ� ���� ���ο� switch �Լ��� ®��.  
        case smmObjGrade_Ap:
            return "A+";
        case smmObjGrade_A0:
            return "A0";
        case smmObjGrade_Am:
            return "A-";
        case smmObjGrade_Bp:
            return "B+";
        case smmObjGrade_B0:
            return "B0";
        case smmObjGrade_Bm:
            return "B-";
        case smmObjGrade_Cp:
            return "C+";
        case smmObjGrade_C0:
            return "C0";
        case smmObjGrade_Cm:
            return "C-";
        default:
            return "Unknown Grade";
    }
}

