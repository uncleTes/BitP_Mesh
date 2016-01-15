// ========================================================================== //
//                  - DEMO FOR CARTESIAN MESH MANAGER -                       //
//                                                                            //
// Main                                                                       //
// ========================================================================== //
// INFO                                                                       //
// ========================================================================== //
// Author      :   Alessandro Alaia                                           //
// Version     :   v2.0                                                       //
//                                                                            //
// All rights reserved.                                                       //
// ========================================================================== //

// ========================================================================== //
// INCLUDES                                                                   //
// ========================================================================== //

// Standard Template Library
# include <iostream>

# include <Class_UCartMesh.hpp>
# include <UCartMesh.hpp>

using namespace std;

void Demo2D_UCartMesh(
        void
        ){

    // ========================================================================== //
    // void Demo_Class_UCartMesh2D(                                               //
    //     void)                                                                  //
    //                                                                            //
    // Example of usage of Class_UCartMesh2D.                                     //
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
    UCartMesh           Mesh1, Mesh2;

    // Counters
    // none

    // ========================================================================== //
    // OUTPUT MESSAGE                                                             //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        // none

        // Output message ------------------------------------------------------- //
        cout << endl;
        cout << "================= UCartMesh 2D DEMO =================" << endl;
    }

    // ========================================================================== //
    // SETTING MESH & MESH MANIPULATION                                           //
    // ========================================================================== //
    {
        // Scope variables ------------------------------------------------------ //
        std::array<double,3>        B0, B1;
        iarray3E        nc ;

        // Output message ------------------------------------------------------- //
        cout << " - Setting mesh and mesh manipulation" << endl;

        // Set mesh ------------------------------------------------------------- //
        B0.fill(0.) ; 
        B1.fill(1.) ; 
        nc.fill(100) ; 

        Mesh1.setMesh(B0, B1, nc, 2);
        Mesh2 = Mesh1;
        Mesh2.ExportVtr("./","original");

        // Scale mesh ----------------------------------------------------------- //
        std::array<double,3>        scale;
        scale.fill(2.0) ;

        Mesh2.Scale(scale);
        Mesh2.ExportVtr("./","scaled");

        // Translate mesh ------------------------------------------------------- //
        std::array<double,3>        transl;
        transl[0] = -0.507;    transl[1] = -0.523;    transl[2] = 0.0 ;
        Mesh2.Translate(transl);
        Mesh2.ExportVtr("./","translated");

    }

    // ========================================================================== //
    // CELL DATA INTERPOLATION                                                    //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        int                 J;
        vector<double>           Scalar1( Mesh1.getNCells(), 1.0);
        vector<double>           Scalar2( Mesh2.getNCells(), 0.0);
        vector<array<double,3>>           Vectorial1( Mesh1.getNCells(), std::array<double,3>() );
        vector<array<double,3>>           Vectorial2( Mesh2.getNCells(), std::array<double,3>() );

        int                 n, N ;
        vector<int>         intStencil ;
        vector<double>      intWeights ;

        std::array<double,3>            P;

        P[2] = 0.0 ;

        // Output message ------------------------------------------------------- //
        cout << " - Cell data interpolation" << endl;

        // Intialize cell data on finer mesh ------------------------------------ //
        for( J=0 ; J<Mesh1.getNCells(); ++J){
            P = Mesh1.getCellCenter(J) ;
            Scalar1[J] =  P[0] + P[1] +1.0;
            Vectorial1[J][0] = P[0] + P[1] + 1.0;
            Vectorial1[J][1] = P[0] + P[1] + 1.0;
            Vectorial1[J][2] = 0.0 ;
        } //next j

        // Interpolate cell data on the coarser mesh ---------------------------- //
        for (J = 0; J < Mesh2.getNCells(); J++) {
            P = Mesh2.getCellCenter( J ) ;

            N = Mesh1.linearCellInterpolation( P, intStencil, intWeights );

            Scalar2[J] = 0. ;
            Vectorial2[J] = {0.,0.,0.} ;
            for( n=0; n<N; ++n){
                Scalar2[J] += intWeights[n] * Scalar1[ intStencil[n] ] ;
                Vectorial2[J] = Vectorial2[J] + intWeights[n] * Vectorial1[ intStencil[n] ] ;
            };

        } //next j

        // Export vectorial data ------------------------------------------------ //
        Mesh1.ExportVtr("./", "2Doriginal_CSdata", "scalar_field", "Cell", Scalar1);
        Mesh2.ExportVtr("./", "2Dmapped_CSdata", "scalar_field", "Cell", Scalar2);
        Mesh1.ExportVtr("./", "2Doriginal_CVdata", "vectorial_field", "Cell", Vectorial1);
        Mesh2.ExportVtr("./", "2Dmapped_CVdata", "vectorial_field", "Cell", Vectorial2);
    }



    // ========================================================================== //
    // POINT DATA INTERPOLATION                                                   //
    // ========================================================================== //
    {

        // Scope variables ------------------------------------------------------ //
        int                 J ;
        vector<double>           Scalar1( Mesh1.getNNodes(), 1.0);
        vector<double>           Scalar2( Mesh2.getNNodes(), 0.0);
        vector<array<double,3>>           Vectorial1( Mesh1.getNNodes(), std::array<double,3>() );
        vector<array<double,3>>           Vectorial2( Mesh1.getNNodes(), std::array<double,3>() );

        int                 n, N ;
        vector<int>         intStencil ;
        vector<double>      intWeights ;

        std::array<double,3>            P;

        // Output message ------------------------------------------------------- //
        cout << " - Point data interpolation" << endl;

        // Intialize cell data on finer mesh ------------------------------------ //
        for (J = 0; J < Mesh1.getNNodes(); J++) {

            P = Mesh1.getNodeCoordinates(J);

            Scalar1[J]       = P[0] + P[1] + 1.0;

            Vectorial1[J][0] = P[0] + P[1] + 1.0;
            Vectorial1[J][1] = P[0] + P[1] + 1.0;
            Vectorial1[J][2] = 0.0 ;
        } //next j

        // Interpolate cell data on the coarser mesh ---------------------------- //
        for (J = 0; J < Mesh2.getNNodes(); J++) {

            P = Mesh2.getNodeCoordinates(J);

            N = Mesh1.linearNodeInterpolation( P, intStencil, intWeights );

            Scalar2[J] = 0. ;
            Vectorial2[J] = {0.,0.,0.} ;
            for( n=0; n<N; ++n){
                Scalar2[J] += intWeights[n] * Scalar1[ intStencil[n] ] ;
                Vectorial2[J] = Vectorial2[J] + intWeights[n] * Vectorial1[ intStencil[n] ] ;
            };

        } //next j

        // Export vectorial data ------------------------------------------------ //
        Mesh1.ExportVtr("./", "2Doriginal_PSdata", "scalar_field", "Point", Scalar1);
        Mesh2.ExportVtr("./", "2Dmapped_PSdata", "scalar_field", "Point", Scalar2);
        Mesh1.ExportVtr("./", "2Doriginal_PVdata", "vectorial_field", "Point", Vectorial1);
        Mesh2.ExportVtr("./", "2Dmapped_PVdata", "vectorial_field", "Point", Vectorial2);

    }

    return; 
}; 

// ========================================================================== //
// MAIN                                                                       //
int main(
        void
        ) {

    // ========================================================================== //
    // int main(                                                                  //
    //     void)                                                                  //
    //                                                                            //
    // Main function for class_UCartMesh2D demo.                                  //
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
    int         selection;

    // Counters
    // none

    // ========================================================================== //
    Demo2D_UCartMesh(); 

    return(0); 
};


