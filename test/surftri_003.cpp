// ========================================================================== //
//                - Class_SurfTri - Examples of usage                         //
//                                                                            //
// Example of usage for Class_SurfTri (grid manager for unstructured meshes)  //
// ========================================================================== //
// INFO                                                                       //
// ========================================================================== //
// Author   : Alessandro Alaia                                                //
// Version  : v3.0                                                            //
//                                                                            //
// All rights reserved.                                                       //
// ========================================================================== //

// ========================================================================== //
// INCLUDE                                                                    //
// ========================================================================== //

// Standard Template Library
# include <iostream>
# include <chrono>
# include <ctime>

# include <Class_SurfTri.hpp>
// -------------------------------------------------------------------------- //
void Demo_GenerationTools_2D(
        void
        ) {

    // ========================================================================== //
    // void Demo_GenerationTools_2D(                                              //
    //     void)                                                                  //
    //                                                                            //
    // Demo for generation tools.                                                 //
    // ========================================================================== //
    // INPUT                                                                      //
    // ========================================================================== //
    // - none                                                                     //
    // ========================================================================== //
    // OUTPUT                                                                     //
    // ========================================================================== //
    // - none                                                                     //
    // ========================================================================== //

    // ========================================================================== //
    // VARIABLES DECLARATION                                                      //
    // ========================================================================== //

    // Local variables
    string                  dgf_name = "./data/naca0012.dgf";
    Class_SurfTri           Tri, Tri2;
    Class_SurfTri           Edges;


    // ========================================================================== //
    // OUTPUT MESSAGE                                                             //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << "============ Class_SurfTri: generation tools demo (2D) ========" << endl;
    }

    // ========================================================================== //
    // LOAD TASSELATION FROM DGF FILE                                             //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Loading 2D surface tasselation" << endl << endl;

        // Import from dgf file ------------------------------------------------- //
        Tri.Import_dgf(dgf_name);

        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("geom.vtu");

    }

    // ========================================================================== //
    // GENERATE EDGES                                                             //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Cleaning input triangulation" << endl << endl;

        // Clean input triangulation -------------------------------------------- //
        Tri.RemoveDoubleVertex();
        Tri.ResizeVertex();
        Tri.BuildAdjacency();
        Tri.GenerateNormals();
        for (int i = 0; i < Tri.nSimplex; ++i) {
            swap(Tri.Normal[i][0], Tri.Normal[i][1]);
            Tri.Normal[i][1] = -Tri.Normal[i][1];
        } //next i

        // Build edges ---------------------------------------------------------- //

        // Build edge-vertex and simplex-edge connectivities
        Tri.BuildEdges();

        // Copy edge network into a surf_tri class
        Edges.AddVertices(Tri.Vertex);
        Edges.AddSimplicies(Tri.Edge);

        // Export edge network to .vtu file
        Edges.Export_vtu("edges.vtu");

    }

    // ========================================================================== //
    // GENERATE EDES NORMALS                                                      //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Generating edges normals" << endl << endl;

        // Generate edges normals ----------------------------------------------- //
        Tri.GenerateENormals();

        // Export results ------------------------------------------------------- //
        Edges.ExportVCData_vtu("Enormals.vtu", "n", Tri.ENormal);

    }

    // ========================================================================== //
    // GENERATE VERTEX NORMALS                                                    //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Generating vertex normals" << endl << endl;

        // Generate edges normals ----------------------------------------------- //
        Tri.GenerateVNormals();

        // Export results ------------------------------------------------------- //
        Tri.ExportVPData_vtu("Vnormals.vtu", "n", Tri.VNormal);

    }

    // ========================================================================== //
    // TEST ASSIGNAMENT OPERATORS                                                 //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Testing assignament operators" << endl << endl;

        // Makes a copy --------------------------------------------------------- //
        Tri2 = Tri;

        // Export edge and vertex normals --------------------------------------- //
        Edges.ExportVCData_vtu("Enormals_copy.vtu", "n", Tri2.ENormal);
        Tri2.ExportVPData_vtu("Vnormals_copy.vtu", "n", Tri2.VNormal);

    }

    // ========================================================================== //
    // TEST EDGE CONSTRUCTION WITH EXTERNAL VERTEX LIST                           //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Testing edge/vertex normals construction with external vertex list" << endl << endl;

        // Clean data structure ------------------------------------------------- //
        Tri2.DestroyVertex();
        Tri2.DestroyEdge();
        Tri2.DestroySimplex2Edge();
        Tri2.DestroyVNormal();
        Tri2.DestroyENormal();

        // Re-compute vertex normals -------------------------------------------- //
        Tri2.GenerateVNormals(Tri.Vertex);
        Tri2.AddVertices(Tri.Vertex);

        // Export re-generated edge and vertex normals -------------------------- //
        Edges.ExportVCData_vtu("Enormals_copy2.vtu", "n", Tri2.ENormal);
        Tri2.ExportVPData_vtu("Vnormals_copy2.vtu", "n", Tri2.VNormal);

    }


    // ========================================================================== //
    // CLOSING MESSAGE                                                            //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << "======================== DEMO: done!! =========================" << endl;
    }

    return; 
};

// ========================================================================== //
int main(
        void
        ) {

    // ========================================================================== //
    // int main(                                                                  //
    //     void)                                                                  //
    //                                                                            //
    // Demo with example of usage for Class_SurfTri variables. Main.              //
    // ========================================================================== //
    // INPUT                                                                      //
    // ========================================================================== //
    // - none                                                                     //
    // ========================================================================== //
    // OUTPUT                                                                     //
    // ========================================================================== //
    // - none                                                                     //
    // ========================================================================== //

    // ========================================================================== //
    // VARIABLES DECLARATION                                                      //
    // ========================================================================== //


    // Counters
    // none

    // Output message ------------------------------------------------------- //
    cout << "===================== Class_SurfTri DEMO ===================== " << endl;
    // Scope variables ------------------------------------------------------ //
    // none

    // Run demo ------------------------------------------------------------- //
    Demo_GenerationTools_2D();

    return 0;
}