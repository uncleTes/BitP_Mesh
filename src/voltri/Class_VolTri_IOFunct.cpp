// ========================================================================== //
//                         - Class_VolTri -                                   //
//                                                                            //
// Grid manager for unstructured volume meshes.                               //
// ========================================================================== //
// INFO                                                                       //
// ========================================================================== //
// Author   : Alessandro Alaia                                                //
// Version  : v2.0                                                            //
//                                                                            //
// All rights reserved.                                                       //
// ========================================================================== //

// ========================================================================== //
// INCLUDES                                                                   //
// ========================================================================== //
# include "Class_VolTri.hpp"

// ========================================================================== //
// IMPLEMENTATIONS                                                            //
// ========================================================================== //

// dgf format =============================================================== //

// -------------------------------------------------------------------------- //
void Class_VolTri::Import_dgf(
    string          file_name
) {

// ========================================================================== //
// void Class_VolTri::Import_dgf(                                             //
//     string          file_name)                                             //
//                                                                            //
// Import volume triangulation from .dgf file.                                //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - file_name      : string, dgf file name                                   //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// Local variables
int                  n = 0;
DGF_obj              DGF(trim(file_name));

// Counters
// none

// ========================================================================== //
// READ MESH DATA                                                             //
// ========================================================================== //
DGF.load(nVertex, nSimplex, Vertex, Simplex);

// ========================================================================== //
// UPDATE INTERNAL FLAG                                                       //
// ========================================================================== //
DGF.load_sdata("types", n, e_type);

return; };

// -------------------------------------------------------------------------- //
void Class_VolTri::Export_dgf(
    string          file_name
) {

// ========================================================================== //
// void Class_VolTri::Export_dgf(                                             //
//     string          file_name)                                             //
//                                                                            //
// Export volume triangulation in .dgf file.                                  //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - file_name      : string, dgf file name                                   //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// Local variables
DGF_obj              DGF(trim(file_name));

// Counters
// none

// ========================================================================== //
// READ MESH DATA                                                             //
// ========================================================================== //
DGF.save(nVertex, nSimplex, Vertex, Simplex);

// ========================================================================== //
// UPDATE INTERNAL FLAG                                                       //
// ========================================================================== //
DGF.append_sdata("types", nSimplex, e_type);

return; };

        // =================================================================================== //
        // OUTPUT FUNCTIONS                                                                    //
        // =================================================================================== //

        // Export in vtu file ---------------------------------------------------------------- //
        void Class_VolTri::Export_vtu(string filename) {

        // =================================================================================== //
        // void Class_VolTri::Export_vtu(string filename)                                      //
        //                                                                                     //
        // Export volume mesh in a .vtu file.                                                  //
        // =================================================================================== //
        // INPUT                                                                               //
        // =================================================================================== //
        // - filename      : string, .vtu filename                                             //
        // =================================================================================== //
        // OUTPUT                                                                              //
        // =================================================================================== //
        // - none                                                                              //
        // =================================================================================== //

        // =================================================================================== //
        // VARIABLES DECLARATION                                                               //
        // =================================================================================== //

        // Local variables
        int                  n, dum, connect_size;
        vector<short>        types(nSimplex, 0);
        ivector1D            offset(nSimplex, 0);
        ivector1D            connectivity;
        dvector1D            vertex(3*nVertex, 0.0);
        ofstream             file_handle;

        // Counters
        int                  i, j, k;

        // =================================================================================== //
        // PREPARE OUTPUT                                                                      //
        // =================================================================================== //

        // Vertex coordinate list ------------------------------------------------------------ //
        k = 0;
        for (i = 0; i < nVertex; i++) {
            for (j = 0; j < 3; j++) {
                vertex[k] = Vertex[i][j];
                k++;
            } //next j
        } //next i

        // Simplex type ---------------------------------------------------------------------- //
        connect_size = 0;
        for (i = 0; i < nSimplex; i++) {
            connect_size += Simplex[i].size();
            types[i] = e_type[i];
        } //next i

        // Simplex-vertex connectivity ------------------------------------------------------- //
        connectivity.resize(connect_size);
        k = 0;
        for (i = 0; i < nSimplex; i++) {
            for (j = 0; j < Simplex[i].size(); j++) {
                connectivity[k] = Simplex[i][j];
                k++;
            } //next j

            // offset
            offset[i] = k;

        } //next i

        // =================================================================================== //
        // EXPORT DATA IN A .VTU FILE                                                          //
        // =================================================================================== //

        // Open .vtu file
        Open_vtu(file_handle, trim(filename));

        // Export mesh data
        Write_vtuMeshData(file_handle, nVertex, nSimplex, vertex, connectivity, offset, types);

        // Close .vtu file
        Close_vtu(file_handle);

        return; };

        // Export in vtu file ---------------------------------------------------------------- //
        void Class_VolTri::Export_CellData_vtu(string filename, dvector1D &data) {

        // =================================================================================== //
        // void Class_VolTri::Export_CellData_vtu(string filename, dvector1D &data)            //
        //                                                                                     //
        // Export cell data to vtu format.                                                     //
        // =================================================================================== //
        // INPUT                                                                               //
        // =================================================================================== //
        // - filename      : string, .vtu filename                                             //
        // - data          : dvector1D, cell data                                              //
        // =================================================================================== //
        // OUTPUT                                                                              //
        // =================================================================================== //
        // - none                                                                              //
        // =================================================================================== //

        // =================================================================================== //
        // VARIABLES DECLARATION                                                               //
        // =================================================================================== //

        // Local variables
        int                  n, dum, connect_size;
        vector<short>        types(nSimplex, 0);
        ivector1D            offset(nSimplex, 0);
        ivector1D            connectivity;
        dvector1D            vertex(3*nVertex, 0.0);
        ofstream             file_handle;

        // Counters
        int                  i, j, k;

        // =================================================================================== //
        // PREPARE OUTPUT                                                                      //
        // =================================================================================== //

        // Vertex coordinate list ------------------------------------------------------------ //
        k = 0;
        for (i = 0; i < nVertex; i++) {
            for (j = 0; j < 3; j++) {
                vertex[k] = Vertex[i][j];
                k++;
            } //next j
        } //next i

        // Simplex type ---------------------------------------------------------------------- //
        connect_size = 0;
        for (i = 0; i < nSimplex; i++) {
            connect_size += Simplex[i].size();
            types[i] = e_type[i];
        } //next i

        // Simplex-vertex connectivity ------------------------------------------------------- //
        connectivity.resize(connect_size);
        k = 0;
        for (i = 0; i < nSimplex; i++) {
            for (j = 0; j < Simplex[i].size(); j++) {
                connectivity[k] = Simplex[i][j];
                k++;
            } //next j

            // offset
            offset[i] = k;

        } //next i

        // =================================================================================== //
        // EXPORT DATA IN A .VTU FILE                                                          //
        // =================================================================================== //

        // Open .vtu file
        Open_vtu(file_handle, trim(filename));

        // Export mesh data
        Write_vtuMeshData(file_handle, nVertex, nSimplex, vertex, connectivity, offset, types);

        // Export cell Data
        Open_Data(file_handle, "CellData");
        Write_DataArray(file_handle, "density", 1, data);
        Close_Data(file_handle, "CellData");

        // Close .vtu file
        Close_vtu(file_handle);

        return; };

        // Export in vtu file ---------------------------------------------------------------- //
        void Class_VolTri::Export_CellData_vtu(string filename, ivector1D &data) {

        // =================================================================================== //
        // void Class_VolTri::Export_CellData_vtu(string filename, ivector1D &data)            //
        //                                                                                     //
        // Export cell data to vtu format.                                                     //
        // =================================================================================== //
        // INPUT                                                                               //
        // =================================================================================== //
        // - filename      : string, .vtu filename                                             //
        // - data          : dvector1D, cell data                                              //
        // =================================================================================== //
        // OUTPUT                                                                              //
        // =================================================================================== //
        // - none                                                                              //
        // =================================================================================== //

        // =================================================================================== //
        // VARIABLES DECLARATION                                                               //
        // =================================================================================== //

        // Local variables
        int                  n, dum, connect_size;
        vector<short>        types(nSimplex, 0);
        ivector1D            offset(nSimplex, 0);
        ivector1D            connectivity;
        dvector1D            vertex(3*nVertex, 0.0);
        ofstream             file_handle;

        // Counters
        int                  i, j, k;

        // =================================================================================== //
        // PREPARE OUTPUT                                                                      //
        // =================================================================================== //

        // Vertex coordinate list ------------------------------------------------------------ //
        k = 0;
        for (i = 0; i < nVertex; i++) {
            for (j = 0; j < 3; j++) {
                vertex[k] = Vertex[i][j];
                k++;
            } //next j
        } //next i

        // Simplex type ---------------------------------------------------------------------- //
        connect_size = 0;
        for (i = 0; i < nSimplex; i++) {
            connect_size += Simplex[i].size();
            types[i] = e_type[i];
        } //next i

        // Simplex-vertex connectivity ------------------------------------------------------- //
        connectivity.resize(connect_size);
        k = 0;
        for (i = 0; i < nSimplex; i++) {
            for (j = 0; j < Simplex[i].size(); j++) {
                connectivity[k] = Simplex[i][j];
                k++;
            } //next j

            // offset
            offset[i] = k;

        } //next i

        // =================================================================================== //
        // EXPORT DATA IN A .VTU FILE                                                          //
        // =================================================================================== //

        // Open .vtu file
        Open_vtu(file_handle, trim(filename));

        // Export mesh data
        Write_vtuMeshData(file_handle, nVertex, nSimplex, vertex, connectivity, offset, types);

        // Export cell Data
        Open_Data(file_handle, "CellData");
        Write_DataArray(file_handle, "density", 1, data);
        Close_Data(file_handle, "CellData");

        // Close .vtu file
        Close_vtu(file_handle);

        return; };
