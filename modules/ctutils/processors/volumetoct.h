#ifndef __VOLUME_TO_CT_H__
#define __VOLUME_TO_CT_H__

#include <modules/ctutils/ctutilsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <modules/segmentangling/common.h>
#include <inviwo/core/properties/fileproperty.h>


namespace inviwo {

class IVW_MODULE_CTUTILS_API VolumeToCT : public Processor {
public:
    VolumeToCT();
    virtual ~VolumeToCT() = default;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    virtual void process() override;

    FileProperty _baseVolume;
    FileProperty _subsampledVolumeFile;

    //FileProperty _workingDirectory;
    FileProperty _fullVolumeFile;
    FileProperty _partVolumeFile;
    StringProperty _contourTreeFile;

    ButtonProperty _loadButton;

    bool _volumeIsDirty;
};

} // namespace

#endif // __VOLUME_TO_CT_H__
