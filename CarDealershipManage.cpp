#include <iostream>
#include <string>
#include "CarDealershipManage.h"
#include "exceptions.h"

#define SAIL_POINTS 10

using std::ostream;
using std::string;
using namespace wet1;

/*CarModel application*/

CarModel::CarModel() : model_type(0), model_num(0), sails(0), score(0) {}

CarModel::CarModel(int type, int model) : model_type(type), model_num(model), sails(0), score(0) {}

int CarModel::getModelNum()
{
    return model_num;
}

int CarModel::getType()
{
    return model_type;
}

int CarModel::getSails()
{
    return sails;
}

int CarModel::getScore()
{
    return score;
}

// void CarModel::setType(int type)
// {
//     model_type = type;
// }

// void CarModel::setModelNum(int model)
// {
//     model_num = model;
// }

void CarModel::operator++(int)
{
    sails++;
    score += SAIL_POINTS;
}

void CarModel::complain(int t)
{
    score -= (100 / t);
}

/**************************************************/
/*CarType application*/

/*ctor*/
CarType::CarType(int type, int numOfModels) : typeId(type), models_num(numOfModels),
 best_seller_model_num(0),models(nullptr), zero_score_modelIds(nullptr)
{
    models = new CarModel* [models_num];
    for (int i = 0; i < models_num; i++)
    {
        models[i] = new CarModel(typeId, i);
    }
    zero_score_modelIds = new AvlTree<CarModel*, CompModelNum, PrintModel>(models, 0, numOfModels-1);
}

CarType::CarType(int type) : typeId(type), models_num(0),
 best_seller_model_num(0),models(nullptr), zero_score_modelIds(nullptr) {}

/*dtor*/
CarType::~CarType()
{
    for (int i = 0; i < models_num; i++)
    {
        delete models[i];
    }
    delete[] models;
    delete zero_score_modelIds;
}

/**
 * returns a model with modelNum (if exist) 
 **/
CarModel* CarType::getModelByNum(int modelNum)
{
    if(modelNum >= models_num)
    {
        throw NoSuchAModel();
    }
    return models[models_num];
}

/**
 * returns the type id
 **/
int CarType::getId()
{
    return typeId;
}

int CarType::getNumOfModels()
{
    return models_num;
}

/**
 * returns the best seller model if exist
*/
int CarType::getBestSeller()
{
    return best_seller_model_num;
}

/**
 * sets the best seller model num
*/
void CarType::setBestSeller(int new_best_seller_num)
{
    best_seller_model_num = new_best_seller_num;
}

/*adds model to zero tree*/
void CarType::addToZeroTree(CarModel* model)
{
    zero_score_modelIds->addElement(model);
}

/*removes model to zero tree*/
void CarType::removeFromZeroTree(CarModel* model)
{
    zero_score_modelIds->deleteElement(model);
}

/**
 * prints amount of model numbers with zero score
*/
void CarType::printZeroScoreModels(int& amount)
{
    if(amount <= 0)
    {
        return;
    }
    zero_score_modelIds->efficiantInorderPrint(amount);
}
/*************************************************/

bool CompModelNum::operator()(CarModel* const model1 , CarModel* const model2)
{
    return (model1->getModelNum()) < model2->getModelNum();
}

bool CompModelSales::operator()(CarModel* const model1 , CarModel* const model2)
{
    return (model1->getSails()) < model2->getSails();
}

bool CompModelScore::operator()(CarModel* const model1 , CarModel* const model2)
{
    return (model1->getScore()) < model2->getScore();
}

bool CompTypeId::operator()(CarType* const type1 , CarType* const type2)
{
    return (type1->getId()) < type2->getId();
}

string PrintModel::operator() (CarModel* const model1) const
{
    return std::to_string(model1->getType()) + "	|	" + std::to_string(model1->getType());
}
/*CarDealershipManage application*/

/*ctor*/
CarDealershipManage::CarDealershipManage() : carTypes(), modelSales(),
 PosModelScores(), NegModelScores(), best_seller(nullptr)
{}

StatusType CarDealershipManage::AddCarType(int typeId, int numOfModels)
{
    if(typeId <=0 || numOfModels <= 0)
    {
        return INVALID_INPUT;
    }
    CarType* car_type;
    try{
        car_type = new CarType(typeId, numOfModels);
    }
    catch(std::bad_alloc()){
        delete car_type;
        return ALLOCATION_ERROR;
    }
    if (!carTypes.find(car_type))
    {
        carTypes.addElement(car_type);
    }
    else //already exist
    {
        delete car_type;
        return FAILURE;
    }
    if(!best_seller)/*the system was empty before insert*/
    {
        best_seller = car_type->getModelByNum(0);
    }
    return SUCCESS;
}

StatusType CarDealershipManage::RemoveCarType (int typeId)
{
    CarType* tmp = nullptr;
    if(typeId <= 0)
        return INVALID_INPUT;
    try{
        tmp = new CarType(typeId);
    }
    catch(std::bad_alloc()){
        return ALLOCATION_ERROR;
    }
    AvlTreeNode<CarType*>* node = carTypes.find(tmp);
    if(!node)
        return FAILURE;
    CarType* car_type = node->get_data();
    /*delete this type model of all trees O(mlog(M))*/
    for (int i = 0; i < car_type->getNumOfModels(); i++)
    {
        modelSales.deleteElement(car_type->getModelByNum(i));
    }
    
}


/*********************************************************************/