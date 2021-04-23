#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "texture_2d.h"

class Thing;

class Thing
{
   private:
        int ID;
        std::string name;

  public:
        bool       isShown;
        Texture2D *texture;
        int        posX;
        int        posY;
        int        posZ;

        int         Get_ID() { return ID; }
        std::string Get_Name() { return name; }

        void        Set_ID(int newID) { ID = newID; }
        void        Set_Name(std::string newName) { name = newName; }

};

class Limb: public Thing
{
    // Perhaps we'll want to put something here eventually.
  public:
    enum LimbFunction
    {
        move,
        grip
    };
};

class Entity : public Thing
{
   private:
        std::vector<Limb*> limbs;

   public:
        Limb                *Get_Limb(int i) { return limbs[i]; }
        std::vector<Limb *> Get_Limbs() { return limbs; }

        void                Add_Limb(Limb *newLimb) { limbs.push_back(newLimb); }
};

#endif