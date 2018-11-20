#ifndef __AB_LOADCONTOURTREE_H__
#define __AB_LOADCONTOURTREE_H__

#include <modules/segmentangling/segmentanglingmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <modules/opengl/inviwoopengl.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <modules/segmentangling/common.h>

#include <modules/segmentangling/ext/contour-tree/ContourTree/TopologicalFeatures.hpp>


namespace inviwo {

class IVW_MODULE_SEGMENTANGLING_API LoadContourTree : public Processor {
public:
    LoadContourTree();
    virtual ~LoadContourTree() = default;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    virtual void process() override;
    
    ContourOutport _outportContour;
    FeatureOutport _outportFeature;
   
    OptionPropertyInt _mode;
    FloatProperty _contourTreeLevel;
    IntProperty _nFeatures;
    FloatProperty _quasiSimplificationFactor;
    StringProperty _contourTreeFile;

    contourtree::TopologicalFeatures _topologicalFeatures;

    bool _fileIsDirty;
    bool _dataIsDirty;
};

} // namespace

#endif // __AB_LOADCONTOURTREE_H__
