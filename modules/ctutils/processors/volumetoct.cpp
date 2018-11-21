#include "volumetoct.h"


#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/io/serialization/serialization.h>
#include <inviwo/core/io/serialization/versionconverter.h>
#include <inviwo/core/interaction/events/keyboardevent.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/rendercontext.h>
#include <modules/base/algorithm/volume/volumeramsubsample.h>
#include <glm/gtx/component_wise.hpp>

#include "TopologicalFeatures.hpp"

#include <algorithm>

#include "DisjointSets.hpp"
#include "Grid3D.hpp"
#include "MergeTree.hpp"
#include "ContourTreeData.hpp"
#include "SimplifyCT.hpp"
#include "Persistence.hpp"
#include "TriMesh.hpp"
#include "TopologicalFeatures.hpp"
#include "HyperVolume.hpp"
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/io/datareaderfactory.h>

#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>


namespace inviwo {

const ProcessorInfo VolumeToCT::processorInfo_{
    "ct.volumetoct",  // Class identifier
    "Data Preprocessor",            // Display name
    "Volume Loading",            // Category
    CodeState::Experimental,             // Code state
    Tags::GL                       // Tags
};

VolumeToCT::VolumeToCT()
    : Processor()
    , _baseVolume("_baseVolume", "Base Volume")
    , _partVolumeFile("_partVolumeFile", "Volume File")
    , _subsampledVolumeFile("_subsampledVolumeFile", "Subsampled Volume File")
    , _fullVolumeFile("_fullVolumeFile", "Full Volume File")
    , _contourTreeFile("contourTreeFile", "Contour Tree File")
    , _loadButton("_loadButton", "Load")
{
    addProperty(_baseVolume);
    addProperty(_subsampledVolumeFile);

    _partVolumeFile.setReadOnly(true);
    addProperty(_partVolumeFile);
    _fullVolumeFile.setReadOnly(true);
    addProperty(_fullVolumeFile);

    //_contourTreeFile.onChange([&]() { _fileIsDirty = true; });
    _contourTreeFile.setReadOnly(true);
    addProperty(_contourTreeFile);

    _loadButton.onChange([&]() { _volumeIsDirty = true; });
    addProperty(_loadButton);
}

const ProcessorInfo VolumeToCT::getProcessorInfo() const {
    return processorInfo_;
}

#pragma optimize ("", off)
void VolumeToCT::process() {
    const bool isDirty = _volumeIsDirty;
    const bool hasBaseFile = !_baseVolume.get().empty() && filesystem::fileExists(_baseVolume.get());
    const bool hasScaledFile = !_subsampledVolumeFile.get().empty() && filesystem::fileExists(_subsampledVolumeFile.get());

    if (!isDirty || !hasBaseFile || !hasScaledFile) {
        return;
    }

    //const std::string baseVolumeFile = _baseVolume.get();
        
    // Step 1
    // Derive the dependent names
    //const std::string SubSampledPart = "_subsample.dat";
    //const std::string subSampleVolumeFile =
    //    filesystem::getFileDirectory(baseVolumeFile) + '/' +
    //    filesystem::getFileNameWithoutExtension(baseVolumeFile) + SubSampledPart;

    //if (!filesystem::fileExists(subSampleVolumeFile)) {
    //    // The subsampled volume does not exist, so we assume none of the other files
    //    // to exist either

    //    // Step 2
    //    // Load the volume

    auto rf = InviwoApplication::getPtr()->getDataReaderFactory();
    const std::string ext = filesystem::getFileExtension(_baseVolume);
    auto volreader = rf->getReaderForTypeAndExtension<VolumeSequence>(ext);
    auto fullVolume = (*(volreader->readData(_baseVolume)))[0];
    auto scaledVolume = (*(volreader->readData(_subsampledVolumeFile)))[0];

    //    

    //    // Step 3
    //    // Resample the volume
    //    // Find the minimum dimension
    //    const glm::size_t min = glm::compMin(volume->getDimensions());
    //    glm::size3_t scaleFactor = glm::size3_t(1);

    //    if (min > 256) {
    //        const glm::size_t factor = min / 256;
    //        scaleFactor = glm::size3_t(factor);
    //    }

    //    auto vol = volume->getRepresentation<VolumeRAM>();
    //    auto sample = std::make_shared<Volume>(util::volumeSubSample(vol, scaleFactor));
    //    sample->copyMetaDataFrom(*volume);
    //    sample->dataMap_ = volume->dataMap_;
    //    sample->setModelMatrix(volume->getModelMatrix());
    //    sample->setWorldMatrix(volume->getWorldMatrix());

    //    // Step 4
    //    // Write the subsampled volume to disk
    //    auto factory = getNetwork()->getApplication()->getDataWriterFactory();
    //    auto writer = factory->template getWriterForTypeAndExtension<Volume>("dat");
    //    writer->writeData(sample.get(), subSampleVolumeFile);

        // Step 5
        // Perform the contour tree computations
    const std::string baseVolumeFile = _baseVolume.get();
    const std::string subSampleVolumeFile = _subsampledVolumeFile.get();
    const std::string baseFile =
        filesystem::getFileDirectory(baseVolumeFile) + '/' +
        filesystem::getFileNameWithoutExtension(subSampleVolumeFile);

    const glm::size3_t subSampledSize = scaledVolume->getDimensions();

    scaledVolume->getRepresentation<VolumeRAM>()->dispatch<void, dispatching::filter::Scalars>(
        [&](auto ram) {

        //using ValueType = util::PrecsionValueType<decltype(ram)>::x;
        

        contourtree::Grid3D<glm::u16> grid(subSampledSize.x, subSampledSize.y, subSampledSize.z);
        grid.loadGrid(baseFile + ".raw");
        contourtree::MergeTree ct;
        contourtree::TreeType tree = contourtree::TypeJoinTree;
        ct.computeTree(&grid, tree);
        ct.output(baseFile, tree);
    });
    

    contourtree::ContourTreeData ctdata;
    ctdata.loadBinFile(baseFile);

    contourtree::SimplifyCT sim;
    sim.setInput(&ctdata);
    bool persistence = false;
    contourtree::SimFunction* simFn;
    if (persistence) {
        simFn = new contourtree::Persistence(ctdata);
    }
    else {
        simFn = new contourtree::HyperVolume(ctdata, baseFile + ".part.raw");
    }
    sim.simplify(simFn);
    sim.outputOrder(baseFile);

    // Step 6
    // Write the missing dat file for the part volume
    std::ofstream file(baseFile + ".part.dat");
    file << "Rawfile: " << filesystem::getFileNameWithoutExtension(subSampleVolumeFile) + ".part.raw" << '\n';
    file << "Resolution: " << subSampledSize.x << " " << subSampledSize.y << " " << subSampledSize.z << '\n';
    file << "Format: UINT32\n";

    const glm::size_t minSize = glm::compMin(subSampledSize);
    file << "BasisVector1: " << float(subSampledSize.x) / float(minSize) << " 0.0 0.0\n";
    file << "BasisVector2: " << "0.0 " << float(subSampledSize.y) / float(minSize) << " 0.0\n";
    file << "BasisVector3: " << "0.0 0.0 " << float(subSampledSize.z) / float(minSize) << "\n";
    file << '\n';
    //}
    
    _fullVolumeFile = baseVolumeFile;
    //_subsampledVolumeFile = subSampleVolumeFile;

    //const std::string baseFile =
    //    filesystem::getFileDirectory(baseVolumeFile) + '/' +
    //    filesystem::getFileNameWithoutExtension(subSampleVolumeFile);


    _partVolumeFile =
        filesystem::getFileDirectory(baseVolumeFile) + '/' +
        filesystem::getFileNameWithoutExtension(subSampleVolumeFile) + ".part.dat";

    _contourTreeFile = 
        filesystem::getFileDirectory(baseVolumeFile) + '/' + 
        filesystem::getFileNameWithoutExtension(subSampleVolumeFile);

    _volumeIsDirty = false;
}
#pragma optimize ("", on)


}  // namespace
