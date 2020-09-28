//aimNode.h

#ifndef aimNode_H
#define aimNode_H

#include <maya/MTypeId.h>  //Manage Maya Object type identifiers
#include <maya/MPxNode.h>  //Base class for user defined dependency nodes

class aimNode : public MPxNode{
  public:
    static MStatus initialize();  //initialize node
    static void* creator();  //create node
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);  //implements core of the node

  public:
    //needed variables
    static MTypeId typeId;
    static MObject innTransX;
    static MObject innTransY;
    static MObject innTransZ;
    static MObject innTrans;
    static MObject outRotX;
    static MObject outRotY;
    static MObject outRotZ;
    static MObject outRot;
    static MObject driverMatrix;
    static MObject upVecMatrix;

};

#endif
