//aimNode.cpp

#include "aimNode.h"

#include <maya/MGlobal.h>  //static class provviding common API global functions
// #include <maya/MFnTypedAttribute.h>  //typed attribute function set
#include <maya/MFnNumericAttribute.h>  //numeric attribute function set
#include <maya/MFnCompoundAttribute.h>  //compound attribute function set
#include <maya/MFnMatrixAttribute.h>  //matrix attribute function set
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MPlug.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MEulerRotation.h>
#include <maya/MDataHandle.h>

#include <cmath>  //c++ math library


MTypeId aimNode::typeId(0x80001);  //define value for typeId

//needed attributes
MObject aimNode::innTransX;
MObject aimNode::innTransY;
MObject aimNode::innTransZ;
MObject aimNode::innTrans;
MObject aimNode::outRotX;
MObject aimNode::outRotY;
MObject aimNode::outRotZ;
MObject aimNode::outRot;
MObject aimNode::driverMatrix;
MObject aimNode::upVecMatrix;

void* aimNode::creator(){
  return new aimNode();
}

MStatus aimNode::initialize(){
  MFnNumericAttribute numFn;
  MFnMatrixAttribute matrixFn;
  MFnCompoundAttribute compound;
  MFnUnitAttribute uAttr;
  MStatus stat;

  //matricies
  driverMatrix = matrixFn.create("driverMatrix", "dvm");
  addAttribute(driverMatrix);

  upVecMatrix = matrixFn.create("upVecMatrix", "uvm");
  addAttribute(upVecMatrix);

  innTransX = numFn.create("InputTranslateX", "itx", MFnNumericData::kDouble, 0, &stat);
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating aimNode inpute translate x value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(innTransX);

  innTransY = numFn.create("InputTranslateY", "ity", MFnNumericData::kDouble, 0, &stat);
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating aimNode inpute translate y value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(innTransY);

  innTransZ = numFn.create("InputTranslateZ", "itz", MFnNumericData::kDouble, 0, &stat);
  if(stat != MStatus::kSuccess)  //check status
    stat.perror("ERROR creating aimNode inpute translate z value");  //report error if fails
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(innTransZ);

  innTrans = compound.create("InputTranslate", "int");
  compound.addChild(innTransX);
  compound.addChild(innTransY);
  compound.addChild(innTransZ);
  numFn.setKeyable(true);
  numFn.setStorable(true);
  numFn.setWritable(true);
  addAttribute(innTrans);

  outRotX = uAttr.create("OutputRotateX", "orx", MFnUnitAttribute::kAngle, 0.0, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating aimNode output rotate x value");  //report error if fails
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(outRotX);

  outRotY = uAttr.create("OutputRotateY", "ory", MFnUnitAttribute::kAngle, 0.0, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating aimNode output rotate y value");  //report error if fails
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(outRotY);

  outRotZ = uAttr.create("OutputRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0, &stat);
  if(stat != MStatus::kSuccess)   //check status
    stat.perror("ERROR creating aimNode output rotate z value");  //report error if fails
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(outRotZ);

  outRot = compound.create("OutputRotate", "or");
  compound.addChild(outRotX);
  compound.addChild(outRotY);
  compound.addChild(outRotZ);
  numFn.setKeyable(false);
  numFn.setStorable(false);
  numFn.setWritable(false);
  addAttribute(outRot);

  //Maya will see when input is changed will force the node to evaluate the new output
  attributeAffects(innTrans, outRot);
  attributeAffects(upVecMatrix, outRot);
  attributeAffects(driverMatrix, outRot);

  return MS::kSuccess;
}

MStatus aimNode::compute(const MPlug& plug, MDataBlock& dataBlock){
  //trigger only when needed
  if((plug == outRot) || (plug == outRotX) || (plug == outRotY) || (plug == outRotZ)){

    //extract input data
    MMatrix driverMatV = dataBlock.inputValue(driverMatrix).asMatrix();
    MMatrix upVecMatV = dataBlock.inputValue(upVecMatrix).asMatrix();
    MVector innTransV = dataBlock.inputValue(innTrans).asVector();

    //extract positions from matricies
    MVector driverMatPos (driverMatV[3][0], driverMatV[3][1], driverMatV[3][2]);
    MVector upVecMatPos (upVecMatV[3][0], upVecMatV[3][1], upVecMatV[3][2]);

    //compute needed vectors
    MVector upVec = upVecMatPos - innTransV;
    MVector aimVec = driverMatPos - innTransV;
    upVec.normalize();
    aimVec.normalize();

    //compute cross products
    MVector cross = aimVec ^ upVec;
    upVec = cross ^ aimVec;

    double rotationMatrix[4][4] = {{aimVec.x, aimVec.y, aimVec.z, 0}, {upVec.x, upVec.y, upVec.z, 0}, {cross.x, cross.y, cross.z, 0}, {innTransV[0], innTransV[1], innTransV[2], 1}};

    //extract euler rotation
    MMatrix outRotMatrix (rotationMatrix);
    MTransformationMatrix matrixFn(outRotMatrix);
    MEulerRotation euler = matrixFn.eulerRotation();

    //set output values
    dataBlock.outputValue(outRot).set(euler.x, euler.y, euler.z);
    dataBlock.outputValue(outRot).setClean();  //clean mease has been evaluated correctly
  }

  return MS::kSuccess;
}
