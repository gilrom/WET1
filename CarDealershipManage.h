#ifndef CAR_DEALER
#define CAR_DEALER

#include "AvlTree.h"
#include "library.h"
#include <string>

typedef enum {
    CAR_TPYES,
    POS_MODEL_SCORES,
    NEG_MODEL_SCORES,
} TreeType;

namespace wet1
{
    class CarModel
    {
        private:
            int model_type, model_num, sails, score;
        public:
            CarModel();
            CarModel(int type, int model);
            int getType();
            int getModelNum();
            int getScore();
            int getSails();
            // void setType(int type);
            // void setModelNum(int model);
            /*for sale*/
            void operator++(int);
            /*for complaint*/
            void complain (int t);
    };

    /**
     * object function to compare models by score
     * compares by TypeId is score is even
     */
    class CompModelNum
    {
        public:
            bool operator() (CarModel* const model1 , CarModel* const model2);
    };

    /**
     * object function to compare models by score
     * compares by TypeId is score is even
     */
    class CompModelSales
    {
        public:
            bool operator() (CarModel* const model1 , CarModel* const model2);
    };
    
    /**
     * object function to compare models by score
     * compares by TypeId is score is even
     */
    class CompModelScore
    {
        public:
            bool operator() (CarModel* const model1 , CarModel* const model2);
    };

    class CarType
    {
        int typeId, models_num;
        CarModel* best_seller_model;
        CarModel** models;
        /*zeros tree*/
        AvlTree<CarModel*, CompModelNum>* zero_score_modelIds;
        public:
            CarType(int id, int numOfModels);
            /*ctor for dummy CarType that will only hold typeID, used for searching*/
            explicit CarType(int id);
            ~CarType();
            CarModel* getModelByNum(int modelNum);
            int getId();
            int getNumOfModels();
            CarModel* getBestSeller();
            void setBestSeller(CarModel* new_best_seller);
            void addToZeroTree(CarModel* model);
            void removeFromZeroTree(CarModel* model);
            void printZeroScoreModels(int& amount);
            void efficiantInorder(TreeType tree_type, int& amount, int& index, int* types, int* models);
    };

    /**
     * object function to compare models by score
     * compares by TypeId if score is even
     */
    class CompTypeId
    {
        public:
            bool operator() (CarType* const type1 , CarType* const type2);
    };

    class CarDealershipManage
    {
        private:
            AvlTree<CarType*, CompTypeId> carTypes;
            AvlTree<CarModel*, CompModelSales> modelSales;
            AvlTree<CarModel*, CompModelScore> PosModelScores;
            AvlTree<CarModel*, CompModelScore> NegModelScores;
            int types_num, num_of_models;
            void efficiantInorder(AvlTreeNode<CarModel*>* base,
             int& amount, int& index, int* types, int* models);
            void inOrder(AvlTreeNode<CarModel*>* root,
             int& amount, int& index, int* types, int* models);
        public:
            CarDealershipManage();
            StatusType AddCarType (int typeId, int numOfModels);
            StatusType RemoveCarType (int typeId);
            StatusType SellCar (int typeId, int modelId);
            StatusType MakeComplaint (int typeId, int modelId, int t);
            StatusType GetBestSellerModelByType (int typeId, int* modelId);
            StatusType GetWorstModels (int numOfModels, int* types, int* models);
    };

}
#endif