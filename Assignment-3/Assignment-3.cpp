//===- SVF-Teaching Assignment 3-------------------------------------//
//
//    SVF: Static Value-Flow Analysis Framework for Source Code       
//
// Copyright (C) <2013->
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Assignment 3 : Andersen's pointer analysis
 //
 // 
 */

#include "SVF-FE/LLVMUtil.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "WPA/Andersen.h"
#include "Assignment-3.h"
using namespace SVF;
using namespace llvm;
using namespace std;


// TODO: Implement your Andersen's Algorithm here
void AndersenPTA::solveWorklist(){
        processAllAddr();

        // Keep solving until workList is empty.
        while (!isWorklistEmpty())
        {
            NodeID nodeId = popFromWorklist();
            ConstraintNode *node = consCG->getConstraintNode(nodeId);

            /// foreach o \in pts(p)
            for (NodeID o : getPts(nodeId))
            {

                /// *p = q  pts(q) \subseteq pts(o) 
                for (ConstraintEdge *edge : node->getStoreInEdges())
                {
                    if(addCopyEdge(edge->getSrcID(), o)){
                        pushIntoWorklist(edge->getSrcID());
                    }
                }
                // r = *p   pts(o) \subseteq pts(r)
                for (ConstraintEdge *edge : node->getLoadOutEdges())
                {
                    if(addCopyEdge(o, edge->getDstID())){
                        pushIntoWorklist(o);
                    }
                }
            }

            /// q = p or q = &p->f  pts(p) \subseteq pts(q)
            for (ConstraintEdge *edge : node->getDirectOutEdges())
            {
                if(unionPts(edge->getDstID(),edge->getSrcID())){
                    pushIntoWorklist(edge->getDstID());
                }
            }
        }

}

// Process all address constraints to initialize pointers' points-to sets
void AndersenPTA::processAllAddr(){
    for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end(); nodeIt != nodeEit; nodeIt++)
    {
        ConstraintNode *cgNode = nodeIt->second;
        for (ConstraintEdge* edge : cgNode->getAddrInEdges()) {
            const AddrCGEdge *addr = SVFUtil::cast<AddrCGEdge>(edge);
            NodeID dst = addr->getDstID();
            NodeID src = addr->getSrcID();
            if (addPts(dst, src))
                pushIntoWorklist(dst);
        }
    }
}
