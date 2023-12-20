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
    ptr->objType = objType;       //Type -> objType�� ����. �׷��� node�� Type�� �����۵� 
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

GradeInfo smmObj_getNodeGrade(smmObjGrade_e grade) {
    GradeInfo gradeInfo;

    switch (grade) {
        case smmObjGrade_Ap:
            gradeInfo.gradeStr = "A+";
            gradeInfo.gradeValue = 4.3;
            break;
        case smmObjGrade_A0:
            gradeInfo.gradeStr = "A0";
            gradeInfo.gradeValue = 4.0;
            break;
        case smmObjGrade_Am:
            gradeInfo.gradeStr = "A-";
            gradeInfo.gradeValue = 3.7;
            break;
        case smmObjGrade_Bp:
            gradeInfo.gradeStr = "B+";
            gradeInfo.gradeValue = 3.3;
            break;
        case smmObjGrade_B0:
            gradeInfo.gradeStr = "B0";
            gradeInfo.gradeValue = 3.0;
            break;
        case smmObjGrade_Bm:
            gradeInfo.gradeStr = "B-";
            gradeInfo.gradeValue = 2.7;
            break;
        case smmObjGrade_Cp:
            gradeInfo.gradeStr = "C+";
            gradeInfo.gradeValue = 2.3;
            break;
        case smmObjGrade_C0:
            gradeInfo.gradeStr = "C0";
            gradeInfo.gradeValue = 2.0;
            break;
        case smmObjGrade_Cm:
            gradeInfo.gradeStr = "C-";
            gradeInfo.gradeValue = 1.7;
            break;
        default:
            gradeInfo.gradeStr = "Unknown Grade";
            gradeInfo.gradeValue = 0.0;
    }

    return gradeInfo;
}


