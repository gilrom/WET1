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
            CarModel(int type, int model);
            int getType();
            int getModelNum();
            int getScore();
            int getSails();
            /*for sale*/
            void operator++(int);
            /*for complaint*/
            void complain (int t);
    };

    /**
     * object function to compare models by model number
     */
    class CompModelNum
    {
        public:
            bool operator() (CarModel* const model1 , CarModel* const model2);
    };

    /**
     * object function to compare models by sailes
     * compares by TypeId if sailes are even
     * compares by modelNum if TypeIDs are even
     */
    class CompModelSailes
    {
        public:
            bool operator() (CarModel* const model1 , CarModel* const model2);
    };
    
    /**
     * object function to compare models by score
     * compares by TypeId is score is even
     * compares by modelNum if TypeIDs are even
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
        CarModel** models; //array of models
        /*zeros tree*/
        AvlTree<CarModel*, CompModelNum>* zero_score_modelIds;//zero score models tree

        /**
         * Scan models tree from base up and feels the given
         * models and types arrays
        */
        void efficiantInorder(AvlTreeNode<CarModel*>* base,
             int& amount, int& index, int* types, int* models);
        /**
         * Scan models tree by inOrder and feels the given
         * models and types arrays
        */
        void inOrder(AvlTreeNode<CarModel*>* root,
             int& amount, int& index, int* types, int* models);

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
            void insertZeroScoreModels(int& amount, int& index, int* types, int* models_nums);
    };

    /**
     * object function to compare models by TypeId
     */
    class CompTypeId
    {
        public:
            bool operator() (CarType* const type1 , CarType* const type2);
    };

    class CarDealershipManager
    {
        private:
            AvlTree<CarType*, CompTypeId> carTypes;
            AvlTree<CarModel*, CompModelSailes> modelSales;
            AvlTree<CarModel*, CompModelScore> PosModelScores;
            AvlTree<CarModel*, CompModelScore> NegModelScores;
            int types_num, num_of_models;
            
            /**The same function as in CarType
             * used to scan PosScore and NegScore trees
            */
            void efficiantInorder(AvlTreeNode<CarModel*>* base,
             int& amount, int& index, int* types, int* models);

            /*Same as in CarType*/
            void inOrder(AvlTreeNode<CarModel*>* root,
             int& amount, int& index, int* types, int* models);

            /**
             * Scan carTypes tree from base up and feels the given
             * models and types arrays
             * calls CarType.efficiantInOrder() func for each type
             */
            void efficiantInorderZeroScores(AvlTreeNode<CarType*>* base,
             int& amount, int& index, int* types, int* models);

            /*regular inOrder used by efficiantInorderZeroScores*/
            void inOrderZeroScores(AvlTreeNode<CarType*>* root,
             int& amount, int& index, int* types, int* mode);

             /*deletes all carTypes*/
            void deleteCarTypes(AvlTreeNode<CarType*>* root);
        public:
            CarDealershipManager();
            ~CarDealershipManager();
            StatusType AddCarType (int typeId, int numOfModels);
            StatusType RemoveCarType (int typeId);
            StatusType SellCar (int typeId, int modelId);
            StatusType MakeComplaint (int typeId, int modelId, int t);
            StatusType GetBestSellerModelByType (int typeId, int* modelId);
            StatusType GetWorstModels (int numOfModels, int* types, int* models);
    };

}
#endif