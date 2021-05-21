#include "CarDealershipManager.h"
#include "exceptions.h"

#define SAIL_POINTS 10

using namespace wet1;


/*CarModel application*/

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
 best_seller_model(nullptr),models(nullptr), zero_score_modelIds(nullptr)
{
    models = new CarModel* [models_num];
    for (int i = 0; i < models_num; i++)
    {
        models[i] = new CarModel(typeId, i);
    }
    zero_score_modelIds = new AvlTree<CarModel*, CompModelNum>(models, numOfModels-1, 0);
}

CarType::CarType(int type) : typeId(type), models_num(0),
 best_seller_model(nullptr),models(nullptr), zero_score_modelIds(nullptr) {}

/*dtor*/
CarType::~CarType()
{
    if(models)
    {
        for (int i = 0; i < models_num; i++)
        {
            delete models[i];
        }
        delete[] models;
        delete zero_score_modelIds;
    }
}

/**
 * returns a model with modelNum (if exist) 
 **/
CarModel* CarType::getModelByNum(int modelNum)
{
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
CarModel* CarType::getBestSeller()
{
    return best_seller_model;
}

/**
 * sets the best seller model num
*/
void CarType::setBestSeller(CarModel* new_best_seller)
{
    best_seller_model = new_best_seller;
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

void CarType::insertZeroScoreModels(int& amount, int& index, int* types, int* model_nums)
{
    if(amount > 0)
    {
        efficiantInorder(zero_score_modelIds->getYoungestNode(), amount, index, types, model_nums);
    }
}

void CarType::efficiantInorder(AvlTreeNode<CarModel*>* base,
             int& amount, int& index, int* types, int* models)
{
    if(amount <= 0)
        return;
    if(base != nullptr)
    {
        --amount;
        types[index] = base->get_data()->getType();
        models[index] = base->get_data()->getModelNum();
        index++;
        if(amount > 0)
        {
            inOrder(base->get_right(), amount, index, types, models);
        }
        if(amount > 0)
        {
            efficiantInorder(base->get_parent(), amount, index, types, models);
        }
    }
}

void CarType::inOrder(AvlTreeNode<CarModel*>* root,
             int& amount, int& index, int* types, int* models)
{
    if(!root)
        return;
    if(amount > 0)
        inOrder(root->get_left(),amount, index, types, models);
    if(amount > 0)
    {
        --amount;
        types[index] = root->get_data()->getType();
        models[index] = root->get_data()->getModelNum();
        index++;
    }
    if(amount > 0)
        inOrder(root->get_right(),amount, index, types, models);
}

/*************************************************/

bool CompModelNum::operator()(CarModel* const model1 , CarModel* const model2)
{
    return model1->getModelNum() < model2->getModelNum();
}

bool CompModelSailes::operator()(CarModel* const model1 , CarModel* const model2)
{
    if(model1->getSails() == model2->getSails())
    {
        if(model1->getType() == model2->getType())
            return model1->getModelNum() > model2->getModelNum();
        return model1->getType() > model2->getType();
    }
    return model1->getSails() < model2->getSails();
}

bool CompModelScore::operator()(CarModel* const model1 , CarModel* const model2)
{
    if(model1->getScore() == model2->getScore())
    {
        if(model1->getType() == model2->getType())
            return model1->getModelNum() < model2->getModelNum();
        return model1->getType() < model2->getType();
    }
    return model1->getScore() < model2->getScore();
}

bool CompTypeId::operator()(CarType* const type1 , CarType* const type2)
{
    return type1->getId() < type2->getId();
}

/*************CarDealershipManager application*********************************************************/

/*ctor*/
CarDealershipManager::CarDealershipManager() : carTypes(), modelSales(),
 PosModelScores(), NegModelScores(), types_num(0), num_of_models(0)
 {}

 CarDealershipManager::~CarDealershipManager()
 {
    deleteCarTypes(carTypes.getRoot());
 }

 void CarDealershipManager::deleteCarTypes(AvlTreeNode<CarType*>* root)
{
    if(!root)
        return;
    delete root->get_data();
    deleteCarTypes(root->get_left());
    deleteCarTypes(root->get_right());
}

StatusType CarDealershipManager::AddCarType(int typeId, int numOfModels)
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
    try {
        carTypes.find(car_type);
    }
    catch(NotFound()){
        carTypes.addElement(car_type);
        car_type->setBestSeller(0); //set best seller of this type as model 0
        types_num++;
        num_of_models += numOfModels;
        return SUCCESS;
    }
    //already exist
    delete car_type;
    return FAILURE;
}

StatusType CarDealershipManager::RemoveCarType (int typeId)
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
    CarType* car_type = nullptr;
    try{
        car_type = carTypes.find(tmp);
    }
    catch(NotFound()){
        delete tmp;
        return FAILURE;
    }
    delete tmp;
    /*delete this type models of all trees O(mlog(M))*/
    CarModel* model = nullptr;
    for (int i = 0; i < car_type->getNumOfModels(); i++)
    {
        model = car_type->getModelByNum(i);
        modelSales.deleteElement(model);
        PosModelScores.deleteElement(model);
        NegModelScores.deleteElement(model);
    }
    num_of_models -= car_type->getNumOfModels();
    delete car_type;
    types_num--;
    return SUCCESS;
}

StatusType CarDealershipManager::SellCar (int typeId, int modelId)
{
    if(typeId <=0 || modelId < 0)
    {
        return INVALID_INPUT;
    }
    CarType* tmp = nullptr;
    try{
        tmp = new CarType(typeId);
    }
    catch(std::bad_alloc()){
        return ALLOCATION_ERROR;
    }
    CarType* car_type = nullptr;
    try{
        car_type = carTypes.find(tmp);
    }
    catch(NotFound()){
        delete tmp;
        return FAILURE;
    }
    delete tmp;
    CarModel* model = car_type->getModelByNum(modelId);
    modelSales.deleteElement(model);
    NegModelScores.deleteElement(model);
    (*model)++; //add to model sales
    //update this type best seller
    CarModel* type_best_seller = car_type->getBestSeller();
    if(type_best_seller->getSails() < model->getSails())
        car_type->setBestSeller(model);
    modelSales.addElement(model);
    if(model->getScore() > 0)
    {
        car_type->removeFromZeroTree(model);
        PosModelScores.addElement(model);
    }
    else if(model->getScore() < 0)
    {
        NegModelScores.addElement(model);
    }
    else //model new score is 0
    {
        car_type->addToZeroTree(model);
    }
    return SUCCESS;
}

StatusType CarDealershipManager::MakeComplaint (int typeId, int modelId, int t)
{
    if(typeId <=0 || modelId < 0 || t <= 0)
    {
        return INVALID_INPUT;
    }
    CarType* tmp = nullptr;
    try{
        tmp = new CarType(typeId);
    }
    catch(std::bad_alloc()){
        return ALLOCATION_ERROR;
    }
    CarType* car_type = nullptr;
    try{
        car_type = carTypes.find(tmp);
    }
    catch(NotFound()){
        delete tmp;
        return FAILURE;
    }
    delete tmp;
    CarModel* model = car_type->getModelByNum(modelId);
    PosModelScores.deleteElement(model);
    model->complain(t);
    if(model->getScore() > 0)
    {
        PosModelScores.addElement(model);
    }
    else if(model->getScore() < 0)
    {
        car_type->removeFromZeroTree(model);
        NegModelScores.addElement(model);
    }
    else //model new score is 0
    {
        car_type->addToZeroTree(model);
    }
    return SUCCESS;
}

 StatusType CarDealershipManager::GetBestSellerModelByType (int typeId, int* modelId)
 {
     if(typeId < 0)
    {
        return INVALID_INPUT;
    }
    if(types_num == 0)/*system is empty*/
    {
        return FAILURE;
    }
    if(typeId == 0)
    {
        CarModel* best_seller = nullptr;
        try{
            best_seller = modelSales.getOldestData();
        }
        //all models have zero sales
        catch(EmptyTree()){
            *modelId = 0;
            return SUCCESS;
        }
        *modelId = best_seller->getModelNum();
        return SUCCESS;
    }
    else
    {
        CarType* tmp = nullptr;
        try{
            tmp = new CarType(typeId);
        }
        catch(std::bad_alloc()){
            delete tmp;
            return ALLOCATION_ERROR;
        }
        CarType* car_type = nullptr;
        try{
            car_type = carTypes.find(tmp);
        }
        catch(NotFound()){
            delete tmp;
            return FAILURE;
        }
        delete tmp;
        *modelId = car_type->getBestSeller()->getModelNum();
        return SUCCESS;
    }
 }

 StatusType CarDealershipManager::GetWorstModels (int numOfModels, int* types, int* models)
 {
    if(numOfModels <= 0)
        return INVALID_INPUT;
    if(numOfModels > num_of_models)
        return FAILURE;
    int index = 0;
    int amount = numOfModels;
    efficiantInorder(NegModelScores.getYoungestNode(),amount, index, types, models);
    if(amount > 0)
    {
        efficiantInorderZeroScores(carTypes.getYoungestNode(),amount, index, types, models);
    }
    if(amount > 0)
    {
        efficiantInorder(PosModelScores.getYoungestNode(),amount, index, types, models);
    }
    return SUCCESS;
 }

void CarDealershipManager::efficiantInorder(AvlTreeNode<CarModel*>* base,
             int& amount, int& index, int* types, int* models)
{
    if(amount <= 0)
        return;
    if(base != nullptr)
    {
        --amount;
        types[index] = base->get_data()->getType();
        models[index] = base->get_data()->getModelNum();
        index++;
        if(amount > 0)
        {
            inOrder(base->get_right(), amount, index, types, models);
        }
        if(amount > 0)
        {
            efficiantInorder(base->get_parent(), amount, index, types, models);
        }
    }
}

void CarDealershipManager::inOrder(AvlTreeNode<CarModel*>* root,
             int& amount, int& index, int* types, int* models)
{
    if(!root)
        return;
    if(amount > 0)
        inOrder(root->get_left(),amount, index, types, models);
    if(amount > 0)
    {
        --amount;
        types[index] = root->get_data()->getType();
        models[index] = root->get_data()->getModelNum();
        index++;
    }
    if(amount > 0)
        inOrder(root->get_right(),amount, index, types, models);
}

void CarDealershipManager::efficiantInorderZeroScores(AvlTreeNode<CarType*>* base,
             int& amount, int& index, int* types, int* models)
{
    if(amount <= 0)
        return;
    if(base != nullptr)
    {
        base->get_data()->insertZeroScoreModels(amount, index, types, models);
        if(amount > 0)
        {
            inOrderZeroScores(base->get_right(), amount, index, types, models);
        }
        if(amount > 0)
        {
            inOrderZeroScores(base->get_parent(), amount, index, types, models);
        }
    }
}

void CarDealershipManager::inOrderZeroScores(AvlTreeNode<CarType*>* root,
             int& amount, int& index, int* types, int* models)
{
    if(!root)
        return;
    if(amount > 0)
        inOrderZeroScores(root->get_left(),amount, index, types, models);
    if(amount > 0)
    {
        root->get_data()->insertZeroScoreModels(amount, index, types, models);
    }
    if(amount > 0)
        inOrderZeroScores(root->get_right(),amount, index, types, models);
}

/*********************************************************************/