/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/ctvisualizationgl/processors/plotcontourtree.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>


#include <modules/ctutils/ext/ct/ContourTree/TopologicalFeatures.hpp>
#include <modules/ctutils/ext/ct/ContourTree/SimplifyCT.hpp>
#include <inviwo/core/util/filesystem.h>


#include <algorithm>

namespace {
    const int ModeFeatures = 0;
    const int ModeThreshold = 1;
} // namespace

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo PlotContourTree::processorInfo_{
    "org.inviwo.PlotContourTree",      // Class identifier
    "Plot Contour Tree",                // Display name
    "Undefined",              // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
};
const ProcessorInfo PlotContourTree::getProcessorInfo() const { return processorInfo_; }

PlotContourTree::PlotContourTree()
    : Processor()
    , nodeMeshOutport_("nodeMeshOutport")
    , arcsMeshOutport_("arcsMeshOutport")
    , _mode("mode", "Selection mode")
    , _contourTreeLevel("contourTreeLevel", "Contour Tree Level", 0.f, 0.f, 1.f)
    , _nFeatures("nFeatures", "Number of Features", 0, 0, 10000)
    , _quasiSimplificationFactor("_quasiSimplificationFactor", "Quasi Simplification Factor", 0.f, 0.f, 1.f)
    , _contourTreeFile("contourTreeFile", "Contour Tree File")
    , _fileIsDirty(false)
    , _dataIsDirty(false) {

    addPort(nodeMeshOutport_);
    addPort(arcsMeshOutport_);

    _mode.addOption("nFeatures", "Number of features", ModeFeatures);
    _mode.addOption("threshold", "Threshold", ModeThreshold);
    _mode.onChange([&]() { _dataIsDirty = true; });
    addProperty(_mode);

    _contourTreeLevel.onChange([&]() { _dataIsDirty = true; });
    addProperty(_contourTreeLevel);

    _nFeatures.onChange([&]() { _dataIsDirty = true; });
    addProperty(_nFeatures);

    _quasiSimplificationFactor.onChange([&]() { _dataIsDirty = true; });
    addProperty(_quasiSimplificationFactor);

    _contourTreeFile.onChange([&]() { _fileIsDirty = true; });
    addProperty(_contourTreeFile);
}

void PlotContourTree::process() {
    if (!filesystem::fileExists(_contourTreeFile.get() + ".dat")) {
        return;
    }


    if (_fileIsDirty) {
        std::string file = _contourTreeFile;
        try {
            _topologicalFeatures = contourtree::TopologicalFeatures();
            _topologicalFeatures.loadData(file, true);
            _dataIsDirty = true;
            _fileIsDirty = false;
        }
        catch (std::exception&) {

        }
    }

    auto traverseBranch = [&](const std::vector<contourtree::Branch>& branches) {
        auto nodeMesh = std::make_shared<BasicMesh>();

        LogInfo("Branches after simplification :" << branches.size());
        
        std::map<uint32_t, int> node_ValencyMap;

        for (size_t i = 0; i < branches.size(); i++) {

            //if (branches[i].parent == rootID) LogInfo("Root Node encountered");
            node_ValencyMap[branches[i].parent] += 1;
                
        }

        for (auto nk : node_ValencyMap)
            LogInfo("Map Key, Valencey " << nk.first << "," << nk.second);

        LogInfo("Node Valency map size :" << node_ValencyMap.size());

        return nodeMesh;

    };


    if (_dataIsDirty) {
        
        //Simplified Tree Processing
        contourtree::SimplifyCT sim;
        sim.setInput(&_topologicalFeatures.ctdata);

        sim.simplify(_topologicalFeatures.order, _nFeatures,
            _quasiSimplificationFactor, _topologicalFeatures.wts);

        
        const auto &branches = sim.branches;

        auto nodeMesh = traverseBranch(branches);


        nodeMeshOutport_.setData(nodeMesh);


        //Feature extraction

        /*std::vector<contourtree::Feature> features = [m = _mode.get(), this]() {
            switch (m) {
            case ModeFeatures:
                return _topologicalFeatures.getArcFeatures(_nFeatures, _quasiSimplificationFactor);
            case ModeThreshold:
                return _topologicalFeatures.getPartitionedExtremaFeatures(-1, _contourTreeLevel);
            default:
                assert(false);
                return std::vector<contourtree::Feature>();
            }
        }();
        LogInfo("Number of features: " << features.size());

        uint32_t size = _topologicalFeatures.ctdata.noArcs;

        // Buffer contents:
        // [0]: number of features
        // [...]: A linearized map from voxel identifier -> feature number

        auto debug = false;
        if (debug == true) {
            LogInfo("nFeatures :" << static_cast<uint32_t>(features.size()));
            LogInfo("Arcs :");

            std::stringstream ss;
            for (auto& arc : _topologicalFeatures.ctdata.arcs) {
                ss << "(ID: " << arc.id;
                ss << " " << arc.from;
                ss << " -> " << arc.to;
                ss << "),";
            }

            LogInfo(ss.str());
        }

        std::vector<uint32_t> bufferData(size + 1 + 1, static_cast<uint32_t>(-1));
        bufferData[0] = static_cast<uint32_t>(features.size());
        for (size_t i = 0; i < features.size(); ++i) {
            for (uint32_t j : features[i].arcs) {
                bufferData[j + 1] = static_cast<uint32_t>(i);
            }
        }
        */

        _dataIsDirty = false;
    }
}

}  // namespace inviwo
