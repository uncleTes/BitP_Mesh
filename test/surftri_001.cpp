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
void Demo_CleaningTools(
        void
        ) {

    // ========================================================================== //
    // void Demo_CleaningTools(                                                   //
    //     void)                                                                  //
    //                                                                            //
    // Demo for Class_SurfTri cleaning tools.                                     //
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
    Class_SurfTri           Tri;
    ivector2D		E, S2E;

    // Counters
    // none

    // ========================================================================== //
    // OUTPUT MESSAGE                                                             //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << "============= Class_SurfTri: cleaning tools demo ==============" << endl;
    }

    // ========================================================================== //
    // LOAD TASSELATION FROM DGF FILE                                             //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Loading surface tasselation" << endl << endl;

        // Import from dgf file ------------------------------------------------- //
        Tri.Import_dgf("./data/naca0012.dgf");

        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);

        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("tri_0.vtu");

    }

    // ========================================================================== //
    // REMOVE DOUBLE VERTICES                                                     //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Removing duplicated vertices" << endl << endl;


        // Output stats --------------------------------------------------------- //
        Tri.RemoveDoubleVertex();

        cout << " test" << endl ;
        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);


        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("tri_1.vtu");
    }

    // ========================================================================== //
    // REMOVE ISOLATED SIMPLICIES                                                 //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Removing isolated simplicies" << endl << endl;

        // Output stats --------------------------------------------------------- //
        Tri.RemoveIsolatedSimplex();

        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);

        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("tri_2.vtu");
    }

    // ========================================================================== //
    // REMOVE ISOLATED VERTICES                                                   //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl << " - Removing isolated vertices" << endl << endl;

        // Output stats --------------------------------------------------------- //
        Tri.RemoveIsolatedVertex();

        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);

        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("tri_3.vtu");
    }

    // ========================================================================== //
    // BUILD ADJACENCY                                                            //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        ivector1D           list;

        // Output message ------------------------------------------------------- //
        cout << endl << " - Building adjacency matrix" << endl << endl;

        // Output stats --------------------------------------------------------- //
        Tri.BuildAdjacency();

        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);
        for (int i = 0; i < Tri.nSimplex; i++) {
            cout << Tri.Adjacency[i] << endl;
        } //next i

        // Output message ------------------------------------------------------- //
        cout << endl << " - Updating adjacency matrix" << endl << endl;

        // Update adjacencies --------------------------------------------------- //    
        list.resize(2, 0);
        list[1] = 1;
        Tri.UpdateAdjacency(list);
        for (int i = 0; i < Tri.nSimplex; i++) {
            cout << Tri.Adjacency[i] << endl;
        } //next i
    }

    // ========================================================================== //
    // REMOVE DOUBLE SIMPLICIES                                                   //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        ivector1D           list;

        // Output message ------------------------------------------------------- //
        cout << endl << " - Removing double simplicies" << endl << endl;

        // Output stats --------------------------------------------------------- //
        list = Tri.FindDoubleSimplex();
        Tri.RemoveSimplex(list);

        // Output stats --------------------------------------------------------- //
        // Tri.Stats(cout);
        for (int i = 0; i < Tri.nSimplex; i++) {
            cout << Tri.Adjacency[i] << endl;
        } //next i

        // Export to vtk format ------------------------------------------------- //
        Tri.Export_vtu("tri_4.vtu");

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
// MAIN                                                                       //
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
    Demo_CleaningTools(); 

    return 0;
}

