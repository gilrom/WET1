/***************************************************************************/
/*                                                                         */
/* 234218 Data DSs 1, Spring 2021                                          */
/* Homework : Wet 1                                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* File Name : main1.cpp                                                   */
/*                                                                         */
/* Holds the "int main()" function and the parser of the shell's           */
/* command line.                                                           */
/***************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The command's strings */
typedef enum {
    NONE_CMD = -2,
    COMMENT_CMD = -1,
    INIT_CMD = 0,
	ADDCARTYPE_CMD = 1,
	REMOVECARTYPE_CMD = 2,
	SELLCAR_CMD = 3,
	MAKECOMPLAINT_CMD = 4,
	GETBESTSELLERMODELBYTYPE_CMD = 5,
	GETWORSTMODELS_CMD = 6,
    QUIT_CMD = 7
} commandType;

static const int numActions = 10;
static const char *commandStr[] = {
        "Init",
        "AddCarType",
        "RemoveCarType",
        "SellCar",
		"MakeComplaint",
        "GetBestSellerModelByType",
        "GetWorstModels",
        "Quit" };

static const char* ReturnValToStr(int val) {
    switch (val) {
        case SUCCESS:
            return "SUCCESS";
        case ALLOCATION_ERROR:
            return "ALLOCATION_ERROR";
        case FAILURE:
            return "FAILURE";
        case INVALID_INPUT:
            return "INVALID_INPUT";
        default:
            return "";
    }
}

/* we assume maximum string size is not longer than 256  */
#define MAX_STRING_INPUT_SIZE (255)
#define MAX_BUFFER_SIZE       (255)

#define StrCmp(Src1,Src2) ( strncmp((Src1),(Src2),strlen(Src1)) == 0 )

typedef enum {
    error_free, error
} errorType;
static errorType parser(const char* const command);

#define ValidateRead(read_parameters,required_parameters,ErrorString,ErrorParams) \
if ( (read_parameters)!=(required_parameters) ) { printf(ErrorString, ErrorParams); return error; }

static bool isInit = false;

/***************************************************************************/
/* main                                                                    */
/***************************************************************************/

int main(int argc, const char**argv) {

    char buffer[MAX_STRING_INPUT_SIZE];

    // Reading commands
    while (fgets(buffer, MAX_STRING_INPUT_SIZE, stdin) != NULL) {
        fflush(stdout);
        if (parser(buffer) == error)
            break;
    };
    return 0;
}

/***************************************************************************/
/* Command Checker                                                         */
/***************************************************************************/

static commandType CheckCommand(const char* const command,
                                const char** const command_arg) {
    if (command == NULL || strlen(command) == 0 || StrCmp("\n", command))
        return (NONE_CMD);
    if (StrCmp("#", command)) {
        if (strlen(command) > 1)
            printf("%s", command);
        return (COMMENT_CMD);
    };
    for (int index = 0; index < numActions; index++) {
        if (StrCmp(commandStr[index], command)) {
            *command_arg = command + strlen(commandStr[index]) + 1;
            return ((commandType)index);
        };
    };
    return (NONE_CMD);
}

/***************************************************************************/
/* Commands Functions                                                      */
/***************************************************************************/

static errorType OnInit(void** DS, const char* const command);
static errorType OnAddCarType(void* DS, const char* const command);
static errorType OnRemoveCarType(void* DS, const char* const command);
static errorType OnSellCar(void* DS, const char* const command);
static errorType OnMakeComplaint(void* DS, const char* const command);
static errorType OnGetBestSellerModelByType(void* DS, const char* const command);
static errorType OnGetWorstModels(void* DS, const char* const command);
static errorType OnQuit(void** DS, const char* const command);

/***************************************************************************/
/* Parser                                                                  */
/***************************************************************************/

static errorType parser(const char* const command) {
    static void *DS = NULL; /* The general data structure */
    const char* command_args = NULL;
    errorType rtn_val = error;

    commandType command_val = CheckCommand(command, &command_args);
	
    switch (command_val) {

        case (INIT_CMD):
            rtn_val = OnInit(&DS, command_args);
            break;
        case (ADDCARTYPE_CMD):
            rtn_val = OnAddCarType(DS, command_args);
            break;
        case (REMOVECARTYPE_CMD):
            rtn_val = OnRemoveCarType(DS, command_args);
            break;
        case (SELLCAR_CMD):
            rtn_val = OnSellCar(DS, command_args);
            break;
        case (MAKECOMPLAINT_CMD):
            rtn_val = OnMakeComplaint(DS, command_args);
            break;
        case (GETBESTSELLERMODELBYTYPE_CMD):
            rtn_val = OnGetBestSellerModelByType(DS, command_args);
            break;
        case (GETWORSTMODELS_CMD):
            rtn_val = OnGetWorstModels(DS, command_args);
            break;			
        case (QUIT_CMD):
            rtn_val = OnQuit(&DS, command_args);
            break;

        case (COMMENT_CMD):
            rtn_val = error_free;
            break;
        case (NONE_CMD):
            rtn_val = error;
            break;
        default:
            assert(false);
            break;
    };
    return (rtn_val);
}

static errorType OnInit(void** DS, const char* const command) {
    if (isInit) {
        printf("init was already called.\n");
        return (error_free);
    };
    isInit = true;

    ValidateRead(0, 0, "%s failed.\n", commandStr[INIT_CMD]);
    *DS = Init();

    if (*DS == NULL) {
        printf("init failed.\n");
        return error;
    };

    printf("init done.\n");
    return error_free;
}

static errorType OnAddCarType(void* DS, const char* const command) {
    int typeID, numOfModels;
    ValidateRead(sscanf(command, "%d %d", &typeID, &numOfModels), 2, "%s failed.\n", commandStr[ADDCARTYPE_CMD]);
    StatusType res = AddCarType(DS, typeID, numOfModels);

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[ADDCARTYPE_CMD], ReturnValToStr(res));
        return error_free;
    }

    printf("%s: %s\n", commandStr[ADDCARTYPE_CMD], ReturnValToStr(res));
    return error_free;
}

static errorType OnRemoveCarType(void* DS, const char* const command) {
    int typeID;
    ValidateRead(sscanf(command, "%d", &typeID), 1, "%s failed.\n", commandStr[REMOVECARTYPE_CMD]);
	StatusType res = RemoveCarType(DS, typeID);

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[REMOVECARTYPE_CMD], ReturnValToStr(res));
        return error_free;
    }

    printf("%s: %s\n", commandStr[REMOVECARTYPE_CMD], ReturnValToStr(res));
    return error_free;
}

static errorType OnSellCar(void* DS, const char* const command) {
    int typeID, modelID;
    ValidateRead(sscanf(command, "%d %d", &typeID, &modelID), 2, "%s failed.\n", commandStr[SELLCAR_CMD]);
    StatusType res = SellCar(DS, typeID, modelID);

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[SELLCAR_CMD], ReturnValToStr(res));
        return error_free;
    }

    printf("%s: %s\n", commandStr[SELLCAR_CMD], ReturnValToStr(res));
    return error_free;
}

static errorType OnMakeComplaint(void* DS, const char* const command) {
    int typeID, modelID, time;
    ValidateRead(sscanf(command, "%d %d %d", &typeID, &modelID, &time), 3, "%s failed.\n", commandStr[MAKECOMPLAINT_CMD]);
    StatusType res = MakeComplaint(DS, typeID, modelID, time);

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[MAKECOMPLAINT_CMD], ReturnValToStr(res));
        return error_free;
    }

    printf("%s: %s\n", commandStr[MAKECOMPLAINT_CMD], ReturnValToStr(res));
    return error_free;
}

static errorType OnGetBestSellerModelByType(void* DS, const char* const command) {
	int typeID, modelID;
    ValidateRead(sscanf(command, "%d", &typeID), 1, "%s failed.\n", commandStr[GETBESTSELLERMODELBYTYPE_CMD]);
    StatusType res = GetBestSellerModelByType(DS, typeID, &modelID);

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[GETBESTSELLERMODELBYTYPE_CMD], ReturnValToStr(res));
        return error_free;
    }

    printf("%s: %d\n", commandStr[GETBESTSELLERMODELBYTYPE_CMD], modelID);
    return error_free;
}

static errorType OnGetWorstModels(void* DS, const char* const command) {
    int numOfModels;
    int *types = NULL, *models = NULL;
	StatusType res = SUCCESS;

	ValidateRead(sscanf(command, "%d", &numOfModels), 1, "%s failed.\n", commandStr[GETWORSTMODELS_CMD]);
	if (numOfModels > 0) {
		types = (int *)malloc(numOfModels * sizeof(int));
		models = (int *)malloc(numOfModels * sizeof(int));
		if (types == NULL || models == NULL) {
		res = ALLOCATION_ERROR;
		}
	}

	if (res != ALLOCATION_ERROR) {
		res = GetWorstModels(DS, numOfModels, types, models);
	}

    if (res != SUCCESS) {
        printf("%s: %s\n", commandStr[GETWORSTMODELS_CMD], ReturnValToStr(res));
		if (types != NULL) free(types);
		if (models != NULL) free(models);
        return error_free;
    }

    
    printf("--Start of worst models--\n");
	printf("CarType\t|\tModel\n");

    for (int i = 0; i < numOfModels; i++)
    {
        printf("%d\t|\t%d\n", types[i], models[i]);
    }

    printf("--End of worst models--\n");
	printf("%s: %s\n", commandStr[GETWORSTMODELS_CMD], ReturnValToStr(res));

	if (types != NULL) free(types);
	if (models != NULL) free(models);

    return error_free;
}

static errorType OnQuit(void** DS, const char* const command) {
    Quit(DS);
    if (*DS != NULL) {
        printf("quit failed.\n");
        return error;
    };

    isInit = false;
    printf("quit done.\n");
    return error_free;
}

#ifdef __cplusplus
}
#endif
