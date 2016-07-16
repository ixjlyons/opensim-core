/* -------------------------------------------------------------------------- *
 *                       OpenSim:  testIterators.cpp                        *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2014-2014 Stanford University and the Authors                *
 * Author(s): Ayman Habib                                                     *
 * Contributer(s) :                                                           *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#include <iostream>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Common/LoadOpenSimLibrary.h>
#include <OpenSim/Auxiliary/auxiliaryTestFunctions.h>

using namespace OpenSim;
using namespace std;

// Example filter that allows for iterating only through Components that have
// state variables, used for demonstration purposes.
class ComponentWithStateVariables : public ComponentFilter {
public:
    ComponentWithStateVariables() {
        //std::cout << "ComponentWithStateVariables constructed" << std::endl;
    };
    bool isMatch(const Component& comp) const override {
        return (comp.getNumStateVariables()>0);
    };
    ~ComponentWithStateVariables() {
        //std::cout << "ComponentWithStateVariables destructed" << std::endl;
    }
    ComponentWithStateVariables* clone() const override {
        return new ComponentWithStateVariables(*this);
    }
};

const std::string modelFilename = "arm26.osim";
// TODO: Hard-coding these numbers is not ideal. As we introduce more components
// to recompose existing components, this will need continual updating. For example,
// Joint's often add PhysicalOffsetFrames to handle what used to be baked in location
// and orientation offsets.
const int expectedNumComponents = 134;
const int expectedNumJointsWithStateVariables = 2;
const int expectedNumModelComponentsWithStateVariables = 10;
// Below updated from 1 to 3 to account for offset frame and its geometry added
// to the Joint
const int expectedNumJntComponents = 3;
// Test using the iterator to skip over every other Component (Frame in this case)
// nf = 1 ground + 2 bodies + 2 joint offsets = 5, skipping - 2 = 3
const int expectedNumCountSkipFrames = 3;

void testComponentListConst() {

    Model model(modelFilename);
    model.dumpSubcomponents();

    ComponentList<const Component> componentsList = model.getComponentList();
    std::cout << "list begin: " << componentsList.begin()->getName() << std::endl;
    int numComponents = 0;
    for (ComponentList<const Component>::const_iterator it = componentsList.begin();
            it != componentsList.end();
            ++it) {
        std::cout << "Iterator is at: " << it->getFullPathName() << " <" << it->getConcreteClassName() << ">" << std::endl;
        numComponents++;
        // it->setName("this line should not compile; using const_iterator.");
    }
    
    ComponentList<const OpenSim::Body> bodiesList = model.getComponentList<OpenSim::Body>();
    int numBodies = 0;
    std::cout << "Bodies list begin: " << bodiesList.begin()->getName() << std::endl;
    for (ComponentList<OpenSim::Body>::const_iterator it = bodiesList.begin();
            it != bodiesList.end();
            ++it) {
        std::cout << "Iterator is at Body: " << it->getName() << std::endl;
        numBodies++;
    }
    // Now we try the post increment variant of the iterator
    std::cout << "Bodies list begin, using post increment: " << bodiesList.begin()->getName() << std::endl;
    int numBodiesPost = 0;
    for (ComponentList<OpenSim::Body>::const_iterator itPost = bodiesList.begin();
            itPost != bodiesList.end();
            itPost++) {
        std::cout << "Iterator is at Body: " << itPost->getName() << std::endl;
        numBodiesPost++;
    }

    int numMuscles = 0;
    std::cout << "Using range-for loop over Muscles: " << std::endl;
    ComponentList<const Muscle> musclesList = model.getComponentList<Muscle>();
    for (const Muscle& muscle : musclesList) {
        std::cout << "Iterator is at muscle: " << muscle.getName() << std::endl;
        numMuscles++;
    }
    
    int numGeomPaths = 0;
    ComponentList<const GeometryPath> geomPathList = model.getComponentList<GeometryPath>();
    for (const GeometryPath& gpath : geomPathList) {
        (void)gpath; // Suppress unused variable warning.
        numGeomPaths++;
    }
    const OpenSim::Joint& shoulderJnt = model.getJointSet().get(0);
    // cycle through components under shoulderJnt should return the Joint itself and the Coordinate
    int numJntComponents = 0;
    ComponentList<const Component> jComponentsList = shoulderJnt.getComponentList();
    std::cout << "Components/subComponents under Shoulder Joint:" << std::endl;
    for (ComponentList<Component>::const_iterator it = jComponentsList.begin();
            it != jComponentsList.end();
            ++it) {
        std::cout << "Iterator is at: " << it->getConcreteClassName() << " " << it->getFullPathName() << std::endl;
        numJntComponents++;
    }
    cout << "Num all components = " << numComponents << std::endl;
    cout << "Num bodies = " << numBodies << std::endl;
    cout << "Num Muscles = " << numMuscles << std::endl;
    cout << "Num GeometryPath components = " << numGeomPaths << std::endl;
    // Components = Model+3Body+3Marker+2(Joint+Coordinate)+6(Muscle+GeometryPath)
    // Should test against 1+#Bodies+#Markers+#Joints+#Constraints+#Coordinates+#Forces+#ForcesWithPath+..
    // Would that account for internal (split-bodies etc.?)

    // To test states we must have added the components to the system
    // which is done when the model creates and initializes the system
    SimTK::State state = model.initSystem();

    int numJointsWithStateVariables = 0;
    ComponentList<const Joint> jointsWithStates = model.getComponentList<Joint>();
    ComponentWithStateVariables myFilter;
    jointsWithStates.setFilter(myFilter); 
    for (const Joint& comp : jointsWithStates) {
        cout << comp.getConcreteClassName() << ":" << comp.getFullPathName() << endl;
        numJointsWithStateVariables++;
    }
    int numModelComponentsWithStateVariables = 0;
    ComponentList<const ModelComponent> comps = model.getComponentList<ModelComponent>();
    comps.setFilter(myFilter);
    for (const ModelComponent& comp : comps) {
        cout << comp.getConcreteClassName() << ":" << comp.getFullPathName() << endl;
        numModelComponentsWithStateVariables++;
    }
    //Now test a std::iterator method
    ComponentList<const Frame> allFrames = model.getComponentList<Frame>();
    ComponentList<Frame>::const_iterator skipIter = allFrames.begin();
    int countSkipFrames = 0;
    while (skipIter != allFrames.end()){
        cout << skipIter->getConcreteClassName() << ":" << skipIter->getFullPathName() << endl;
        std::advance(skipIter, 2);
        countSkipFrames++;
    }
    
    ASSERT(numComponents == expectedNumComponents,
            "", 0, "Number of Components mismatch");
    ASSERT(numBodies == model.getNumBodies(), "", 0, "Number of Bodies mismatch");
    ASSERT(numBodiesPost == numBodies, "", 0, "Number of Bodies post mismatch");
    ASSERT(numMuscles == model.getMuscles().getSize(), "", 0, 
        "Number of Muscles mismatch");
    ASSERT(numJointsWithStateVariables == expectedNumJointsWithStateVariables,
            "", 0, "Number of Joints with StateVariables mismatch");
    ASSERT(numModelComponentsWithStateVariables ==
           expectedNumModelComponentsWithStateVariables, "", 0, 
        "Number of Components with StateVariables mismatch");
    ASSERT(numJntComponents == expectedNumJntComponents, "", 0,
        "Number of Components within Joints mismatch");
    ASSERT(countSkipFrames == expectedNumCountSkipFrames, "", 0,
        "Number of Frames skipping every other one, mismatch");
}

// This test repeats the same tests as testComponentListConst(), but using
// updComponentList() instead, and focusing on using const iterators.
// That is, one still cannot
// modify the components, even though we are using updComponentList(). It's
// important that, even if you have a ComponentList() that does not allow
// modifying the components, it still can provide const access to the
// components.
void testComponentListNonConstWithConstIterator() {
    Model model(modelFilename);

    // Making this a const ComponentList causes us to use the const
    // begin()/end() methods, which return const_iterators, and thus avoids
    // the need for implicit conversion of non-const iterator to const_iterator.
    const ComponentList<Component> componentsList = model.updComponentList();
    std::cout << "list begin: " << componentsList.begin()->getName() << std::endl;
    int numComponents = 0;
    for (ComponentList<Component>::const_iterator it = componentsList.begin();
            it != componentsList.end();
            ++it) {
        std::cout << "Iterator is at: " << it->getFullPathName() << " <" << it->getConcreteClassName() << ">" << std::endl;
        numComponents++;
        // it->setName("this line should not compile; using const_iterator.");
    }
    
    ComponentList<OpenSim::Body> bodiesList = model.updComponentList<OpenSim::Body>();
    int numBodies = 0;
    // Test the cbegin()/cend() variants, which avoid the implicit conversion
    // from iterator to const_iterator.
    for (ComponentList<OpenSim::Body>::const_iterator it = bodiesList.cbegin();
            it != bodiesList.cend();
            ++it) {
        std::cout << "Iterator is at Body: " << it->getName() << std::endl;
        numBodies++;
    }
    // Now we try the post increment variant of the iterator
    std::cout << "Bodies list begin, using post increment: "
              << bodiesList.begin()->getName() << std::endl;
    int numBodiesPost = 0;
    for (ComponentList<OpenSim::Body>::const_iterator itPost = bodiesList.begin();
            itPost != bodiesList.end();
            itPost++) {
        std::cout << "Iterator is at Body: " << itPost->getName() << std::endl;
        numBodiesPost++;
    }

    int numMuscles = 0;
    std::cout << "Using range-for loop over Muscles: " << std::endl;
    // Make the ComponentList const to force the range for loop to use the
    // const iterator.
    const ComponentList<Muscle> musclesList = model.updComponentList<Muscle>();
    for (const Muscle& muscle : musclesList) {
        std::cout << "Iterator is at muscle: " << muscle.getName() << std::endl;
        numMuscles++;
    }
    
    int numGeomPaths = 0;
    // Make the ComponentList const to force the range for loop to use the
    // const iterator.
    const ComponentList<GeometryPath> geomPathList = model.updComponentList<GeometryPath>();
    for (const GeometryPath& gpath : geomPathList) {
        (void)gpath; // Suppress unused variable warning.
        numGeomPaths++;
    }
    OpenSim::Joint& shoulderJnt = model.getJointSet().get(0);
    // cycle through components under shoulderJnt should return the Joint
    // itself and the Coordinate
    int numJntComponents = 0;
    // This test relies on the implicit conversion from iterator to
    // const_iterator.
    ComponentList<Component> jComponentsList = shoulderJnt.updComponentList();
    std::cout << "Components/subComponents under Shoulder Joint:" << std::endl;
    for (ComponentList<Component>::const_iterator it = jComponentsList.begin();
        it != jComponentsList.end();
        ++it) {
        std::cout << "Iterator is at: " << it->getConcreteClassName() << " " << it->getFullPathName() << std::endl;
        numJntComponents++;
    }
    cout << "Num all components = " << numComponents << std::endl;
    cout << "Num bodies = " << numBodies << std::endl;
    cout << "Num Muscles = " << numMuscles << std::endl;
    cout << "Num GeometryPath components = " << numGeomPaths << std::endl;

    // To test states we must have added the components to the system
    // which is done when the model creates and initializes the system
    SimTK::State state = model.initSystem();

    int numJointsWithStateVariables = 0;
    ComponentList<Joint> jointsWithStates = model.updComponentList<Joint>();
    ComponentWithStateVariables myFilter;
    jointsWithStates.setFilter(myFilter); 
    for (const Joint& comp : jointsWithStates) {
        cout << comp.getConcreteClassName() << ":" << comp.getFullPathName() << endl;
        numJointsWithStateVariables++;
    }
    int numModelComponentsWithStateVariables = 0;
    ComponentList<ModelComponent> comps = model.updComponentList<ModelComponent>();
    comps.setFilter(myFilter);
    for (const ModelComponent& comp : comps) {
        cout << comp.getConcreteClassName() << ":" << comp.getFullPathName() << endl;
        numModelComponentsWithStateVariables++;
    }
    //Now test a std::iterator method
    ComponentList<Frame> allFrames = model.updComponentList<Frame>();
    // This line uses implicit conversion from iterator to const_iterator.
    ComponentList<Frame>::const_iterator skipIter = allFrames.begin();
    int countSkipFrames = 0;
    while (skipIter != allFrames.end()){
        cout << skipIter->getConcreteClassName() << ":" << skipIter->getFullPathName() << endl;
        std::advance(skipIter, 2);
        countSkipFrames++;
    }
    
    ASSERT(numComponents == expectedNumComponents);
    ASSERT(numBodies == model.getNumBodies());
    ASSERT(numBodiesPost == numBodies);
    ASSERT(numMuscles == model.getMuscles().getSize());
    ASSERT(numJointsWithStateVariables == expectedNumJointsWithStateVariables);
    ASSERT(numModelComponentsWithStateVariables ==
           expectedNumModelComponentsWithStateVariables);
    ASSERT(numJntComponents == expectedNumJntComponents);
    ASSERT(countSkipFrames == expectedNumCountSkipFrames);
    
    
    // It is not possible to convert const_iterator to (non-const) iterator.
    {
        // Lines are commented out b/c they don't compile. I (Chris) uncommented
        // them during development of the non-const iterators to check that
        // these lines do not copmile.
        ComponentList<Body> mutBodyList = model.updComponentList<Body>();
        // ComponentList<Body>::iterator itBody = mutBodyList.cbegin();
        // Also does not work with an abstract type.
        ComponentList<Component> mutCompList = model.updComponentList<Component>();
        // ComponentList<Component>::iterator itComp = mutCompList.cbegin();
    }
    
}

// Similar to testComponentListNonConstWithNonConstIterator, except that we
// allow modifying the elements of the list.
void testComponentListNonConstWithNonConstIterator() {
    Model model(modelFilename);

    ComponentList<Component> componentsList = model.updComponentList();
    int numComponents = 0;
    for (ComponentList<Component>::iterator it = componentsList.begin();
            it != componentsList.end();
            ++it) {
        numComponents++;
        it->setName(it->getName());
    }
    
    ComponentList<OpenSim::Body> bodiesList = model.updComponentList<OpenSim::Body>();
    int numBodies = 0;
    for (ComponentList<OpenSim::Body>::iterator it = bodiesList.begin();
            it != bodiesList.end();
            ++it) {
        numBodies++;
        it->setName(it->getName());
    }
    // Now we try the post increment variant of the iterator
    int numBodiesPost = 0;
    for (ComponentList<OpenSim::Body>::iterator itPost = bodiesList.begin();
            itPost != bodiesList.end();
            itPost++) {
        numBodiesPost++;
        itPost->setName(itPost->getName());
    }

    int numMuscles = 0;
    ComponentList<Muscle> musclesList = model.updComponentList<Muscle>();
    for (Muscle& muscle : musclesList) {
        numMuscles++;
        muscle.setName(muscle.getName());
    }
    
    int numGeomPaths = 0;
    ComponentList<GeometryPath> geomPathList = model.updComponentList<GeometryPath>();
    for (GeometryPath& gpath : geomPathList) {
        (void)gpath; // Suppress unused variable warning.
        numGeomPaths++;
        gpath.setName(gpath.getName());
    }
    OpenSim::Joint& shoulderJnt = model.getJointSet().get(0);
    // cycle through components under shoulderJnt should return the Joint
    // itself and the Coordinate
    int numJntComponents = 0;
    ComponentList<Component> jComponentsList = shoulderJnt.updComponentList();
    for (ComponentList<Component>::iterator it = jComponentsList.begin();
            it != jComponentsList.end();
            ++it) {
        numJntComponents++;
        it->setName(it->getName());
    }
    cout << "Num all components = " << numComponents << std::endl;
    cout << "Num bodies = " << numBodies << std::endl;
    cout << "Num Muscles = " << numMuscles << std::endl;
    cout << "Num GeometryPath components = " << numGeomPaths << std::endl;

    // To test states we must have added the components to the system
    // which is done when the model creates and initializes the system
    SimTK::State state = model.initSystem();

    int numJointsWithStateVariables = 0;
    ComponentList<Joint> jointsWithStates = model.updComponentList<Joint>();
    ComponentWithStateVariables myFilter;
    jointsWithStates.setFilter(myFilter); 
    for (Joint& comp : jointsWithStates) {
        numJointsWithStateVariables++;
        comp.setName(comp.getName());
    }
    int numModelComponentsWithStateVariables = 0;
    ComponentList<ModelComponent> comps = model.updComponentList<ModelComponent>();
    comps.setFilter(myFilter);
    for (ModelComponent& comp : comps) {
        numModelComponentsWithStateVariables++;
        comp.setName(comp.getName());
    }
    //Now test a std::iterator method
    ComponentList<Frame> allFrames = model.updComponentList<Frame>();
    ComponentList<Frame>::iterator skipIter = allFrames.begin();
    int countSkipFrames = 0;
    while (skipIter != allFrames.end()){
        skipIter->setName(skipIter->getName());
        std::advance(skipIter, 2);
        countSkipFrames++;
    }
    
    ASSERT(numComponents == expectedNumComponents);
    ASSERT(numBodies == model.getNumBodies());
    ASSERT(numBodiesPost == numBodies);
    ASSERT(numMuscles == model.getMuscles().getSize());
    ASSERT(numJointsWithStateVariables == expectedNumJointsWithStateVariables);
    ASSERT(numModelComponentsWithStateVariables ==
           expectedNumModelComponentsWithStateVariables);
    ASSERT(numJntComponents == expectedNumJntComponents);
    ASSERT(countSkipFrames == expectedNumCountSkipFrames);
}

// Ensure that we can compare const_iterator and (non-const) iterator.
void testComponentListComparisonOperators() {
    Model model(modelFilename);
    ComponentList<Body> list = model.updComponentList<Body>();
    ComponentList<Body>::const_iterator constIt = list.cbegin();
    ComponentList<Body>::iterator mutIt = list.begin();
    SimTK_TEST(constIt == mutIt);
    // It's important to switch the operands, since operator==() is invoked
    // on the first operand, and we want to ensure both iterators can be
    // used as the first operand.
    SimTK_TEST(mutIt == constIt);
    SimTK_TEST(constIt.equals(mutIt));
    // No support for mutIt.equals(constIt); (no implicit conversion from
    // const to non-const iterator.
    
    ++mutIt;
    SimTK_TEST(constIt != mutIt);
    SimTK_TEST(mutIt != constIt);
}

int main() {
    LoadOpenSimLibrary("osimActuators");
    SimTK_START_TEST("testIterators");
        SimTK_SUBTEST(testComponentListConst);
        SimTK_SUBTEST(testComponentListNonConstWithConstIterator);
        SimTK_SUBTEST(testComponentListNonConstWithNonConstIterator);
        SimTK_SUBTEST(testComponentListComparisonOperators);
    SimTK_END_TEST();
}



