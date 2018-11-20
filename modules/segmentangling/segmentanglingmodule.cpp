#include <modules/segmentangling/segmentanglingmodule.h>

#include <modules/opengl/shader/shadermanager.h>
#include <modules/segmentangling/processors/tetmesher.h>
#include <modules/segmentangling/processors/contourfilter.h>
#include <modules/segmentangling/processors/datapreprocessor.h>
#include <modules/segmentangling/processors/loadcontourtree.h>
#include <modules/segmentangling/processors/segmentationidraycaster.h>
//#include <modules/segmentangling/processors/straightener.h>
#include <modules/segmentangling/processors/volumecollectiongenerator.h>
#include <modules/segmentangling/processors/volumeexportgenerator.h>
#include <modules/segmentangling/processors/volumesliceoverlay.h>
//#include <modules/segmentangling/processors/yixinloader.h>
#include <modules/segmentangling/processors/topoppparameterer.h>
#include <modules/segmentangling/processors/selector.h>
#include <modules/segmentangling/processors/volumestopper.h>

namespace inviwo {

SegmentanglingModule::SegmentanglingModule(InviwoApplication* app)
    : InviwoModule(app, "Segmentangling")
{
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerPort<DataOutport<GLuint>>();
    registerPort<DataInport<GLuint>>();

    //registerPort<DataOutport<GLuint>>("bufferOutport");
    //registerPort<DataInport<GLuint>>("bufferInport");

    registerProcessor<TetMesher>();
    registerProcessor<ContourFilter>();
    registerProcessor<DataPreprocessor>();
    registerProcessor<LoadContourTree>();
    registerProcessor<SegmentationIdRaycaster>();
    //registerProcessor<Straightener>();
    registerProcessor<TopoPPParameterer>();
    registerProcessor<VolumeCollectionGenerator>();
    registerProcessor<VolumeExportGenerator>();
    registerProcessor<VolumeSliceOverlay>();
    //registerProcessor<YixinLoader>();
    registerProcessor<Selector>();
    registerProcessor<VolumeStopper>();
}

}  // namespace
